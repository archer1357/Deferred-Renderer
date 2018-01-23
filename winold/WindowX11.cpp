
#ifdef _X11

#include "WindowX11.h"

bool window_check_glx(Display *display) {
  int glx_major,glx_minor;

  return !(!glXQueryVersion(display,&glx_major,&glx_minor) ||
           ((glx_major == 1) && (glx_minor < 3)) || (glx_major < 1));
}

XVisualInfo *window_gl_visualinfo(Display *display,int screen,GLXFBConfig *fbcOut) {
   int visual_attribs[] ={
    GLX_X_RENDERABLE,True,
    GLX_DRAWABLE_TYPE,GLX_WINDOW_BIT,
    GLX_RENDER_TYPE,GLX_RGBA_BIT,
    GLX_X_VISUAL_TYPE,GLX_TRUE_COLOR,
    GLX_RED_SIZE,8,
    GLX_GREEN_SIZE,8,
    GLX_BLUE_SIZE,8,
    GLX_ALPHA_SIZE,8,
    GLX_DEPTH_SIZE,24,
    GLX_STENCIL_SIZE,8,
    GLX_DOUBLEBUFFER,True,
    //GLX_SAMPLE_BUFFERS  , 1,
    //GLX_SAMPLES         , 4,
    None
  };

  int fbcount;
  GLXFBConfig* fbc=glXChooseFBConfig(display,screen,visual_attribs,&fbcount);

  if(!fbc) {
    fprintf(stderr,"window_create: Failed to retrieve a framebuffer config.\n");
    return 0;
  }

  int best_fbc = -1,worst_fbc = -1,best_num_samp = -1,worst_num_samp = 999;
  int i;

  for(i=0; i<fbcount; ++i) {
    XVisualInfo *vi = glXGetVisualFromFBConfig(display,fbc[i]);

    if(vi) {
      int samp_buf,samples;
      glXGetFBConfigAttrib(display,fbc[i],GLX_SAMPLE_BUFFERS,&samp_buf);
      glXGetFBConfigAttrib(display,fbc[i],GLX_SAMPLES,&samples);
      fprintf(stderr,"Matching fbconfig %d, visual ID 0x%d: SAMPLE_BUFFERS=%d,SAMPLES=%d.\n",
              i,vi->visualid,samp_buf,samples);

      if(best_fbc < 0 || samp_buf && samples > best_num_samp) {
        best_fbc = i;
        best_num_samp = samples;
      }

      if(worst_fbc < 0 || !samp_buf || samples < worst_num_samp) {
        worst_fbc = i;
        worst_num_samp = samples;
      }
    }

    XFree(vi);
  }

  //

  GLXFBConfig bestFbc = fbc[best_fbc];
  *fbcOut= bestFbc;
  XFree(fbc);

  XVisualInfo *vi = glXGetVisualFromFBConfig(display,bestFbc);
  fprintf(stderr,"Chosen visual ID = 0x%d\n",vi->visualid);

  return vi;
}
bool window_gl_context(const int *context_attribs) {
  struct WindowData *data=window_data();

  if(!data->created) {
    fprintf(stderr,"window_context: window not created.\n");
    return false;
  }

  //
  if(data->ctx) {
    glXMakeCurrent(data->display,0,0);
    glXDestroyContext(data->display,data->ctx);
    data->ctx=NULL;
  }

  //
  GLXContext ctx;



  //
  int(*oldHandler)(Display*,XErrorEvent*)=XSetErrorHandler(&window_ctx_error_handler);
  data->ctxErrorOccurred = false;

  //
  ctx = data->glXCreateContextAttribsARB(data->display,
                                         data->bestFbc,0,True,
                                         context_attribs);

  //
  XSync(data->display,False);

  if(data->ctxErrorOccurred || !ctx) {

    return false;
  }

  //
  XSync(data->display,False);
  XSetErrorHandler(oldHandler);

  //
  if(data->ctxErrorOccurred || !ctx) {

    return false;
  }


  // Verifying that context is a direct context
  if(!glXIsDirect(data->display,ctx)) {
    fprintf(stderr,"window_create: Indirect GLX rendering context obtained.\n");
  } else {
    fprintf(stderr,"window_create: Direct GLX rendering context obtained.\n");
  }

  //
  glXMakeCurrent(data->display,data->win,ctx);

  //
  if(data->glXSwapIntervalMESA) {
    data->glXSwapIntervalMESA(1);
  } else if(data->glXSwapIntervalEXT) {
    data->glXSwapIntervalEXT(data->display,glXGetCurrentDrawable(),1);
  } else if(data->glXSwapIntervalSGI) {
    data->glXSwapIntervalSGI(1);
  }

  data->ctx=ctx;
  return true;
}


bool window_create(const char *caption, int width, int height) {
  struct WindowData *data=window_data();

  Display *display;
  Colormap cmap;
  Window win;
  Cursor blankCursor;
  int i;

  //
  display = XOpenDisplay(NULL);

  if(!display) {
    fprintf(stderr,"window_create: Failed to open X display.\n");
    return false;
  }

  //
 

  //
  if(! window_check_glx(display)) {
    fprintf(stderr,"window_create: Invalid GLX version.\n");

    XCloseDisplay(display);

    return false;
  }

  int screen=DefaultScreen(display);

  GLXFBConfig bestFbc;
  XVisualInfo *vi = window_gl_visualinfo(display,screen,&bestFbc);
  if(!vi) {
  fprintf(stderr,"visual info err\n");
    return false;
  }

  fprintf(stderr,"Chosen visual ID = 0x%d\n",vi->visualid);

  XSetWindowAttributes swa;

  swa.colormap=cmap=XCreateColormap(display,RootWindow(display,vi->screen),vi->visual,AllocNone);
  swa.background_pixmap=None;
  swa.border_pixel=0;
  swa.event_mask=ExposureMask|VisibilityChangeMask|StructureNotifyMask;//|
  // KeyPressMask|KeyReleaseMask;//|PointerMotionMask|ButtonPressMask|ButtonReleaseMask;

  swa.bit_gravity = StaticGravity;

  //fix width height to size outside, not inside size.
  win = XCreateWindow(display,RootWindow(display,vi->screen),
                      0,0,width,height,0,vi->depth,InputOutput,
                      vi->visual,
                      CWBorderPixel|CWColormap|CWEventMask,&swa);
  if(!win) {
    fprintf(stderr,"window_create: Failed to create window.\n");

    XFreeColormap(display,cmap);
    XCloseDisplay(display);
    return false;
  }

  XFree(vi);
  XStoreName(display,win,caption);
  XMapWindow(display,win);

  //
  const char *glxExts = glXQueryExtensionsString(display,DefaultScreen(display));

  glXCreateContextAttribsARBProc glXCreateContextAttribsARB;
  PFNGLXSWAPINTERVALMESAPROC glXSwapIntervalMESA;
  PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT;
  PFNGLXSWAPINTERVALSGIPROC glXSwapIntervalSGI;

  glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)
    glXGetProcAddressARB((const GLubyte *) "glXCreateContextAttribsARB");

  glXSwapIntervalMESA=(PFNGLXSWAPINTERVALMESAPROC)
    glXGetProcAddressARB((const GLubyte*)"glXSwapIntervalMESA");
  glXSwapIntervalEXT=(PFNGLXSWAPINTERVALEXTPROC)
    glXGetProcAddressARB((const GLubyte*)"glXSwapIntervalEXT");
  glXSwapIntervalSGI=(PFNGLXSWAPINTERVALSGIPROC)
    glXGetProcAddressARB((const GLubyte*)"glXSwapIntervalSGI");


  if(!isExtensionSupported(glxExts,"GLX_ARB_create_context") ||
     !glXCreateContextAttribsARB) {
    fprintf(stderr,"window_create: glXCreateContextAttribsARB not supported.\n");

    XDestroyWindow(display,win);
    XFreeColormap(display,cmap);
    XCloseDisplay(display);

    return false;
  }

  //
  {

    Pixmap blank;
    XColor dummy;
    char data[1] = {0};
    blank = XCreateBitmapFromData (display, win, data, 1, 1);

    if(blank == None) {

      // ffprintf(stderr,stderr, "error: out of memory.\n");
      fprintf(stderr,"window_create: failed to create blank cursor.\n");


      XDestroyWindow(display,win);
      XFreeColormap(display,cmap);
      XCloseDisplay(display);
      return false;
    }

    blankCursor = XCreatePixmapCursor(display, blank, blank, &dummy, &dummy, 0, 0);
    XFreePixmap (display, blank);
  }

  int opcode;
  {
  //   int major = 2, minor = 0;

  //   if (XIQueryVersion(display, &major, &minor) == BadRequest) {
  //     fprintf(stderr,"window_create: XI2 not available. Server supports %d.%d\n", major, minor);
  //     //todo: cleanup
  //     return false;
  //   }

    int event, error;

    if(XQueryExtension(display, "XInputExtension", &opcode, &event, &error) == 0) {
      fprintf(stderr,"window_create: X Input extension not available.\n");


      XDestroyWindow(display,win);
      XFreeColormap(display,cmap);
      XCloseDisplay(display);

      return false;
    }

    XIEventMask eventmask;
    unsigned char mask[4] =  { 0, 0, 0, 0 };

    eventmask.deviceid = XIAllMasterDevices;
    eventmask.mask_len = sizeof(mask);
    eventmask.mask = mask;

    XISetMask(mask, XI_RawMotion);
    XISetMask(mask, XI_RawButtonPress);
    XISetMask(mask, XI_RawButtonRelease);
    XISetMask(mask, XI_RawKeyPress);
    XISetMask(mask, XI_RawKeyRelease);

    XISelectEvents(display, DefaultRootWindow(display), &eventmask, 1);
  }

  //
  data->created=true;
  data->justCreated=true;

  data->ctx=0;
  data->display=display;
  data->win=win;
  data->cmap=cmap;
  data->bestFbc=bestFbc;

  data->glXCreateContextAttribsARB=glXCreateContextAttribsARB;
  data->glXSwapIntervalMESA=glXSwapIntervalMESA;;
  data->glXSwapIntervalEXT=glXSwapIntervalEXT;
  data->glXSwapIntervalSGI=glXSwapIntervalSGI;

  memset(data->inputs, WINDOW_INPUT_UP, sizeof(data->inputs));

  data->blankCursor=blankCursor;
  data->xiopcode=opcode;

  int context_attribs[] ={
    GLX_CONTEXT_MAJOR_VERSION_ARB,2,
    GLX_CONTEXT_MINOR_VERSION_ARB,0,
    GLX_CONTEXT_FLAGS_ARB,0,
    // GLX_CONTEXT_FLAGS_ARB,GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
    // GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_ES2_PROFILE_BIT_EXT,
    // GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
    // GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
    None
  };

  if(!window_gl_context(context_attribs)) {
    fprintf(stderr,"window_create:gl context failed.\n");
    //todo cleanup
    return false;
  }
  return true;
}

void window_destroy() {
  struct WindowData *data=window_data();

  if(!data->created) {
    fprintf(stderr,"window_destroy: window not created.\n");
    return;
  }

  if(data->ctx) {
    glXMakeCurrent(data->display,0,0);
    glXDestroyContext(data->display,data->ctx);
    data->ctx=0;
  }

  //
  if(data->cursorLocked) {
    XUndefineCursor(data->display, data->win);
  }

  XFreeCursor(data->display,data->blankCursor);

  //
  XDestroyWindow(data->display,data->win);
  XFreeColormap(data->display,data->cmap);
  XCloseDisplay(data->display);

  data->created=false;
}

bool window_update() {
  struct WindowData *data=window_data();

  if(!data->created) {
    fprintf(stderr,"window_update: window not created.\n");
    return false;
  }

  data->mouseX=0;
  data->mouseY=0;
  data->mouseZ=0;

  memset(data->inputText,0,sizeof(data->inputText));

  if(data->clientWidth!=-1 && data->clientHeight!=-1) {
    data->sized=false;
  }

  int i;

  for(i=0;i<256;i++) {
    if(data->inputs[i]==WINDOW_INPUT_PRESS) {
      data->inputs[i]=WINDOW_INPUT_DOWN;
    } else if(data->inputs[i]==WINDOW_INPUT_RELEASE) {
      data->inputs[i]=WINDOW_INPUT_UP;
    }
  }

  //
  XEvent ev;
  XGenericEventCookie *cookie = (XGenericEventCookie*)&ev.xcookie;
  while(XPending(data->display)) {

    XNextEvent(data->display,&ev);

    if (ev.xcookie.type == GenericEvent && ev.xcookie.extension == data->xiopcode &&
        XGetEventData(data->display, &ev.xcookie)){
      XIRawEvent *xiEvent = (XIRawEvent*)ev.xcookie.data;

      int i;
      double *raw_valuator = xiEvent->raw_values;
      double *valuator = xiEvent->valuators.values;

      switch(ev.xcookie.evtype) {
      case XI_RawButtonPress:
        if(data->focused) {
          if(xiEvent->detail == 1) {
            data->inputs[WINDOW_MOUSE_LEFT]=WINDOW_INPUT_PRESS;
          } else if(xiEvent->detail == 2) {
            data->inputs[WINDOW_MOUSE_MIDDLE]=WINDOW_INPUT_PRESS;
          } else if(xiEvent->detail == 3) {
            data->inputs[WINDOW_MOUSE_RIGHT]=WINDOW_INPUT_PRESS;
          }
        }
        break;
      case XI_RawButtonRelease:
        if(xiEvent->detail == 1 ) {//
          if(data->inputs[WINDOW_MOUSE_LEFT]!=WINDOW_INPUT_UP) {
            data->inputs[WINDOW_MOUSE_LEFT]=WINDOW_INPUT_RELEASE;
          }
        } else if(xiEvent->detail == 2) {
          if(data->inputs[WINDOW_MOUSE_MIDDLE]!=WINDOW_INPUT_UP) {
            data->inputs[WINDOW_MOUSE_MIDDLE]=WINDOW_INPUT_RELEASE;
          }
        } else if(xiEvent->detail == 3) {
          if(data->inputs[WINDOW_MOUSE_RIGHT]!=WINDOW_INPUT_UP) {
            data->inputs[WINDOW_MOUSE_RIGHT]=WINDOW_INPUT_RELEASE;
          }
        }
        break;
      case XI_RawMotion:
        for (i = 0; i < xiEvent->valuators.mask_len * 8; i++) {
          if (XIMaskIsSet(xiEvent->valuators.mask, i) ) {
            if(i==0) {//x
              data->mouseX=valuator[i];//- raw_valuator[i];
            } else if(i==1) {//y
              data->mouseY=valuator[i];//- raw_valuator[i];
            } else if(i==2) {//z
              data->mouseZ=(int)valuator[i];//- raw_valuator[i];
            }
            // fprintf(stderr,"Acceleration on valuator %d:  %f\n",  i, valuator[i]- raw_valuator[i]);
          }
        }
        break;
      case XI_RawKeyPress:
        // fprintf(stderr,"%d\n",xiEvent->detail);
        if(data->focused) {
          data->inputs[window_key_convert(xiEvent->detail)]=WINDOW_INPUT_PRESS;
        }
        break;
      case XI_RawKeyRelease:
        if(data->inputs[window_key_convert(xiEvent->detail)]!=WINDOW_INPUT_UP) {
          data->inputs[window_key_convert(xiEvent->detail)]=WINDOW_INPUT_RELEASE;
        }
        break;
      default:
        break;
      }

      // continue;
    }



    switch(ev.type) {
    case UnmapNotify:
      break;
    case MapNotify:
    case ConfigureNotify:
      data->sized=true;
      break;
    case DestroyNotify:
      return false;
    case FocusIn:
      if(ev.xfocus.mode == NotifyNormal) {
        data->focused=true;
      }

      break;
    case FocusOut:
      if(ev.xfocus.mode == NotifyNormal) {
        data->focused=false;
      }
      break;
    }


    XFreeEventData(data->display, &ev.xcookie);
  }

  //
  {
    // Window w;
    // int revert_to_return;
    // XGetInputFocus(data->display,&w,&revert_to_return);
    // if(w==data->window) {
    // }
  }

  //
  XWindowAttributes window_attributes;
  XGetWindowAttributes(data->display,data->win,&window_attributes);
  data->clientWidth=window_attributes.width;
  data->clientHeight=window_attributes.height;

  //cursor
  {
    Window window_returned;
    int root_x, root_y;
    unsigned int mask_return;
    Bool result = XQueryPointer(data->display,data->win, &window_returned,
                                &window_returned, &root_x, &root_y, &data->cursorX, &data->cursorY,
                                &mask_return);
  }

  if(data->lockCursor && data->focused) {
    XWarpPointer(data->display,data->win,data->win,0,0,data->clientWidth,data->clientHeight,
                 data->clientWidth/2,data->clientHeight/2);
    if(!data->cursorLocked) {
      XDefineCursor(data->display, data->win, data->blankCursor);
    }
    data->cursorLocked=true;
  } else {
    if(data->cursorLocked) {
      XUndefineCursor(data->display, data->win);
    }
    data->cursorLocked=false;
  }

  //
  // int event_mask=ButtonPressMask|ButtonReleaseMask|
  //   PointerMotionMask |FocusChangeMask|
  //   EnterWindowMask|LeaveWindowMask;
  // if(GrabSuccess !=XGrabPointer(data->display,data->win, True,
  //                               event_mask,GrabModeAsync,GrabModeAsync,
  //                               data->win, None, CurrentTime)) {
  // }
  // XUngrabPointer(data->display,CurrentTime);

  //
  if(!data->focused) {
    for(i=0;i<256;i++) {
      data->inputs[i]=WINDOW_INPUT_UP;
    }
  }

  //
  if(data->iconified) {
    //todo: test which is better

    //this
    // struct timespec t;
    // t.tv_sec=0;
    // t.tv_nsec=1000000;
    // nanosleep(&t,0);

    //or
    sched_yield();

    //not
    // sleep(1);//sec
  }

  //
  return true;
}

void window_swap_buffers() {
  struct WindowData *data=window_data();

  if(!data->created) {
    fprintf(stderr,"window_swap_buffers: window not created.\n");
    return;
  }

  glXSwapBuffers(data->display,data->win);
}

int window_client_width() {
  struct WindowData *data=window_data();
  return data->clientWidth;
}

int window_client_height() {
  struct WindowData *data=window_data();
  return data->clientHeight;
}

int window_cursor_x() {
  struct WindowData *data=window_data();
  return data->cursorX;
}

int window_cursor_y() {
  struct WindowData *data=window_data();
  return data->cursorY;
}

int window_mouse_x() {
  struct WindowData *data=window_data();
  return data->mouseX;
}

int window_mouse_y() {
  struct WindowData *data=window_data();
  return data->mouseY;
}

int window_mouse_z() {
  struct WindowData *data=window_data();
  return data->mouseZ;
}

bool window_iconified() {
  struct WindowData *data=window_data();
  return data->iconified;
}

bool window_focused() {
  struct WindowData *data=window_data();
  return data->focused;
}

bool window_sized() {
  struct WindowData *data=window_data();
  return data->sized;
}

void window_lock_cursor(bool lockCursor) {
  struct WindowData *data=window_data();
  data->lockCursor=lockCursor;
}
bool window_cursor_locked() {
  struct WindowData *data=window_data();
  return data->cursorLocked;
}

bool window_input_down(int key) {
  struct WindowData *data=window_data();
  return data->inputs[key]==WINDOW_INPUT_PRESS ||  data->inputs[key]==WINDOW_INPUT_DOWN;
}

bool window_input_press(int key) {
  struct WindowData *data=window_data();
  return data->inputs[key]==WINDOW_INPUT_PRESS;
}

bool window_input_release(int key) {
  struct WindowData *data=window_data();
  return data->inputs[key]==WINDOW_INPUT_RELEASE;
}

const char *window_input_text() {
  struct WindowData *data=window_data();
  return data->inputText;
}

int window_ctx_error_handler(Display *dpy,XErrorEvent *ev) {
  struct WindowData *data=window_data();
  data->ctxErrorOccurred = true;
  return 0;
}

struct WindowData *window_data() {
  static struct WindowData data;
  static bool init=false;

  if(!init) {
    init=true;

    data.ctx=0;

    data.created=false;
    data.iconified=false;
    data.focused=true;
    data.sized=true;
    data.lockCursor=false;
    data.cursorLocked=false;
    memset(data.inputs, WINDOW_INPUT_UP, sizeof(data.inputs));
  }

  return &data;
}

int window_key_convert(int vk) {
  static bool init=false;
  static unsigned int keyMap[256];

  if(!init) {
    init=true;
    memset(keyMap,WINDOW_KEY_UKNOWN, sizeof(keyMap));

    keyMap[9] = WINDOW_KEY_ESCAPE;
    keyMap[10] = WINDOW_KEY_1;
    keyMap[11] = WINDOW_KEY_2;
    keyMap[12] = WINDOW_KEY_3;
    keyMap[13] = WINDOW_KEY_4;
    keyMap[14] = WINDOW_KEY_5;
    keyMap[15] = WINDOW_KEY_6;
    keyMap[16] = WINDOW_KEY_7;
    keyMap[17] = WINDOW_KEY_8;
    keyMap[18] = WINDOW_KEY_9;
    keyMap[19] = WINDOW_KEY_0;
    keyMap[20] = WINDOW_KEY_MINUS;
    keyMap[21] = WINDOW_KEY_EQUAL;
    keyMap[22] = WINDOW_KEY_BACKSPACE;
    keyMap[23] = WINDOW_KEY_TAB;
    keyMap[24] = WINDOW_KEY_Q;
    keyMap[25] = WINDOW_KEY_W;
    keyMap[26] = WINDOW_KEY_E;
    keyMap[27] = WINDOW_KEY_R;
    keyMap[28] = WINDOW_KEY_T;
    keyMap[29] = WINDOW_KEY_Y;
    keyMap[30] = WINDOW_KEY_U;
    keyMap[31] = WINDOW_KEY_I;
    keyMap[32] = WINDOW_KEY_O;
    keyMap[33] = WINDOW_KEY_P;
    keyMap[34] = WINDOW_KEY_LEFT_BRACKET;
    keyMap[35] = WINDOW_KEY_RIGHT_BRACKET;
    keyMap[36] = WINDOW_KEY_RETURN;// Return
    keyMap[37] = WINDOW_KEY_CONTROL;// Control_L
    keyMap[38] = WINDOW_KEY_A;
    keyMap[39] = WINDOW_KEY_S;
    keyMap[40] = WINDOW_KEY_D;
    keyMap[41] = WINDOW_KEY_F;
    keyMap[42] = WINDOW_KEY_G;
    keyMap[43] = WINDOW_KEY_H;
    keyMap[44] = WINDOW_KEY_J;
    keyMap[45] = WINDOW_KEY_K;
    keyMap[46] = WINDOW_KEY_L;
    keyMap[47] = WINDOW_KEY_SEMICOLON;
    keyMap[48] = WINDOW_KEY_APOSTROPHE;
    keyMap[49] = WINDOW_KEY_GRAVE_ACCENT;
    keyMap[50] = WINDOW_KEY_SHIFT;// Shift_L
    keyMap[51] = WINDOW_KEY_BACKSLASH;
    keyMap[52] = WINDOW_KEY_Z;
    keyMap[53] = WINDOW_KEY_X;
    keyMap[54] = WINDOW_KEY_C;
    keyMap[55] = WINDOW_KEY_V;
    keyMap[56] = WINDOW_KEY_B;
    keyMap[57] = WINDOW_KEY_N;
    keyMap[58] = WINDOW_KEY_M;
    keyMap[59] = WINDOW_KEY_COMMA;
    keyMap[60] = WINDOW_KEY_PERIOD;
    keyMap[61] = WINDOW_KEY_SLASH;
    keyMap[62] = WINDOW_KEY_SHIFT;// Shift_R
    keyMap[63] = WINDOW_KEY_KP_MULTIPLY;
    keyMap[64] = WINDOW_KEY_ALT;// Alt_L
    keyMap[65] = WINDOW_KEY_SPACE;
    keyMap[66] = WINDOW_KEY_CAPS_LOCK;
    keyMap[67] = WINDOW_KEY_F1;
    keyMap[68] = WINDOW_KEY_F2;
    keyMap[69] = WINDOW_KEY_F3;
    keyMap[70] = WINDOW_KEY_F4;
    keyMap[71] = WINDOW_KEY_F5;
    keyMap[72] = WINDOW_KEY_F6;
    keyMap[73] = WINDOW_KEY_F7;
    keyMap[74] = WINDOW_KEY_F8;
    keyMap[75] = WINDOW_KEY_F9;
    keyMap[76] = WINDOW_KEY_F10;
    keyMap[77] = WINDOW_KEY_NUM_LOCK;
    keyMap[78] = WINDOW_KEY_SCROLL_LOCK;
    keyMap[79] = WINDOW_KEY_KP_7;
    keyMap[80] = WINDOW_KEY_KP_8;
    keyMap[81] = WINDOW_KEY_KP_9;
    keyMap[82] = WINDOW_KEY_KP_SUBTRACT;
    keyMap[83] = WINDOW_KEY_KP_4;
    keyMap[84] = WINDOW_KEY_KP_5;
    keyMap[85] = WINDOW_KEY_KP_6;
    keyMap[86] = WINDOW_KEY_KP_ADD;
    keyMap[87] = WINDOW_KEY_KP_1;
    keyMap[88] = WINDOW_KEY_KP_2;
    keyMap[89] = WINDOW_KEY_KP_3;
    keyMap[90] = WINDOW_KEY_KP_0;
    keyMap[91] = WINDOW_KEY_KP_DECIMAL;
    keyMap[95] = WINDOW_KEY_F11;
    keyMap[96] = WINDOW_KEY_F12;

    keyMap[104] = WINDOW_KEY_RETURN;// KP_Enter
    keyMap[105] = WINDOW_KEY_CONTROL;// Control_R
    keyMap[106] = WINDOW_KEY_KP_DIVIDE;
    keyMap[107] = WINDOW_KEY_PRINT_SCREEN;
    keyMap[108] = WINDOW_KEY_ALT;// Alt_R
    keyMap[110] = WINDOW_KEY_HOME;
    keyMap[111] = WINDOW_KEY_UP;
    keyMap[112] = WINDOW_KEY_PAGE_UP;
    keyMap[113] = WINDOW_KEY_LEFT;
    keyMap[114] = WINDOW_KEY_RIGHT;
    keyMap[115] = WINDOW_KEY_END;
    keyMap[116] = WINDOW_KEY_DOWN;
    keyMap[117] = WINDOW_KEY_PAGE_DOWN;
    keyMap[118] = WINDOW_KEY_INSERT;
    keyMap[119] = WINDOW_KEY_DELETE;

    keyMap[127] = WINDOW_KEY_PAUSE;
    keyMap[129] = WINDOW_KEY_KP_DECIMAL;
    keyMap[133] = WINDOW_KEY_WIN;// Super_L
    keyMap[134] = WINDOW_KEY_WIN;// Super_R
    keyMap[135] = WINDOW_KEY_APPS;
  }

  return (vk<0||vk>=256)?WINDOW_KEY_UKNOWN:keyMap[vk];
}

bool isExtensionSupported(const char *extList,const char *extension) {
  const char *start;
  const char *where,*terminator;

  // Extension names should not have spaces.
  where = strchr(extension,' ');

  if(where || *extension == '\0') {
    return false;
  }

  // It takes a bit of care to be fool-proof about parsing the
  // OpenGL extensions string. Don't be fooled by sub-strings,
  // etc.
  for(start=extList;;) {
    where = strstr(start,extension);

    if(!where) {
      break;
    }

    terminator = where + strlen(extension);

    if(where == start || *(where - 1) == ' ') {
      if(*terminator == ' ' || *terminator == '\0') {
        return true;
      }
    }

    start = terminator;
  }

  return false;
}

#endif
