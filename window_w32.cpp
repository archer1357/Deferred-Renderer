#ifdef WIN32
#include "window_w32.h"

int key_convert_win32(int vk) {
  static bool init=false;
  static int keyMap[256];

  if(!init) {
    init=true;
    memset(keyMap,KEY_UKNOWN, sizeof(keyMap));

    keyMap[VK_SPACE]=KEY_SPACE;
    keyMap[VK_OEM_7]=KEY_APOSTROPHE;
    keyMap[VK_OEM_COMMA]=KEY_COMMA;
    keyMap[VK_OEM_COMMA]=KEY_MINUS;
    keyMap[VK_OEM_PERIOD]=KEY_PERIOD;
    keyMap[VK_OEM_2]=KEY_SLASH;

    keyMap[VK_0]=KEY_0;
    keyMap[VK_1]=KEY_1;
    keyMap[VK_2]=KEY_2;
    keyMap[VK_3]=KEY_3;
    keyMap[VK_4]=KEY_4;
    keyMap[VK_5]=KEY_5;
    keyMap[VK_6]=KEY_6;
    keyMap[VK_7]=KEY_7;
    keyMap[VK_8]=KEY_8;
    keyMap[VK_9]=KEY_9;

    keyMap[VK_OEM_1]=KEY_SEMICOLON;
    keyMap[VK_OEM_PLUS]=KEY_EQUAL;

    keyMap[VK_A]=KEY_A;
    keyMap[VK_B]=KEY_B;
    keyMap[VK_C]=KEY_C;
    keyMap[VK_D]=KEY_D;
    keyMap[VK_E]=KEY_E;
    keyMap[VK_F]=KEY_F;
    keyMap[VK_G]=KEY_G;
    keyMap[VK_H]=KEY_H;
    keyMap[VK_I]=KEY_I;
    keyMap[VK_J]=KEY_J;
    keyMap[VK_K]=KEY_K;
    keyMap[VK_L]=KEY_L;
    keyMap[VK_M]=KEY_M;
    keyMap[VK_N]=KEY_N;
    keyMap[VK_O]=KEY_O;
    keyMap[VK_P]=KEY_P;
    keyMap[VK_Q]=KEY_Q;
    keyMap[VK_R]=KEY_R;
    keyMap[VK_S]=KEY_S;
    keyMap[VK_T]=KEY_T;
    keyMap[VK_U]=KEY_U;
    keyMap[VK_V]=KEY_V;
    keyMap[VK_W]=KEY_W;
    keyMap[VK_X]=KEY_X;
    keyMap[VK_Y]=KEY_Y;
    keyMap[VK_Z]=KEY_Z;

    keyMap[VK_OEM_4]=KEY_LEFT_BRACKET;
    keyMap[VK_OEM_5]=KEY_BACKSLASH;
    keyMap[VK_OEM_6]=KEY_RIGHT_BRACKET;
    keyMap[VK_OEM_3]=KEY_GRAVE_ACCENT;

    keyMap[VK_ESCAPE]=KEY_ESCAPE;
    keyMap[VK_RETURN]=KEY_RETURN;
    keyMap[VK_TAB]=KEY_TAB;
    keyMap[VK_BACK]=KEY_BACKSPACE;
    keyMap[VK_INSERT]=KEY_INSERT;
    keyMap[VK_DELETE]=KEY_DELETE;
    keyMap[VK_RIGHT]=KEY_RIGHT;
    keyMap[VK_LEFT]=KEY_LEFT;
    keyMap[VK_DOWN]=KEY_DOWN;
    keyMap[VK_UP]=KEY_UP;
    keyMap[VK_PRIOR]=KEY_PAGE_UP;
    keyMap[VK_NEXT]=KEY_PAGE_DOWN;
    keyMap[VK_HOME]=KEY_HOME;
    keyMap[VK_END]=KEY_END;
    keyMap[VK_CAPITAL]=KEY_CAPS_LOCK;
    keyMap[VK_SCROLL]=KEY_SCROLL_LOCK;
    keyMap[VK_NUMLOCK]=KEY_NUM_LOCK;
    keyMap[VK_SNAPSHOT]=KEY_PRINT_SCREEN;
    keyMap[VK_PAUSE]=KEY_PAUSE;

    keyMap[VK_F1]=KEY_F1;
    keyMap[VK_F2]=KEY_F2;
    keyMap[VK_F3]=KEY_F3;
    keyMap[VK_F4]=KEY_F4;
    keyMap[VK_F5]=KEY_F5;
    keyMap[VK_F6]=KEY_F6;
    keyMap[VK_F7]=KEY_F7;
    keyMap[VK_F8]=KEY_F8;
    keyMap[VK_F9]=KEY_F9;
    keyMap[VK_F10]=KEY_F10;
    keyMap[VK_F11]=KEY_F11;
    keyMap[VK_F12]=KEY_F12;

    keyMap[VK_NUMPAD0]=KEY_KP_0;
    keyMap[VK_NUMPAD1]=KEY_KP_1;
    keyMap[VK_NUMPAD2]=KEY_KP_2;
    keyMap[VK_NUMPAD3]=KEY_KP_3;
    keyMap[VK_NUMPAD4]=KEY_KP_4;
    keyMap[VK_NUMPAD5]=KEY_KP_5;
    keyMap[VK_NUMPAD6]=KEY_KP_6;
    keyMap[VK_NUMPAD7]=KEY_KP_7;
    keyMap[VK_NUMPAD8]=KEY_KP_8;
    keyMap[VK_NUMPAD9]=KEY_KP_9;

    keyMap[VK_DECIMAL]=KEY_KP_DECIMAL;
    keyMap[VK_DIVIDE]=KEY_KP_DIVIDE;
    keyMap[VK_MULTIPLY]=KEY_KP_MULTIPLY;
    keyMap[VK_SUBTRACT]=KEY_KP_SUBTRACT;
    keyMap[VK_ADD]=KEY_KP_ADD;

    keyMap[VK_SHIFT]=KEY_SHIFT;
    keyMap[VK_CONTROL]=KEY_CONTROL;
    keyMap[VK_MENU]=KEY_ALT;
    keyMap[VK_LWIN]=KEY_WIN;
    keyMap[VK_APPS]=KEY_APPS;

    keyMap[VK_LBUTTON]=MOUSE_LEFT;
    keyMap[VK_RBUTTON]=MOUSE_RIGHT;
    keyMap[VK_MBUTTON]=MOUSE_MIDDLE;
  }

  return (vk<0||vk>=256)?KEY_UKNOWN:keyMap[vk];
}


bool rawinput_init_win32(HWND hWnd) {
  RAWINPUTDEVICE Rid[2];
  ZeroMemory(&Rid,sizeof(RAWINPUTDEVICE));

  Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
  Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
  Rid[0].dwFlags = 0;//RIDEV_INPUTSINK;
  Rid[0].hwndTarget = hWnd;

  Rid[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
  Rid[1].usUsage = HID_USAGE_GENERIC_KEYBOARD;//RIDEV_DEVNOTIFY
  Rid[1].dwFlags = 0;//RIDEV_INPUTSINK;
  Rid[1].hwndTarget = hWnd;

  return RegisterRawInputDevices(Rid,2,sizeof(RAWINPUTDEVICE))==TRUE;
}

void rawinput_msg_win32(LPARAM lParam,int *inputs,int *mx,int *my,int *mz) {
  // UINT dwSize = 40;
  // static BYTE lpb[40];
  // GetRawInputData((HRAWINPUT)lParam,RID_INPUT,lpb,
  //                 &dwSize,sizeof(RAWINPUTHEADER));
  // RAWINPUT* raw = (RAWINPUT*)lpb;

  RAWINPUT raw;
  UINT DataSize=sizeof(RAWINPUT);
  GetRawInputData((HRAWINPUT)lParam,RID_INPUT,&raw,
                  &DataSize,sizeof(RAWINPUTHEADER));

  if(raw.header.dwType == RIM_TYPEMOUSE) {
    if(raw.data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) {
      int vk2=key_convert_win32(VK_LBUTTON);

      if(inputs[vk2]!=INPUT_DOWN) {
        inputs[vk2]=INPUT_PRESS;
      }
    }

    if(raw.data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP) {
      int vk2=key_convert_win32(VK_LBUTTON);

      if(inputs[vk2]!=INPUT_UP) {
        inputs[vk2]=INPUT_RELEASE;
      }
    }

    if(raw.data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) {
      int vk2=key_convert_win32(VK_RBUTTON);

      if(inputs[vk2]!=INPUT_DOWN) {
        inputs[vk2]=INPUT_PRESS;
      }
    }

    if(raw.data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP) {
      int vk2=key_convert_win32(VK_RBUTTON);

      if(inputs[vk2]!=INPUT_UP) {
        inputs[vk2]=INPUT_RELEASE;
      }
    }

    if(raw.data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN) {
      int vk2=key_convert_win32(VK_MBUTTON);

      if(inputs[vk2]!=INPUT_DOWN) {
        inputs[vk2]=INPUT_PRESS;
      }
    }

    if(raw.data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP) {
      int vk2=key_convert_win32(VK_MBUTTON);

      if(inputs[vk2]!=INPUT_UP) {
        inputs[vk2]=INPUT_RELEASE;
      }
    }

    //
    if(raw.data.mouse.usButtonFlags & RI_MOUSE_WHEEL) {
      short z=(short)raw.data.mouse.usButtonData;
      *mz=(int)z;
    }
    if(raw.data.mouse.lLastX) {
      long x=raw.data.mouse.lLastX;
      *mx=(int)x;
    }

    if(raw.data.mouse.lLastY) {
      long y=raw.data.mouse.lLastY;
      *my=(int)y;
    }

  } else if(raw.header.dwType == RIM_TYPEKEYBOARD) {

    USHORT vk=raw.data.keyboard.VKey;
    USHORT up=raw.data.keyboard.Flags & RI_KEY_BREAK;
    USHORT right=raw.data.keyboard.Flags & RI_KEY_E0;

    int vk2=key_convert_win32(vk);

    if(up && inputs[vk2]!=INPUT_UP) {
      inputs[vk2]=INPUT_RELEASE;
    }else if(!up && inputs[vk2]!=INPUT_DOWN) {
      inputs[vk2]=INPUT_PRESS;
    }
  }
}

void lock_cursor_win32(HWND hWnd,bool lock, bool *locked,int cursorX,int cursorY) {
  CURSORINFO cursorInfo;
  cursorInfo.cbSize=sizeof(CURSORINFO);
  GetCursorInfo(&cursorInfo);

  POINT pt;
  pt.x=cursorX;
  pt.y=cursorY;
  ClientToScreen(hWnd,&pt);

  if(lock) {

    if(!(*locked)) {
      RECT rcClip;
      GetClientRect(hWnd, &rcClip);
      POINT rc0,rc1;
      rc0.x=rcClip.left;
      rc0.y=rcClip.top;
      rc1.x=rcClip.right;
      rc1.y=rcClip.bottom;

      ClientToScreen(hWnd,&rc0);
      ClientToScreen(hWnd,&rc1);

      rcClip.left=rc0.x;
      rcClip.top=rc0.y;
      rcClip.right=rc1.x;
      rcClip.bottom=rc1.y;

      ClipCursor(&rcClip);
    }

    *locked=true;

    if(cursorInfo.flags==CURSOR_SHOWING) {
      // pt=cursorInfo.ptScreenPos;
      ShowCursor(FALSE);
    }

    SetCursorPos(pt.x,pt.y);

  } else {
    if(cursorInfo.flags==0) {
      ShowCursor(TRUE);
    }

    if(*locked) {
      ClipCursor(0);
    }

    *locked=false;
  }
}


void cursor_pos_win32(HWND hWnd,int *x,int *y) {
  CURSORINFO cursorInfo;
  cursorInfo.cbSize=sizeof(CURSORINFO);
  GetCursorInfo(&cursorInfo);
  ScreenToClient(hWnd,&cursorInfo.ptScreenPos);

  RECT clientRect;
  GetClientRect(GetActiveWindow(),&clientRect);

  if(cursorInfo.ptScreenPos.x>clientRect.right ||
     cursorInfo.ptScreenPos.y>clientRect.bottom) {
    *x=-1;
    *y=-1;
  } else {
    *x=(int)cursorInfo.ptScreenPos.x;
    *y=(int)cursorInfo.ptScreenPos.y;
  }
}

void printLastError_win32() {
#ifdef UNICODE
  WCHAR fMessage[256];
#else
  char fMessage[256];
#endif

  FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                fMessage,256, NULL);
  fprintf(stderr,"%s\n",fMessage);
}

void destroy_gl_context(HGLRC hglrc) {
  wglMakeCurrent(NULL,NULL);
  wglDeleteContext(hglrc);
}

bool update_messages_win32() {
  MSG msg;
  ZeroMemory(&msg,sizeof(MSG));

  while(PeekMessage(&msg,NULL,0,0,PM_REMOVE) && msg.message!=WM_QUIT) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  if(msg.message==WM_QUIT) {
    return false;
  }

  return true;
}

// LRESULT CALLBACK HookProc(int nCode,WPARAM wParam,LPARAM lParam) {
//   MSG *msg=(MSG*)lParam;

//   if(msg->message==WM_INPUT) {
//     rawinput_msg_win32(msg->lParam,
//                        windowData.inputs,
//                        &windowData.mouseX,
//                        &windowData.mouseY,
//                        &windowData.mouseZ);
//   }

//   return CallNextHookEx(NULL, nCode, wParam, lParam);
// }

// HHOOK h=SetWindowsHookEx(WH_GETMESSAGE,HookProc,GetModuleHandle(0),GetCurrentThreadId());


LRESULT CALLBACK WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
  // WindowHandle handle = (WindowHandle)GetWindowLong(hWnd,GWL_USERDATA);

  if(message==WM_INPUT) {
    rawinput_msg_win32(lParam,
                       windowData.inputs,
                       &windowData.mouseX,
                       &windowData.mouseY,
                       &windowData.mouseZ);
  } else if(message==WM_DESTROY) {
    PostQuitMessage(0);
    return 0;
  }

  return DefWindowProc(hWnd,message,wParam,lParam);
}

void client_size_win32(HWND hWnd,int *w,int *h) {
  RECT clientRect;
  GetClientRect(hWnd,&clientRect);
  *w=(int)clientRect.right;
  *h=(int)clientRect.bottom;
}

bool register_class_win32(const char *name,HINSTANCE hInstance,
                          WNDPROC wndproc) {
#ifdef UNICODE
  wchar_t name2[256];
  MultiByteToWideChar(CP_ACP,0,name,-1,name2,256);
#else
    const char *name2=name;
#endif

  WNDCLASSEX wcex;
  ZeroMemory(&wcex,sizeof(WNDCLASSEX));

  wcex.lpszClassName=name2;
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = wndproc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
  // wcex.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(IDI_APPLICATION));
  wcex.hCursor = LoadCursor(NULL,IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_GRAYTEXT);
  wcex.lpszMenuName = NULL;
  //wcex.hIconSm = LoadIcon(wcex.hInstance,MAKEINTRESOURCE(IDI_APPLICATION));

  if(!RegisterClassEx(&wcex)) {
    fprintf(stderr,"window_create: Call to RegisterClassEx failed.\n");
    printLastError_win32();
    return false;
  }

  return true;
}

bool unregister_class_win32(const char *name,HINSTANCE hInstance) {
#ifdef UNICODE
  wchar_t name2[256];
  MultiByteToWideChar(CP_ACP,0,name,-1,name2,256);
#else
  const char *name2=name;
  char fMessage[256];
#endif

  if(TRUE!=UnregisterClass(name2,hInstance)) {
    printLastError_win32();
    return false;
  }

  return true;
}

bool set_gl_pixel_format(HDC hdc) {
  PIXELFORMATDESCRIPTOR pfd;
  ZeroMemory(&pfd,sizeof(PIXELFORMATDESCRIPTOR));

  pfd.nSize = sizeof(pfd);
  pfd.nVersion = 1;
  pfd.dwFlags = PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|
    PFD_GENERIC_ACCELERATED|PFD_DOUBLEBUFFER;
  pfd.iPixelType = PFD_TYPE_RGBA;
  pfd.cColorBits = 24;
  pfd.cRedBits = pfd.cGreenBits = pfd.cBlueBits = 8;
  pfd.cDepthBits = 32;
  pfd.cStencilBits=8;
  //
  int iPixelFormat = ChoosePixelFormat(hdc,&pfd);

  if(iPixelFormat == 0) {
    fprintf(stderr,"window_create: ChoosePixelFormat failed.\n");
    printLastError_win32();
    return false;
  }

  if(SetPixelFormat(hdc,iPixelFormat,&pfd) == FALSE) {
    fprintf(stderr,"window_create: SetPixelFormat failed.\n");
    printLastError_win32();
    return false;
  }

  return true;
}


bool run_gl_swap_interval() {
  PFNWGLSWAPINTERVALEXTPROC wglSwapInterval;
  wglSwapInterval=(PFNWGLSWAPINTERVALEXTPROC)
    wglGetProcAddress("wglSwapIntervalEXT");

  if(!wglSwapInterval) {
    fprintf(stderr,"wglSwapInterval failed to retrieve.\n");
    return false;
  }

  wglSwapInterval(1);
  return true;
}



bool create_window_win32(const char *name,
                         const char *caption,
                         int width,int height,
                         HINSTANCE hInstance,
                         HWND *hWndOut) {
#ifdef UNICODE
  wchar_t name2[256],caption2[256];
  MultiByteToWideChar(CP_ACP,0,name,-1,name2,256);
  MultiByteToWideChar(CP_ACP,0,caption,-1,caption2,256);
#else
  const char *name2=name;
  const char *caption2=caption;
#endif

  RECT wr = {0,0,width,height};
  AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

  HWND hWnd = CreateWindow(name2,caption2,WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT,CW_USEDEFAULT,
                            wr.right-wr.left,
                            wr.bottom-wr.top,
                            NULL,NULL,hInstance,NULL);

  if(!hWnd) {
    fprintf(stderr,"window_create: failed to create window");
    printLastError_win32();
    return false;
  }

  ShowWindow(hWnd,SW_SHOW);

  *hWndOut=hWnd;
  return true;
}

bool gl_context_win32(HDC hdc,
                      PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB,
                      int major,int minor,bool core,
                      HGLRC *hglrc) {
  if(*hglrc) {
    wglMakeCurrent(NULL,NULL);
    wglDeleteContext(*hglrc);
  }

  int attribs[] ={
    WGL_CONTEXT_MAJOR_VERSION_ARB,major,
    WGL_CONTEXT_MINOR_VERSION_ARB,minor,
    WGL_CONTEXT_FLAGS_ARB,0,
    WGL_CONTEXT_PROFILE_MASK_ARB,core?
    WGL_CONTEXT_CORE_PROFILE_BIT_ARB:
    WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
    0};

  *hglrc=wglCreateContextAttribsARB(hdc,0,attribs);

  if(!(*hglrc)) {
    fprintf(stderr,"gl context %i.%i unsupported.\n",major,minor);
    return false;
  }


  wglMakeCurrent(hdc,*hglrc);
  fprintf(stdout,"GL %s\n",glGetString(GL_VERSION));
  // fprintf(stdout,"GLSL %s\n",glGetString(GL_SHADING_LANGUAGE_VERSION));
  return true;
}

bool window_create(const char *caption,int width,int height) {
  HINSTANCE hInstance=GetModuleHandle(0);

  if(!register_class_win32("app",hInstance,WndProc)) {
    return false;
  }

  if(!create_window_win32("app",caption,width,height,
                          hInstance,&windowData.hWnd)) {
    //todo unregister class
    return false;
  }

  if(!rawinput_init_win32(NULL)) {
    //todo unregister class
    //todo destroy window
    return false;
  }


  windowData.hdc=GetDC(windowData.hWnd);

  //
  if(!set_gl_pixel_format(windowData.hdc)) {
    //todo unregister class
    //todo destroy window
    return false;
  }

  //
  HGLRC hglrcTemp=wglCreateContext(windowData.hdc);
  wglMakeCurrent(windowData.hdc,hglrcTemp);

  //
  PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
  wglCreateContextAttribsARB=(PFNWGLCREATECONTEXTATTRIBSARBPROC)
    wglGetProcAddress("wglCreateContextAttribsARB");

  if(!wglCreateContextAttribsARB) {
    fprintf(stderr,"wglCreateContextAttribsARB failed to retrieve.\n");
    //todo destroy temp gl context
    //todo unregister class
    //todo destroy window
    return false;
  }

  //
  wglMakeCurrent(NULL,NULL);
  wglDeleteContext(hglrcTemp);

  //
  windowData.hglrc=0;

  if(!gl_context_win32(windowData.hdc,wglCreateContextAttribsARB,
                       3,3,false,&windowData.hglrc) &&
     !gl_context_win32(windowData.hdc,wglCreateContextAttribsARB,
                       3,2,false,&windowData.hglrc) &&
     !gl_context_win32(windowData.hdc,wglCreateContextAttribsARB,
                       3,1,false,&windowData.hglrc) &&
     !gl_context_win32(windowData.hdc,wglCreateContextAttribsARB,
                       3,0,false,&windowData.hglrc) &&
     !gl_context_win32(windowData.hdc,wglCreateContextAttribsARB,
                       2,1,false,&windowData.hglrc) &&
     !gl_context_win32(windowData.hdc,wglCreateContextAttribsARB,
                       2,0,false,&windowData.hglrc)) {
    //todo unregister class
    //todo destroy window
    return false;
  }

  //
  run_gl_swap_interval();

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


  return true;
}

void window_destroy() {
  destroy_gl_context(windowData.hglrc);
  HINSTANCE hInstance=GetModuleHandle(0);
  DestroyWindow(windowData.hWnd);
  unregister_class_win32("app",hInstance);
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
    if(!update_messages_win32()) {
      return false;
    }

    //
    windowData.iconic=IsIconic(windowData.hWnd)==TRUE;

    //
    if(!windowData.iconic) {
      break;
    }

    windowData.restored=true;
    Sleep(1);
  }

  //
  focused=GetActiveWindow()==windowData.hWnd && GetForegroundWindow()==windowData.hWnd;

  //
  int clientWidth,clientHeight;
  client_size_win32(windowData.hWnd,&clientWidth,&clientHeight);
  windowData.sized=windowData.clientWidth!=clientWidth || windowData.clientHeight!=clientHeight;
  windowData.clientWidth=clientWidth;
  windowData.clientHeight=clientHeight;

  //
  cursor_pos_win32(windowData.hWnd,&windowData.cursorX,&windowData.cursorY);

  //
  if(!windowData.cursorLocked) {
    windowData.lockedCursorX=windowData.cursorX;
    windowData.lockedCursorY=windowData.cursorY;
  }

  //
  lock_cursor_win32(windowData.hWnd,
                    windowData.lockCursor && focused,
                    &windowData.cursorLocked,
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

  //
  windowData.lockCursor=false;

  //
  return true;
}

void window_swap_buffers() {
  SwapBuffers(windowData.hdc);
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

#ifdef USE_CANVAS
struct PixelCanvas {
  int width,height;
  HDC hdc;
  BITMAPINFO bitmapInfo;
  HDC bitmapDc;
  HBITMAP bitmapDib;
  unsigned char *bitmapData;
};
#endif

#ifdef USE_CANVAS
  struct PixelCanvas canvas;
#endif

extern void window_paint(const unsigned char *surfaceData,const char *text);

#ifdef USE_CANVAS
bool pixel_canvas_create_win32(HDC hdc,int width,int height,
                               struct PixelCanvas *canvas) {


  canvas->width=width;
  canvas->height=height;

  //
  canvas->bitmapDc=CreateCompatibleDC(hdc);

  //
  canvas->bitmapInfo.bmiHeader.biSize=sizeof(BITMAPINFOHEADER);
  canvas->bitmapInfo.bmiHeader.biWidth=width;
  canvas->bitmapInfo.bmiHeader.biHeight=-height;
  canvas->bitmapInfo.bmiHeader.biPlanes=1;
  canvas->bitmapInfo.bmiHeader.biBitCount=32;
  canvas->bitmapInfo.bmiHeader.biCompression=BI_RGB;
  canvas->bitmapInfo.bmiHeader.biSizeImage=0;

  canvas->bitmapInfo.bmiHeader.biXPelsPerMeter=0;
  canvas->bitmapInfo.bmiHeader.biYPelsPerMeter=0;

  canvas->bitmapInfo.bmiHeader.biClrUsed=0;//
  canvas->bitmapInfo.bmiHeader.biClrImportant=0;

  canvas->bitmapDib=CreateDIBSection(canvas->bitmapDc,
                                     &canvas->bitmapInfo,DIB_RGB_COLORS,
                                     (VOID**)&canvas->bitmapData,
                                     0,0);
  SelectObject(canvas->bitmapDc, canvas->bitmapDib);

  SetTextColor(canvas->bitmapDc,RGB(255,166,111));
  SetBkMode(canvas->bitmapDc,TRANSPARENT);

  return true;
}

void pixel_canvas_paint_win32(HDC hdc,const unsigned char *pixels,const char *text,
                              struct PixelCanvas *canvas) {


  memcpy(canvas->bitmapData,pixels,canvas->width*canvas->height*4);
  RECT clientRect;
  clientRect.left=0;
  clientRect.top=0;
  clientRect.right=canvas->width;
  clientRect.bottom=canvas->height;

#ifdef UNICODE
  wchar_t text2[1024];
  MultiByteToWideChar(CP_ACP,0,text,-1,text2,1024);
#else
  const char *text2=text;
#endif
  DrawText(canvas->bitmapDc,text2,-1,&clientRect,DT_TOP);

  GdiFlush();
  BitBlt(hdc, 0, 0, canvas->width, canvas->height, canvas->bitmapDc, 0, 0, SRCCOPY);

  // StretchDIBits(data->hdc,
  // 0, 0, data->surfaceWidth, data->surfaceHeight,
  // 0, 0, data->surfaceWidth, data->surfaceHeight,
  // data->bitmapData, &data->bitmapInfo,
  // DIB_RGB_COLORS, SRCCOPY);
}

void window_canvas_paint(const unsigned char *pixels,const char *text) {
  struct WindowData *data=window_data();

  pixel_canvas_paint_win32(data->hdc,
                           pixels, text,
                           &data->canvas);
}
#endif

#ifdef USE_CANVAS
  if(!pixel_canvas_create_win32(data->hdc,width,height,&data->canvas)) {
    return false;
  }
#endif

*/
