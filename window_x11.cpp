#ifdef X11
#include "window_x11.h"

int key_convert_x11(int vk) {
  static bool init=false;
  static int keyMap[256];

  if(!init) {
    init=true;
    memset(keyMap,KEY_UKNOWN, sizeof(keyMap));

    keyMap[9] = KEY_ESCAPE;
    keyMap[10] = KEY_1;
    keyMap[11] = KEY_2;
    keyMap[12] = KEY_3;
    keyMap[13] = KEY_4;
    keyMap[14] = KEY_5;
    keyMap[15] = KEY_6;
    keyMap[16] = KEY_7;
    keyMap[17] = KEY_8;
    keyMap[18] = KEY_9;
    keyMap[19] = KEY_0;
    keyMap[20] = KEY_MINUS;
    keyMap[21] = KEY_EQUAL;
    keyMap[22] = KEY_BACKSPACE;
    keyMap[23] = KEY_TAB;
    keyMap[24] = KEY_Q;
    keyMap[25] = KEY_W;
    keyMap[26] = KEY_E;
    keyMap[27] = KEY_R;
    keyMap[28] = KEY_T;
    keyMap[29] = KEY_Y;
    keyMap[30] = KEY_U;
    keyMap[31] = KEY_I;
    keyMap[32] = KEY_O;
    keyMap[33] = KEY_P;
    keyMap[34] = KEY_LEFT_BRACKET;
    keyMap[35] = KEY_RIGHT_BRACKET;
    keyMap[36] = KEY_RETURN;// Return
    keyMap[37] = KEY_CONTROL;// Control_L
    keyMap[38] = KEY_A;
    keyMap[39] = KEY_S;
    keyMap[40] = KEY_D;
    keyMap[41] = KEY_F;
    keyMap[42] = KEY_G;
    keyMap[43] = KEY_H;
    keyMap[44] = KEY_J;
    keyMap[45] = KEY_K;
    keyMap[46] = KEY_L;
    keyMap[47] = KEY_SEMICOLON;
    keyMap[48] = KEY_APOSTROPHE;
    keyMap[49] = KEY_GRAVE_ACCENT;
    keyMap[50] = KEY_SHIFT;// Shift_L
    keyMap[51] = KEY_BACKSLASH;
    keyMap[52] = KEY_Z;
    keyMap[53] = KEY_X;
    keyMap[54] = KEY_C;
    keyMap[55] = KEY_V;
    keyMap[56] = KEY_B;
    keyMap[57] = KEY_N;
    keyMap[58] = KEY_M;
    keyMap[59] = KEY_COMMA;
    keyMap[60] = KEY_PERIOD;
    keyMap[61] = KEY_SLASH;
    keyMap[62] = KEY_SHIFT;// Shift_R
    keyMap[63] = KEY_KP_MULTIPLY;
    keyMap[64] = KEY_ALT;// Alt_L
    keyMap[65] = KEY_SPACE;
    keyMap[66] = KEY_CAPS_LOCK;
    keyMap[67] = KEY_F1;
    keyMap[68] = KEY_F2;
    keyMap[69] = KEY_F3;
    keyMap[70] = KEY_F4;
    keyMap[71] = KEY_F5;
    keyMap[72] = KEY_F6;
    keyMap[73] = KEY_F7;
    keyMap[74] = KEY_F8;
    keyMap[75] = KEY_F9;
    keyMap[76] = KEY_F10;
    keyMap[77] = KEY_NUM_LOCK;
    keyMap[78] = KEY_SCROLL_LOCK;
    keyMap[79] = KEY_KP_7;
    keyMap[80] = KEY_KP_8;
    keyMap[81] = KEY_KP_9;
    keyMap[82] = KEY_KP_SUBTRACT;
    keyMap[83] = KEY_KP_4;
    keyMap[84] = KEY_KP_5;
    keyMap[85] = KEY_KP_6;
    keyMap[86] = KEY_KP_ADD;
    keyMap[87] = KEY_KP_1;
    keyMap[88] = KEY_KP_2;
    keyMap[89] = KEY_KP_3;
    keyMap[90] = KEY_KP_0;
    keyMap[91] = KEY_KP_DECIMAL;
    keyMap[95] = KEY_F11;
    keyMap[96] = KEY_F12;

    keyMap[104] = KEY_RETURN;// KP_Enter
    keyMap[105] = KEY_CONTROL;// Control_R
    keyMap[106] = KEY_KP_DIVIDE;
    keyMap[107] = KEY_PRINT_SCREEN;
    keyMap[108] = KEY_ALT;// Alt_R
    keyMap[110] = KEY_HOME;
    keyMap[111] = KEY_UP;
    keyMap[112] = KEY_PAGE_UP;
    keyMap[113] = KEY_LEFT;
    keyMap[114] = KEY_RIGHT;
    keyMap[115] = KEY_END;
    keyMap[116] = KEY_DOWN;
    keyMap[117] = KEY_PAGE_DOWN;
    keyMap[118] = KEY_INSERT;
    keyMap[119] = KEY_DELETE;

    keyMap[127] = KEY_PAUSE;
    keyMap[129] = KEY_KP_DECIMAL;
    keyMap[133] = KEY_WIN;// Super_L
    keyMap[134] = KEY_WIN;// Super_R
    keyMap[135] = KEY_APPS;
  }

  return (vk<0||vk>=256)?KEY_UKNOWN:keyMap[vk];
}


Cursor createBlankCursor(Display *display,Window win) {
  Cursor cursor;
  Pixmap blank;
  XColor dummy;
  char img[1];

  img[0]=0;
  blank=XCreateBitmapFromData(display,win,img,1,1);

  if(blank==None) {
    fprintf(stderr,"failed to create blank cursor.\n");
    return 0;
  }

  cursor=XCreatePixmapCursor(display,blank,blank,&dummy,&dummy,0,0);
  XFreePixmap(display,blank);

  return cursor;
}


int setupXiInput(Display *display) {
   int opcode;
   Window defaultRootWin;
   defaultRootWin=DefaultRootWindow(display);

  // int major = 2, minor = 0;
   // (XIQueryVersion(display, &major, &minor) == BadRequest)

  int event, error;

  if(XQueryExtension(display, "XInputExtension", &opcode, &event, &error) == 0) {
    fprintf(stderr,"window_create: X Input extension not available.\n");
    return 0;
  }

  XIEventMask eventmask;
  unsigned char mask[4]={0,0,0,0};

  eventmask.deviceid = XIAllMasterDevices;
  eventmask.mask_len = sizeof(mask);
  eventmask.mask = mask;

  XISetMask(mask, XI_RawMotion);
  XISetMask(mask, XI_RawButtonPress);
  XISetMask(mask, XI_RawButtonRelease);
  XISetMask(mask, XI_RawKeyPress);
  XISetMask(mask, XI_RawKeyRelease);

  XISelectEvents(display, defaultRootWin, &eventmask, 1);

  return opcode;
}


void cursor_pos_x11(Display *display,Window win,int *x,int *y) {
  Window window_returned;
  int root_x, root_y;
  unsigned int mask_return;
  if(!XQueryPointer(display,win, &window_returned,&window_returned,
                    &root_x, &root_y, x, y,&mask_return)) {

  }
}


bool rawinput_msg_x11(Display *display,int opcode,XEvent *ev,
                      int *inputs,int *mx,int *my,int *mz) {
  XGenericEventCookie *cookie = &ev->xcookie;

  if(cookie->type!=GenericEvent) {
    return false;
  }

  if(cookie->extension != opcode) {
    return false;
  }

  if(!XGetEventData(display, cookie)) {
    return false;
  }


  XIRawEvent *xiEvent = (XIRawEvent*)cookie->data;


  double *raw_valuator = xiEvent->raw_values;
  double *valuator = xiEvent->valuators.values;

  if(cookie->evtype==XI_RawButtonPress) {
    if(xiEvent->detail == 1) {
      if(inputs[MOUSE_LEFT]!=INPUT_DOWN) {
        inputs[MOUSE_LEFT]=INPUT_PRESS;
      }
    } else if(xiEvent->detail == 2) {
      if(inputs[MOUSE_MIDDLE]!=INPUT_DOWN) {
        inputs[MOUSE_MIDDLE]=INPUT_PRESS;
      }
    } else if(xiEvent->detail == 3) {
      if(inputs[MOUSE_RIGHT]!=INPUT_DOWN) {
        inputs[MOUSE_RIGHT]=INPUT_PRESS;
      }
    }
  } else if(cookie->evtype==XI_RawButtonRelease) {
    if(xiEvent->detail == 1 ) {//
      if(inputs[MOUSE_LEFT]!=INPUT_UP) {
        inputs[MOUSE_LEFT]=INPUT_RELEASE;
      }
    } else if(xiEvent->detail == 2) {
      if(inputs[MOUSE_MIDDLE]!=INPUT_UP) {
        inputs[MOUSE_MIDDLE]=INPUT_RELEASE;
      }
    } else if(xiEvent->detail == 3) {
      if(inputs[MOUSE_RIGHT]!=INPUT_UP) {
        inputs[MOUSE_RIGHT]=INPUT_RELEASE;
      }
    }
  } else if(cookie->evtype==XI_RawKeyPress) {
    int k=key_convert_x11(xiEvent->detail);

    if(inputs[k]!=INPUT_DOWN) {
      inputs[k]=INPUT_PRESS;
    }
  } else if(cookie->evtype==XI_RawKeyRelease) {
    int k=key_convert_x11(xiEvent->detail);

    if(inputs[k]!=INPUT_UP) {
      inputs[k]=INPUT_RELEASE;
    }
  } else if(cookie->evtype==XI_RawMotion) {
    int i;
    for (i = 0; i < xiEvent->valuators.mask_len * 8; i++) {
      if (XIMaskIsSet(xiEvent->valuators.mask, i) ) {
        if(i==0 && valuator[i]!=0.0) {//x
          *mx=(int)valuator[i];//- raw_valuator[i];
        } else if(i==1 && valuator[i]!=0.0) {//y
          *my=(int)valuator[i];//- raw_valuator[i];
        } else if(i==2 && valuator[i]!=0.0) {//z
          *mz=(int)valuator[i];//- raw_valuator[i];
        } else if(i==3 && valuator[i]!=0.0) {//z
        }
      }
    }
  }

  return true;
}



void lock_cursor_x11(bool lock,bool *locked,Display *display,
                     Window win,Cursor blankCursor,
                     int cursorX,int cursorY) {
   //





  if(lock) {
    XWindowAttributes window_attributes;
    XGetWindowAttributes(display,win,&window_attributes);

    int w=window_attributes.width;
    int h=window_attributes.height;

    XWarpPointer(display,win,win,0,0,w,h,
                 cursorX,cursorY );


    int event_mask= ButtonPressMask|
      ButtonReleaseMask|
      // OwnerGrabButtonMask | Expose|PointerMotionHintMask|
      PointerMotionMask |FocusChangeMask|
      EnterWindowMask|LeaveWindowMask;

    if(!(*locked)) {
      XDefineCursor(display,win,blankCursor);
      int grabResult=XGrabPointer(display,win, False,
                                  event_mask,GrabModeAsync,GrabModeAsync,
                                  win, None, CurrentTime);

      if(grabResult==GrabSuccess) {
        *locked=true;
      }
    }
  } else {
    if(*locked) {
      XUndefineCursor(display,win);
       XUngrabPointer(display,CurrentTime);
    }

    *locked=false;
  }
}

bool window_check_glx(Display *display) {
  int glx_major,glx_minor;

  return !(!glXQueryVersion(display,&glx_major,&glx_minor) ||
           ((glx_major == 1) && (glx_minor < 3)) || (glx_major < 1));
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

void client_size_x11(Display *display,Window win,int *w,int *h) {
  XWindowAttributes window_attributes;
  XGetWindowAttributes(display,win,&window_attributes);
  *w=window_attributes.width;
  *h=window_attributes.height;
}

bool window_focused_x11(Display *display,Window win) {
  int revert_to_return;
  Window focus_return;
  XGetInputFocus(display,&focus_return,&revert_to_return);
  return win==focus_return;
}

bool iconic_x11(Display *display,Window win) {
  //todo test
  Atom wmState = XInternAtom(display, "_NET_WM_STATE", True);
  Atom wmStateHidden = XInternAtom(display, "_NET_WM_STATE_HIDDEN", True);
  Atom type;
  int format;
  unsigned long nItem, bytesAfter;
  unsigned char *properties = NULL;

  if(Success!=XGetWindowProperty(display, win, wmState, 0, LONG_MAX, False,
                                 AnyPropertyType, &type, &format, &nItem,
                                 &bytesAfter, &properties)) {
    return false;
  }

  //
  int i;

  //
  for(i=0; i<nItem; ++i) {
    if(properties[i]==wmStateHidden) {
      XFree(properties);
      return true;
    }
  }

  //
  XFree(properties);
  return false;
}

GLXFBConfig get_gl_bestFbc(Display *display,int screen) {
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
  GLXFBConfig *fbc=glXChooseFBConfig(display,screen,visual_attribs,&fbcount);

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

  GLXFBConfig bestFbc = fbc[best_fbc];
  XFree(fbc);

  return bestFbc;
}

bool run_gl_swap_interval(Display *display) {
  PFNGLXSWAPINTERVALMESAPROC glXSwapIntervalMESA;
  PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT;
  PFNGLXSWAPINTERVALSGIPROC glXSwapIntervalSGI;

  glXSwapIntervalMESA=(PFNGLXSWAPINTERVALMESAPROC)
    glXGetProcAddressARB((const GLubyte*)"glXSwapIntervalMESA");
  glXSwapIntervalEXT=(PFNGLXSWAPINTERVALEXTPROC)
    glXGetProcAddressARB((const GLubyte*)"glXSwapIntervalEXT");
  glXSwapIntervalSGI=(PFNGLXSWAPINTERVALSGIPROC)
    glXGetProcAddressARB((const GLubyte*)"glXSwapIntervalSGI");


  //
  if(glXSwapIntervalMESA) {
    glXSwapIntervalMESA(1);
  } else if(glXSwapIntervalEXT) {
    glXSwapIntervalEXT(display,glXGetCurrentDrawable(),1);
  } else if(glXSwapIntervalSGI) {
    glXSwapIntervalSGI(1);
  } else {
    return false;
  }

  return true;
}

bool create_window_x11(Display *display,int screen,Visual *visual,int depth,
                       const char *caption,int width,int height,
                       Colormap *cmap,Window *win) {
  Window rootWindow;
  rootWindow=RootWindow(display,screen);

  //
  *cmap=XCreateColormap(display,rootWindow,visual,AllocNone);


  XSetWindowAttributes swa;
  swa.bit_gravity = StaticGravity;
  swa.colormap=*cmap;
  swa.background_pixmap=None;
  swa.border_pixel=0;
  swa.event_mask=ExposureMask|VisibilityChangeMask|StructureNotifyMask //;
    |KeyPressMask|KeyReleaseMask|PointerMotionMask|ButtonPressMask|ButtonReleaseMask;


  // memset(&swa,0,sizeof(swa));
  // swa.override_redirect = True;

  //fix width height to size outside, not inside size.
  *win = XCreateWindow(display,rootWindow,
                       0,0,width,height,0,depth,InputOutput,
                       visual,
                       CWBorderPixel|CWColormap|CWEventMask|CWBackPixel,
                       &swa);

  //
  // win=XCreateWindow(display, DefaultRootWindow(display),
  //                   0, 0, 100, 100, 0, 0,
  //                   InputOnly, CopyFromParent,
  //                   CWEventMask|CWOverrideRedirect|CWCursor,
  //                   &swa);

  //
  if(!(*win)) {
    fprintf(stderr,"window_create: Failed to create window.\n");

    XFreeColormap(display,*cmap);
    return false;
  }

  XStoreName(display,*win,caption);
  XMapWindow(display,*win);
  XSetWindowBackground(display,*win, 0x2a2a2a);

  return true;
}

int window_ctx_error_handler(Display *dpy,XErrorEvent *ev) {
  windowData.ctxErrorOccurred = true;
  return 0;
}

bool window_gl_context(Display *display,Window win,
                       glXCreateContextAttribsARBProc glXCreateContextAttribsARB,
                       GLXFBConfig bestFbc,GLXContext *ctx,
                       int major,int minor,bool core) {

  int context_attribs[] ={
    GLX_CONTEXT_MAJOR_VERSION_ARB,major,
    GLX_CONTEXT_MINOR_VERSION_ARB,minor,
    GLX_CONTEXT_FLAGS_ARB,0,
    // GLX_CONTEXT_FLAGS_ARB,GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
    // GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_ES2_PROFILE_BIT_EXT,
    GLX_CONTEXT_PROFILE_MASK_ARB,core?GLX_CONTEXT_CORE_PROFILE_BIT_ARB:GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
    None
  };

  //
  if(*ctx) {
    glXMakeCurrent(display,0,0);
    glXDestroyContext(display,*ctx);
  }

  //
  windowData.ctxErrorOccurred=false;
  int(*oldHandler)(Display*,XErrorEvent*)=XSetErrorHandler(&window_ctx_error_handler);

  //
  *ctx=glXCreateContextAttribsARB(display,bestFbc,0,True,context_attribs);

  if(!ctx || windowData.ctxErrorOccurred) {
    fprintf(stderr,"gl context %i.%i unsupported.\n",major,minor);
    *ctx=0;
    return false;
  }

  //
  XSync(display,False);
  XSetErrorHandler(oldHandler);

  //
  glXMakeCurrent(display,win,*ctx);

  //
  fprintf(stdout,"GL %s\n",glGetString(GL_VERSION));
  fprintf(stdout,"GLSL %s\n",glGetString(GL_SHADING_LANGUAGE_VERSION));
  return true;

}

bool window_create(const char *caption,int width,int height) {
  //
  windowData.display = XOpenDisplay(NULL);

  if(!windowData.display) {
    fprintf(stderr,"window_create: Failed to open X display.\n");
    return false;
  }

  //
  if(!window_check_glx(windowData.display)) {
    //todo close display
    fprintf(stderr,"window_create: Invalid GLX version.\n");
    return false;
  }

  //
  int screen;
  screen = XDefaultScreen(windowData.display);

  //
  GLXFBConfig bestFbc=get_gl_bestFbc(windowData.display,screen);

  //
  XVisualInfo *vi = glXGetVisualFromFBConfig(windowData.display,bestFbc);

  if(!vi) {
    //todo close display
    fprintf(stderr,"Could not get visual info.\n");
    return false;
  }

  fprintf(stderr,"Chosen visual ID = 0x%d\n",vi->visualid);

  Visual *viVisual=vi->visual;
  int viScreen=vi->screen;
  int viDepth=vi->depth;
  XFree(vi);

  //
  // int depth = DefaultDepth(windowData.display, screen);
  // XVisualInfo vi;
  // XMatchVisualInfo(windowData.display, screen, depth, TrueColor, &vi);
  // fprintf(stderr,"Chosen visual ID = 0x%d\n",vi.visualid);
  // visual=vi.visual;

  //
  if(!create_window_x11(windowData.display,viScreen,viVisual,viDepth,
                        caption,width,height,
                        &windowData.cmap,&windowData.win)) {
    //todo close display
    return false;
  }

  //
  windowData.wmDelete=XInternAtom(windowData.display, "WM_DELETE_WINDOW", True);
  XSetWMProtocols(windowData.display, windowData.win, &windowData.wmDelete, 1);

  //


  windowData.xiopcode=setupXiInput(windowData.display);

  if(!windowData.xiopcode) {
    //todo destroy window
    //todo free colormap
    //todo close display
    return false;
  }

  //
  windowData.blankCursor=createBlankCursor(windowData.display,windowData.win);

  if(!windowData.blankCursor) {
    //todo destroy xi
    //todo destroy window
    //todo free colormap
    //todo close display
    return false;
  }

  //
  const char *glxExts = glXQueryExtensionsString(windowData.display,viScreen);

  if(!isExtensionSupported(glxExts,"GLX_ARB_create_context")) {
    //todo destroy blank cursor
    //todo destroy xi
    //todo destroy window
    //todo free colormap
    //todo close display

    fprintf(stderr,"window_create: glXCreateContextAttribsARB not supported.\n");
    return false;
  }

  glXCreateContextAttribsARBProc glXCreateContextAttribsARB;
  glXCreateContextAttribsARB=(glXCreateContextAttribsARBProc)
    glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");

  if(!glXCreateContextAttribsARB) {
    //todo destroy blank cursor
    //todo destroy xi
    //todo destroy window
    //todo free colormap
    //todo close display

    fprintf(stderr,"glXCreateContextAttribsARB err.\n");
    return false;
  }

  //
  windowData.ctx=0;

  //
  if(
     !window_gl_context(windowData.display,windowData.win,
                        glXCreateContextAttribsARB,bestFbc,
                        &windowData.ctx,3,3,false) &&
     !window_gl_context(windowData.display,windowData.win,
                        glXCreateContextAttribsARB,bestFbc,
                        &windowData.ctx,3,2,false) &&
     !window_gl_context(windowData.display,windowData.win,
                        glXCreateContextAttribsARB,bestFbc,
                        &windowData.ctx,3,1,false) &&
     !window_gl_context(windowData.display,windowData.win,
                        glXCreateContextAttribsARB,bestFbc,
                        &windowData.ctx,3,0,false) &&
     !window_gl_context(windowData.display,windowData.win,
                        glXCreateContextAttribsARB,bestFbc,
                        &windowData.ctx,2,1,false) &&
     !window_gl_context(windowData.display,windowData.win,
                        glXCreateContextAttribsARB,bestFbc,
                        &windowData.ctx,2,0,false)) {

    //todo destroy blank cursor
    //todo destroy xi
    //todo destroy window
    //todo free colormap
    //todo close display

    return false;
  }


  //
    if(!glXIsDirect(windowData.display,windowData.ctx)) {
    fprintf(stderr,"window_create: Indirect GLX rendering context obtained.\n");
  } else {
    fprintf(stderr,"window_create: Direct GLX rendering context obtained.\n");
  }

  //
  run_gl_swap_interval(windowData.display);

  //
  int i;

  //
  windowData.iconic=false;
  windowData.restored=false;
  windowData.sized=false;

  windowData.clientWidth=-1;
  windowData.clientHeight=-1;

  windowData.cursorX=0;
  windowData.cursorY=0;
  windowData.mouseX=0;
  windowData.mouseY=0;
  windowData.mouseZ=0;

  windowData.lockCursor=false;
  windowData.cursorLocked=false;

  for(i=0;i<INPUTS_SIZE;i++) {
    windowData.inputs[i]=INPUT_UP;
  }

  // int event_mask=ButtonPressMask|ButtonReleaseMask|
  //       PointerMotionMask |FocusChangeMask|
  //       EnterWindowMask|LeaveWindowMask;

  //     if(GrabSuccess !=XGrabPointer(windowData.display,windowData.win, True,
  //                                   event_mask,GrabModeAsync,GrabModeAsync,
  //                                   windowData.win, None, CurrentTime)) {
  //       printf("grabfail\n");
  //     }
  //
  return true;
}

void window_destroy() {
  glXMakeCurrent(windowData.display,0,0);
  glXDestroyContext(windowData.display,windowData.ctx);

  //todo destroy blank cursor
  //todo destroy xi
  XDestroyWindow(windowData.display,windowData.win);
  XFreeColormap(windowData.display,windowData.cmap);
  XCloseDisplay(windowData.display);
}

bool window_update() {
  bool focused=false;
  windowData.restored=false;

  windowData.mouseX=0;
  windowData.mouseY=0;
  windowData.mouseZ=0;
  //
  int i;

  //
  for(i=0;i<INPUTS_SIZE;i++) {
    if(windowData.inputs[i]==INPUT_PRESS) {
      windowData.inputs[i]=INPUT_DOWN;
    } else if(windowData.inputs[i]==INPUT_RELEASE) {
      windowData.inputs[i]=INPUT_UP;
    }
  }

  //
  while(1) {
    XEvent ev;

    while(XPending(windowData.display)) {
      XNextEvent(windowData.display,&ev);

      if(ev.type==DestroyNotify) {
        printf("Exit\n");
        return false;
      }

      if(ev.type==ClientMessage) {
        if ((Atom)ev.xclient.data.l[0] == windowData.wmDelete) {
          printf("Exit2\n");
          return false;
        }
      }

      rawinput_msg_x11(windowData.display,windowData.xiopcode,&ev,windowData.inputs,
                       &windowData.mouseX,&windowData.mouseY,&windowData.mouseZ);

      XFreeEventData(windowData.display, &ev.xcookie);
    }

    //

    //
    windowData.iconic=iconic_x11(windowData.display,windowData.win);

    //
    if(!windowData.iconic) {
      break;
    }

    windowData.restored=true;
    sched_yield();
  }

  //
  focused=window_focused_x11(windowData.display,windowData.win);

  //
  int clientWidth,clientHeight;

  client_size_x11(windowData.display,windowData.win,&clientWidth,&clientHeight);
  windowData.sized=windowData.clientWidth!=clientWidth || windowData.clientHeight!=clientHeight;
  windowData.clientWidth=clientWidth;
  windowData.clientHeight=clientHeight;

  //
  cursor_pos_x11(windowData.display,windowData.win,
                 &windowData.cursorX,&windowData.cursorY);

  //
  if(!windowData.cursorLocked) {
    windowData.lockedCursorX=windowData.cursorX;
    windowData.lockedCursorY=windowData.cursorY;
  }

  //
  lock_cursor_x11(windowData.lockCursor && focused,
                  &windowData.cursorLocked,
                  windowData.display,windowData.win,
                  windowData.blankCursor,
                  windowData.lockedCursorX,windowData.lockedCursorY);

  //
  if(!focused || windowData.restored) {
    for(i=0;i<INPUTS_SIZE;i++) {
      if(windowData.inputs[i]!=INPUT_UP) {
        windowData.inputs[i]=INPUT_RELEASE;
      }
    }

    windowData.cursorX=-1;
    windowData.cursorY=-1;
  }

  windowData.lockCursor=false;
  return true;
}

void window_swap_buffers() {
  glXSwapBuffers(windowData.display,windowData.win);
}

int window_client_width() {
  return windowData.clientWidth;
}

int window_client_height() {
  return windowData.clientHeight;
}

bool window_sized() {
  return windowData.sized;
}

bool window_restored() {
  return windowData.restored;
}

void window_lock_cursor() {
  windowData.lockCursor=true;
}

int window_cursor_x() {
  return windowData.cursorX;
}

int window_cursor_y() {
  return windowData.cursorY;
}

int window_mouse_x() {
  return windowData.mouseX;
}

int window_mouse_y() {
  return windowData.mouseY;
}

int window_mouse_z() {
  return windowData.mouseZ;
}

bool window_key_press(int k) {
  return windowData.inputs[k]==INPUT_PRESS;
}

bool window_key_down(int k) {
  return windowData.inputs[k]==INPUT_PRESS || windowData.inputs[k]==INPUT_DOWN;
}

bool window_key_release(int k) {
  return windowData.inputs[k]==INPUT_RELEASE;
}

#endif


/*


#include <X11/Xlib.h>
#include <X11/Xutil.h>

#ifdef USE_CANVAS
struct PixelCanvas {
  int width,height;
  GC gc;
  Pixmap pixmap;
  XImage *image;
  unsigned char *imageData;
  unsigned long red_mask;
  unsigned long green_mask;
  unsigned long blue_mask;
};
#endif

#ifdef USE_CANVAS
  struct PixelCanvas canvas;
#endif

#ifdef USE_CANVAS
bool pixel_canvas_create_x11(Display *display,Window win,int screen,XVisualInfo *vi,
                             int width,int height,
                             struct PixelCanvas *canvas) {
  unsigned int depth=24;

  canvas->width=width;
  canvas->height=height;
  canvas->gc=DefaultGC(display, 0);

  // XGCValues gcvalues;
  //   data->gc= XCreateGC(data->display, data->win, 0, &gcvalues);
  //   XChangeGC(data->display,data->gc,0,&gcvalues);
  // if (data->gc < 0) {
  //   fprintf(stderr, "XCreateGC: \n");
  // }
  // XSetForeground(data->display, data->gc, BlackPixel(data->display, screen));
  // XSetBackground(data->display, data->gc, WhitePixel(data->display, screen));

    XSetForeground(display,canvas->gc,WhitePixel(display,screen));
    XSetFillStyle(display,canvas->gc,FillSolid);

  // XSetFillStyle(data->display, data->gc, FillOpaqueStippled);
  //printf("depth %i\n",depth);
  //
//  data->surfaceWidth=width;
 // data->surfaceHeight=height;

  canvas->pixmap=XCreatePixmap(display,win,width,height,depth);

  // if(!data->pixmap) {
  //   fprintf(stderr,"window_create: pixmap fail.\n");
  //   return false;
  // }

  // data->image=XGetImage(data->display,data->win, 0,0, width, height, AllPlanes, XYPixmap);
  // Visual *defaultVisual=DefaultVisual(data->display,screen);
  canvas->imageData=(unsigned char*)malloc(width*height*4);

  canvas->image=XCreateImage(display, CopyFromParent, depth, ZPixmap, 0,
                           canvas->imageData, width, height, 32, 0);

  canvas->red_mask=vi->red_mask;
  canvas->green_mask=vi->green_mask;
  canvas->blue_mask=vi->blue_mask;
  printf("%lu %lu %lu\n",vi->red_mask,vi->green_mask,vi->blue_mask);

  if(!canvas->image) {
    fprintf(stderr,"window_create: ximage fail.\n");
    return false;
  }

  return  true;
}

void pixel_canvas_paint_x11(Display *display,Window win,
                            const unsigned char *pixels,const char *text,
                            struct PixelCanvas *canvas) {

  memcpy(canvas->imageData,pixels,canvas->width*canvas->height*4);
  // XSetForeground(data->display, data->gc, 0x000000);
    // XFillRectangle(data->display, data->pixmap, data->gc, 0, 0, 200, 100);
  // XCopyPlane(data->display, data->pixmap, data->win, data->gc,
  //            0, 0,
  //            data->surfaceWidth, data->surfaceHeight,
  //            0, 0,
  //            1);


  XPutImage(display, canvas->pixmap, canvas->gc, canvas->image,
            0, 0, 0, 0, canvas->width, canvas->height);

  int text_len=strlen(text);
  XDrawString (display, canvas->pixmap, canvas->gc,
                0,20, text, text_len);
  XCopyArea(display, canvas->pixmap, win, canvas->gc, 0,0,
            canvas->width, canvas->height,  0, 0);


  //XClearWindow (data->display, data->win);
  // XPutImage (data->display, data->win, data->gc,data->image, 0, 0, 0, 0,
  //            data->surfaceWidth, data->surfaceHeight);

    // int direction;
    // int ascent;
    // int descent;
    // XCharStruct overall;
  // XTextExtents (text_box.font, text_box.text, text_box.text_len,
  //                 & direction, & ascent, & descent, & overall);



  XSync(display, False);
 // XFlush(data->display);
}

void window_canvas_paint(const unsigned char *pixels,const char *text) {
  struct WindowData *data=window_data();

  pixel_canvas_paint_x11(data->display,data->win,
                          pixels, text,
                         &data->canvas);
}

#endif

*/
