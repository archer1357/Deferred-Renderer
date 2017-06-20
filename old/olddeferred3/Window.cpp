#include "Window.h"


#ifdef LINUX
static bool isExtensionSupported(const char *extList,const char *extension)
{
  const char *start;
  const char *where,*terminator;

  /* Extension names should not have spaces. */
  where = strchr(extension,' ');
  if(where || *extension == '\0')
    return false;

  /* It takes a bit of care to be fool-proof about parsing the
  OpenGL extensions string. Don't be fooled by sub-strings,
  etc. */
  for(start=extList;;) {
    where = strstr(start,extension);

    if(!where)
      break;

    terminator = where + strlen(extension);

    if(where == start || *(where - 1) == ' ')
    if(*terminator == ' ' || *terminator == '\0')
      return true;

    start = terminator;
  }

  return false;
}
static bool ctxErrorOccurred = false;
static int ctxErrorHandler(Display *dpy,XErrorEvent *ev)
{
  ctxErrorOccurred = true;
  return 0;
}
#endif
MyWindow::MyWindow(const std::string &title,int width,int height)
: initError(false),clientWidth(0),clientHeight(0),iconified(false),focused(true),sized(false),
justCreated(true),mouseMoveCallback(0),inputCallback(0),lockCursor(false),foreground(true)

{
#ifdef WIN32
  hglrc=0;
  hdc=0;

  //
  HINSTANCE hInstance=GetModuleHandle(0);
  WNDCLASSEX wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  wcex.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(IDI_APPLICATION));
  wcex.hCursor = LoadCursor(NULL,IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
  wcex.lpszMenuName = NULL;
#ifdef UNICODE
  wcex.lpszClassName = L"win32app";
#else
  wcex.lpszClassName = "win32app";
#endif
  wcex.hIconSm = LoadIcon(wcex.hInstance,MAKEINTRESOURCE(IDI_APPLICATION));

  if(!RegisterClassEx(&wcex)) {
    std::cout << "MyWindow : Call to RegisterClassEx failed!\n";
  }

#ifdef UNICODE
  wchar_t title2[256];
  MultiByteToWideChar(CP_ACP,0,title.c_str(),-1,title2,256);
#else
  const char *title2=title.c_str();
#endif

  hWnd = CreateWindow(wcex.lpszClassName,title2,WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT,CW_USEDEFAULT,width,height,NULL,NULL,hInstance,NULL);

  SetWindowLongPtr(hWnd,GWL_USERDATA,(LONG_PTR)this);

  ShowWindow(hWnd,SW_SHOW);


  //
  hdc= GetDC(hWnd);

  PIXELFORMATDESCRIPTOR pfd;
  ZeroMemory(&pfd,sizeof(pfd));

  pfd.nSize = sizeof(pfd);
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_GENERIC_ACCELERATED | PFD_DOUBLEBUFFER;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = 24;
  pfd.cRedBits = pfd.cGreenBits = pfd.cBlueBits = 8;
  pfd.cDepthBits = 32;

  int iPixelFormat = ChoosePixelFormat(hdc,&pfd);

  if(iPixelFormat == 0) {
    std::cout << "MyWindow : ChoosePixelFormat failed.\n";
    initError=true;
    return;
  }

  if(SetPixelFormat(hdc,iPixelFormat,&pfd) != TRUE) {
    std::cout << "MyWindow : SetPixelFormat failed.\n";
    initError=true;
    return;
  }

  //
  HGLRC tempContext = wglCreateContext(hdc);
  wglMakeCurrent(hdc,tempContext);


  //
  PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB=
    (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");


  PFNWGLSWAPINTERVALEXTPROC wglSwapInterval=
    (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
  //
  wglMakeCurrent(0,0);
  wglDeleteContext(tempContext);


  //
  int attribs[] ={
    WGL_CONTEXT_MAJOR_VERSION_ARB,3,
    WGL_CONTEXT_MINOR_VERSION_ARB,3,
    WGL_CONTEXT_FLAGS_ARB,0,
    0
  };

  hglrc=wglCreateContextAttribsARB(hdc,0,attribs);
  for(int i=2;i>=0;i--) {
    if(!hglrc) {
      attribs[3]=i;
      hglrc=wglCreateContextAttribsARB(hdc,0,attribs);
    }
  }
  if(!hglrc) {
    std::cout << "OpenGL 3+ not supported.\n";
    initError=true;
    return;
  }

  wglMakeCurrent(hdc,hglrc);
  wglSwapInterval(1);

  //

  RAWINPUTDEVICE Rid[2];
  Rid[0].usUsagePage = (USHORT)0x01;//HID_USAGE_PAGE_GENERIC;
  Rid[0].usUsage = (USHORT)0x02;//HID_USAGE_GENERIC_MOUSE;
  Rid[0].dwFlags = RIDEV_INPUTSINK;
  Rid[0].hwndTarget = hWnd;

  Rid[1].usUsagePage = (USHORT)0x01;//HID_USAGE_PAGE_GENERIC;
  Rid[1].usUsage = (USHORT)0x06;//HID_USAGE_GENERIC_KEYBOARD;
  Rid[1].dwFlags = RIDEV_INPUTSINK;
  Rid[1].hwndTarget = hWnd;

  RegisterRawInputDevices(Rid,2,sizeof(RAWINPUTDEVICE));

  //
  //inputCodeMap[65]=keyA;
  //inputCodeMap[68]=keyD;
  //inputCodeMap[83]=keyS;
  //inputCodeMap[87]=keyW;
#endif

#ifdef LINUX
  // bool ctxErrorOccurred=false;
  display = XOpenDisplay(NULL);

  if(!display) {
    std::cout << "Window : Failed to open X display.\n";
    initError=true;
    return;
  }

  static int visual_attribs[] ={
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

  int glx_major,glx_minor;

  if(!glXQueryVersion(display,&glx_major,&glx_minor) ||
    ((glx_major == 1) && (glx_minor < 3)) || (glx_major < 1)) {
    std::cout << "Window : Invalid GLX version.\n";
    initError=true;
    return;
  }

  int fbcount;
  GLXFBConfig* fbc = glXChooseFBConfig(display,DefaultScreen(display),visual_attribs,&fbcount);

  if(!fbc) {
    std::cout << "Window :Failed to retrieve a framebuffer config.\n";
    initError=true;
    return;
  }

  int best_fbc = -1,worst_fbc = -1,best_num_samp = -1,worst_num_samp = 999;

  for(int i=0; i<fbcount; ++i) {
    XVisualInfo *vi = glXGetVisualFromFBConfig(display,fbc[i]);
    if(vi) {
      int samp_buf,samples;
      glXGetFBConfigAttrib(display,fbc[i],GLX_SAMPLE_BUFFERS,&samp_buf);
      glXGetFBConfigAttrib(display,fbc[i],GLX_SAMPLES,&samples);
      std::cout << "Matching fbconfig " << i
        <<", visual ID 0x" << vi->visualid
        << ": SAMPLE_BUFFERS = " << samp_buf
        <<", SAMPLES = " << samples
        <<"\n";


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

  XVisualInfo *vi = glXGetVisualFromFBConfig(display,bestFbc);
  std::cout << "Chosen visual ID = 0x" << vi->visualid <<"\n";

  XSetWindowAttributes swa;

  swa.colormap = cmap = XCreateColormap(display,
    RootWindow(display,vi->screen),
    vi->visual,AllocNone);
  swa.background_pixmap = None;
  swa.border_pixel      = 0;
  swa.event_mask        = ExposureMask | VisibilityChangeMask |KeyPressMask | PointerMotionMask    |StructureNotifyMask;


  swa.bit_gravity = StaticGravity;

  win = XCreateWindow(display,RootWindow(display,vi->screen),
    0,0,100,100,0,vi->depth,InputOutput,
    vi->visual,
    CWBorderPixel|CWColormap|CWEventMask,&swa);
  if(!win) {
    std::cout << "Window : Failed to create window.\n";
    initError=true;
    return;
  }

  XFree(vi);
  XStoreName(display,win,title.c_str());
  XMapWindow(display,win);

  const char *glxExts = glXQueryExtensionsString(display,DefaultScreen(display));

  glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
  glXCreateContextAttribsARB = (glXCreateContextAttribsARBProc)
    glXGetProcAddressARB((const GLubyte *) "glXCreateContextAttribsARB");

  ctx = 0;

  ctxErrorOccurred = false;
  int(*oldHandler)(Display*,XErrorEvent*) =
    XSetErrorHandler(&ctxErrorHandler);

  if(!isExtensionSupported(glxExts,"GLX_ARB_create_context") ||
    !glXCreateContextAttribsARB) {
    std::cout << "Window : glXCreateContextAttribsARB() not found.\n";
    initError=true;
    return;
  } else {
    int context_attribs[] ={
      GLX_CONTEXT_MAJOR_VERSION_ARB,3,
      GLX_CONTEXT_MINOR_VERSION_ARB,0,
      GLX_CONTEXT_FLAGS_ARB,0,
      None
    };

    ctx = glXCreateContextAttribsARB(display,bestFbc,0,
      True,context_attribs);

    XSync(display,False);

    if(!ctxErrorOccurred && ctx) {
      std::cout << "Created GL 3.0 context\n";
    } else {
      std::cout << "Window : Failed to create GL 3.0 context.\n";
      initError=true;
      return;
    }
  }
  //
  XSync(display,False);
  XSetErrorHandler(oldHandler);

  if(ctxErrorOccurred || !ctx) {
    std::cout << "Window : Failed to create an OpenGL context.\n";
    initError=true;
    return;
  }

  // Verifying that context is a direct context
  if(!glXIsDirect(display,ctx)) {
    std::cout << "Indirect GLX rendering context obtained.\n";
  } else {
    std::cout << "Direct GLX rendering context obtained.\n";
  }

  //
  glXMakeCurrent(display,win,ctx);

  if(PFNGLXSWAPINTERVALMESAPROC glXSwapIntervalMESA=
    (PFNGLXSWAPINTERVALMESAPROC)
    glXGetProcAddressARB((const GLubyte *)"glXSwapIntervalMESA")) {
    glXSwapIntervalMESA(1);
  } else if(PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT=
    (PFNGLXSWAPINTERVALEXTPROC)
    glXGetProcAddressARB((const GLubyte *)"glXSwapIntervalEXT")) {
    glXSwapIntervalEXT(display,glXGetCurrentDrawable(),1);
  } else if(PFNGLXSWAPINTERVALSGIPROC glXSwapIntervalSGI=
    (PFNGLXSWAPINTERVALSGIPROC)
    glXGetProcAddressARB((const GLubyte *)"glXSwapIntervalSGI")) {
    glXSwapIntervalSGI(1);
  }
#endif


}

MyWindow::~MyWindow() {
#ifdef WIN32
  //
  CURSORINFO cursorInfo;
  cursorInfo.cbSize=sizeof(CURSORINFO);

  if(GetCursorInfo(&cursorInfo) && cursorInfo.flags==0) {
    ShowCursor(TRUE);
  }

  //
  wglMakeCurrent(NULL,NULL);
  wglDeleteContext(hglrc);

  CloseWindow(hWnd);
  HINSTANCE hInstance=GetModuleHandle(0);

#ifdef UNICODE
  UnregisterClass(L"win32app",hInstance);
#else
  UnregisterClass("win32app",hInstance);
#endif
#endif


#ifdef LINUX
  glXMakeCurrent(display,0,0);
  glXDestroyContext(display,ctx);

  XDestroyWindow(display,win);
  XFreeColormap(display,cmap);
  XCloseDisplay(display);
#endif

}

bool MyWindow::run() {
  if(!justCreated) {
    sized=false;
  }

  justCreated=false;


#ifdef WIN32
  while(PeekMessage(&msg,NULL,0,0,PM_REMOVE) && msg.message!=WM_QUIT) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
 
  if(msg.message==WM_QUIT) { //(int)msg.wParam;
    return false;
  }
#endif

#ifdef LINUX
  while(XPending(display)) {
    // Watch for new X events
    XNextEvent(display,&newEvent);

    switch(newEvent.type) {
    case UnmapNotify:
      break;
    case MapNotify:
    case ConfigureNotify:
      XGetWindowAttributes(display,win,&winData);
      clientHeight = winData.height;
      clientWidth = winData.width;
      sized=true;
      break;
    case MotionNotify:
      //rcx.nMousePosX = newEvent.xmotion.x;
      //rcx.nMousePosY = newEvent.xmotion.y;
      break;
    case KeyPress:
      if(newEvent.xkey.keycode==0x09) {
        return false;
      }
      break;
    case DestroyNotify:
      return false;
    }
  }
#endif

#ifdef WIN32
  foreground=GetForegroundWindow()==hWnd;
#endif

  if(!isFocused()) {
    inputCodeDown.clear();
  }

  if(lockCursor && isFocused()) {
#ifdef WIN32
    RECT rect;
    GetWindowRect(hWnd,&rect);
    SetCursorPos((rect.right-rect.left)/2+rect.left,(rect.bottom-rect.top)/2+rect.top);

    //
    CURSORINFO cursorInfo;
    cursorInfo.cbSize=sizeof(CURSORINFO);

    if(GetCursorInfo(&cursorInfo) && cursorInfo.flags!=0) {
      ShowCursor(FALSE);
    }
#endif

#ifdef LINUX
#endif
  } else {
#ifdef WIN32
    //
    CURSORINFO cursorInfo;
    cursorInfo.cbSize=sizeof(CURSORINFO);

    if(GetCursorInfo(&cursorInfo) && cursorInfo.flags==0) {
      ShowCursor(TRUE);
    }
#endif

#ifdef LINUX
#endif
  }

  return true;
}
void MyWindow::swapBuffers() {
#ifdef WIN32
  SwapBuffers(hdc);
#endif

#ifdef LINUX
  glXSwapBuffers(display,win);
#endif
}
void MyWindow::makeContext() {
#ifdef WIN32
  wglMakeCurrent(hdc,hglrc);
#endif


#ifdef LINUX
#endif
}
int MyWindow::getClientWidth() {
  return clientWidth;
}
int MyWindow::getClientHeight() {
  return clientHeight;
}
bool MyWindow::isIconified() {
  return iconified;
}
bool MyWindow::isFocused() {
  return focused && foreground;
}
bool MyWindow::isSized() {
  return sized;
}
bool MyWindow::isInitError() {
  return initError;
}

void MyWindow::setMouseMoveCallback(MouseMoveCallback mouseMoveCallback) {
  this->mouseMoveCallback=mouseMoveCallback;
}

void MyWindow::setInputCallback(InputCallback inputCallback) {
  this->inputCallback=inputCallback;

}
void MyWindow::setLockCursor(bool lockCursor) {
  this->lockCursor=lockCursor;
}
#ifdef WIN32
LRESULT CALLBACK MyWindow::WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
  MyWindow *window = (MyWindow*)GetWindowLong(hWnd,GWL_USERDATA);
  if(message==WM_INPUT) {
    UINT dwSize = 40;
    static BYTE lpb[40];
    GetRawInputData((HRAWINPUT)lParam,RID_INPUT,lpb,&dwSize,sizeof(RAWINPUTHEADER));
    RAWINPUT* raw = (RAWINPUT*)lpb;

    if(raw->header.dwType == RIM_TYPEMOUSE) {
      if(raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) {
        bool alreadyDown=window->inputCodeDown.find(VK_LBUTTON)==window->inputCodeDown.end();
        window->inputCodeDown.insert(VK_LBUTTON);

        if(window->inputCallback) {
          window->inputCallback(VK_LBUTTON,alreadyDown?inputDown:inputPress);
        }
      } else if(raw->data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP) {
        window->inputCodeDown.erase(VK_LBUTTON);

        if(window->inputCallback) {
          window->inputCallback(VK_LBUTTON,inputUp);
        }
      } else if(raw->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN) {
        bool alreadyDown=window->inputCodeDown.find(VK_MBUTTON)==window->inputCodeDown.end();
        window->inputCodeDown.insert(VK_MBUTTON);

        if(window->inputCallback) {
          window->inputCallback(VK_MBUTTON,alreadyDown?inputDown:inputPress);
        }
      } else if(raw->data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP) {
        window->inputCodeDown.erase(VK_MBUTTON);

        if(window->inputCallback) {
          window->inputCallback(VK_MBUTTON,inputUp);
        }
      } else if(raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) {
        bool alreadyDown=window->inputCodeDown.find(VK_RBUTTON)==window->inputCodeDown.end();
        window->inputCodeDown.insert(VK_RBUTTON);

        if(window->inputCallback) {
          window->inputCallback(VK_RBUTTON,alreadyDown?inputDown:inputPress);
        }
      } else if(raw->data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP) {
        window->inputCodeDown.erase(VK_RBUTTON);

        if(window->inputCallback) {
          window->inputCallback(VK_RBUTTON,inputUp);
        }
      } else if(raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_1_DOWN) {
      } else if(raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_1_UP) {
      } else if(raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_2_DOWN) {
      } else if(raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_2_UP) {
      } else if(raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_3_DOWN) {
      } else if(raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_3_UP) {
      } else if(raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN) {
      } else if(raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP) {
      } else if(raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN) {
      } else if(raw->data.mouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP) {
      } else if(raw->data.mouse.usButtonFlags & RI_MOUSE_WHEEL) {
        int z=raw->data.mouse.usButtonData;
      } else {
        int x = raw->data.mouse.lLastX;
        int y = raw->data.mouse.lLastY;

        if(window->mouseMoveCallback && window->isFocused()) {
          window->mouseMoveCallback(x,y);
        }
      }
    } else if(raw->header.dwType == RIM_TYPEKEYBOARD) {
      USHORT key=raw->data.keyboard.VKey;
      bool up=raw->data.keyboard.Flags & RI_KEY_BREAK;

      //std::cout << key << " " << up << std::endl;
      //InputCode code=inputUndefined;

      //auto it=window->inputCodeMap.find(key);

      //if(it!=window->inputCodeMap.end()) {
      //  code=it->second;
      //}

      if(!up) {
        bool alreadyDown=window->inputCodeDown.find(key)!=window->inputCodeDown.end();
        window->inputCodeDown.insert(key);

        if(window->inputCallback && window->isFocused()) {
          window->inputCallback(key,alreadyDown?inputDown:inputPress);
        }
      } else{

        window->inputCodeDown.erase(key);
        if(window->inputCallback && window->isFocused()) {
          window->inputCallback(key,inputUp);
        }
      }
    }
  } else if(message==WM_DESTROY) {
    PostQuitMessage(0);
  } else if(message==WM_SIZE) {
    window->clientWidth=LOWORD(lParam);
    window->clientHeight=HIWORD(lParam);
    window->sized=true;
    window->iconified=(wParam==SIZE_MINIMIZED);
  } else if(message==WM_KILLFOCUS) {
    window->focused=false;
  } else if(message==WM_SETFOCUS) {
    window->focused=true;
  } else if(message==WM_KEYDOWN) {
    if(wParam==VK_ESCAPE) {
      PostQuitMessage(0);
    }
  } else {
    return DefWindowProc(hWnd,message,wParam,lParam);
  }

  return 0;
}
#endif