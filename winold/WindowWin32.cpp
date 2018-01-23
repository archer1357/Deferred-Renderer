
#ifdef _WIN32

#include "WindowWin32.h"

bool window_gl_context(const int *attribs) {
  struct WindowData *data;

  data=window_data();

  if(!data->created) {
    return false;
  }

  //cleanup any previous context
  if(data->hglrc) {
    wglMakeCurrent(NULL,NULL);
    wglDeleteContext(data->hglrc);
    data->hglrc=0;
  }

  //
  data->hglrc=data->wglCreateContextAttribsARB(data->hdc,0,attribs);

  if(!data->hglrc) {
    return false;
  }

  //
  wglMakeCurrent(data->hdc,data->hglrc);
  data->wglSwapInterval(1);
  return true;
}

bool window_create(const char *caption, int width, int height) {
  struct WindowData *data=window_data();
  int iPixelFormat;
  HINSTANCE hInstance;

  //
  if(data->created) {
    fprintf(stderr,"window_create: already created.\n");
    return false;
  }

  hInstance=GetModuleHandle(0);

  //
  WNDCLASSEX wcex;
  ZeroMemory(&wcex,sizeof(WNDCLASSEX));

  wcex.lpszClassName=data->className;
  wcex.cbSize = sizeof(WNDCLASSEX);
  wcex.style = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc = WndProc;
  wcex.cbClsExtra = 0;
  wcex.cbWndExtra = 0;
  wcex.hInstance = hInstance;
 // wcex.hIcon = LoadIcon(hInstance,MAKEINTRESOURCE(IDI_APPLICATION));
  wcex.hCursor = LoadCursor(NULL,IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
  wcex.lpszMenuName = NULL;
  //wcex.hIconSm = LoadIcon(wcex.hInstance,MAKEINTRESOURCE(IDI_APPLICATION));

  if(!RegisterClassEx(&wcex)) {
    fprintf(stderr,"window_create: Call to RegisterClassEx failed.\n");

#ifdef UNICODE
    WCHAR fMessage[256];
#else
    char fMessage[256];
#endif

    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  fMessage, sizeof(fMessage), NULL);
    fprintf(stderr,"%s\n",fMessage);

    return false;
  }

  //
#ifdef UNICODE
  wchar_t caption2[256];
  MultiByteToWideChar(CP_ACP,0,caption,-1,caption2,256);
#else
  const char *caption2=caption;
#endif
  RECT wr = {0,0,width,height};
  AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);

  data->hWnd = CreateWindow(wcex.lpszClassName,caption2,WS_OVERLAPPEDWINDOW,
                            CW_USEDEFAULT,CW_USEDEFAULT,
                            wr.right-wr.left,
                            wr.bottom-wr.top,
                            NULL,NULL,hInstance,NULL);

  if(!data->hWnd) {
    fprintf(stderr,"window_create: failed to create window");

#ifdef UNICODE
    WCHAR fMessage[256];
#else
    char fMessage[256];
#endif

    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  fMessage, sizeof(fMessage), NULL);
    fprintf(stderr,"%s\n",fMessage);

    UnregisterClass(data->className,hInstance);
    data->hWnd=0;
    return false;
  }

  ShowWindow(data->hWnd,SW_SHOW);

  //
  data->hdc=GetDC(data->hWnd);

  //raw input
  RAWINPUTDEVICE Rid[2];
  ZeroMemory(&Rid,sizeof(RAWINPUTDEVICE));

  Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
  Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
  Rid[0].dwFlags = RIDEV_INPUTSINK;
  Rid[0].hwndTarget = data->hWnd;

  Rid[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
  Rid[1].usUsage = HID_USAGE_GENERIC_KEYBOARD;//RIDEV_DEVNOTIFY
  Rid[1].dwFlags = RIDEV_INPUTSINK;
  Rid[1].hwndTarget = data->hWnd;

  RegisterRawInputDevices(Rid,2,sizeof(RAWINPUTDEVICE));

  //
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

  //
  iPixelFormat = ChoosePixelFormat(data->hdc,&pfd);

  if(iPixelFormat == 0) {
    fprintf(stderr,"window_create: ChoosePixelFormat failed.\n");

#ifdef UNICODE
    WCHAR fMessage[256];
#else
    char fMessage[256];
#endif

    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  fMessage, sizeof(fMessage), NULL);
    fprintf(stderr,"%s\n",fMessage);

    //
    DestroyWindow(data->hWnd);
    UnregisterClass(data->className,hInstance);
    data->hWnd=0;
    data->hdc=0;
    return false;
  }

  if(SetPixelFormat(data->hdc,iPixelFormat,&pfd) != TRUE) {
    fprintf(stderr,"window_create: SetPixelFormat failed.\n");

#ifdef UNICODE
    WCHAR fMessage[256];
#else
    char fMessage[256];
#endif

    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  fMessage, sizeof(fMessage), NULL);
    fprintf(stderr,"%s\n",fMessage);

    //
    DestroyWindow(data->hWnd);
    UnregisterClass(data->className,hInstance);
    data->hWnd=0;
    data->hdc=0;
    return false;
  }

  //create temporary context
  data->hglrc=wglCreateContext(data->hdc);

  if(!data->hglrc) {
  }

  wglMakeCurrent(data->hdc,data->hglrc);


  PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
  PFNWGLSWAPINTERVALEXTPROC wglSwapInterval;

  wglCreateContextAttribsARB=(PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress
    ("wglCreateContextAttribsARB");

  wglSwapInterval=(PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");


  if(!wglCreateContextAttribsARB || !wglSwapInterval) {
    fprintf(stderr,"window_create: failed to retrieve some wgl functions.\n");
    wglMakeCurrent(NULL,NULL);
    wglDeleteContext(data->hglrc);
    DestroyWindow(data->hWnd);
    UnregisterClass(data->className,hInstance);
    data->hWnd=0;
    data->hdc=0;
    return false;
  }

  data->wglCreateContextAttribsARB=wglCreateContextAttribsARB;
  data->wglSwapInterval=wglSwapInterval;

  //
  data->created=true;
  data->justCreated=true;

  memset(data->inputs, WINDOW_INPUT_UP, sizeof(data->inputs));

  int attribs[] ={
    WGL_CONTEXT_MAJOR_VERSION_ARB,3,
    WGL_CONTEXT_MINOR_VERSION_ARB,0,
    WGL_CONTEXT_FLAGS_ARB,0,
    // WGL_CONTEXT_PROFILE_MASK_ARB,WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
    // WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
    0};

  if(!window_gl_context(attribs)) {
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

  if(data->hglrc) {
    wglMakeCurrent(NULL,NULL);
    wglDeleteContext(data->hglrc);
    data->hglrc=0;
  }

  HINSTANCE hInstance=GetModuleHandle(0);
  DestroyWindow(data->hWnd);

  if(TRUE!=UnregisterClass(data->className,hInstance)) {
#ifdef UNICODE
    WCHAR fMessage[256];
#else
    char fMessage[256];
#endif
    FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  fMessage, sizeof(fMessage), NULL);
    fprintf(stderr,"window_destroy: %s\n",fMessage);
  }


  data->created=false;
}


bool window_update() {
  struct WindowData *data=window_data();
  int i;
  MSG msg;
  RECT clientRect;
  
  //
  if(!data->created) {
    fprintf(stderr,"window_update: window not created.\n");
    return false;
  }

  data->mouseX=0;
  data->mouseY=0;
  data->mouseZ=0;
  
  memset(data->inputText,0,sizeof(data->inputText));

  if(data->justCreated) {
    data->sized=true;
    data->justCreated=false;
  } else {
    data->sized=false;
  }

 for(i=0;i<256;i++) {
    if(data->inputs[i]==WINDOW_INPUT_PRESS) {
      data->inputs[i]=WINDOW_INPUT_DOWN;
    } else if(data->inputs[i]==WINDOW_INPUT_RELEASE) {
      data->inputs[i]=WINDOW_INPUT_UP;
    }
  }

  //
  ZeroMemory(&msg,sizeof(MSG));

  while(PeekMessage(&msg,NULL,0,0,PM_REMOVE) && msg.message!=WM_QUIT) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  if(msg.message==WM_QUIT) {
    return false;
  }

  //
  bool foreground=GetForegroundWindow()==data->hWnd;
  data->focused=data->focused && foreground;

  //
  CURSORINFO cursorInfo;
  cursorInfo.cbSize=sizeof(CURSORINFO);
  GetCursorInfo(&cursorInfo);
  ScreenToClient(data->hWnd,&cursorInfo.ptScreenPos);

  //
  data->cursorX=(int)cursorInfo.ptScreenPos.x;
  data->cursorY=(int)cursorInfo.ptScreenPos.y;

  //
  if(data->lockCursor && data->focused) {
    RECT rect;
    GetWindowRect(data->hWnd,&rect);
    SetCursorPos((rect.right-rect.left)/2+rect.left,(rect.bottom-rect.top)/2+rect.top);

    if(cursorInfo.flags==CURSOR_SHOWING) {
      ShowCursor(FALSE);
    }
  } else if(cursorInfo.flags==0) {
    ShowCursor(TRUE);
  }

  //
  GetClientRect(data->hWnd,&clientRect);
  data->clientWidth=clientRect.right;
  data->clientHeight=clientRect.bottom;

  //
  if(!data->focused) {
    for(i=0;i<256;i++) {
      data->inputs[i]=WINDOW_INPUT_UP;
    }
  }

  //
  if(data->iconified) {
    Sleep(1); //ms
  }

  return true;
}

void window_swap_buffers() {
  struct WindowData *data=window_data();

  if(!data->created) {
    fprintf(stderr,"window_swap_buffers: window not created.\n");
    return;
  }

  SwapBuffers(data->hdc);
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
  return data->lockCursor;
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

struct WindowData *window_data() {
#ifdef UNICODE
  static LPCTSTR className=L"app";
#else
  static LPCTSTR className="app";
#endif

  static struct WindowData data;
  static bool init=false;

  if(!init) {
    init=true;


    data.className=className;

    // data.hWnd=0;
    // data.hdc=0;
    data.hglrc=0;
    // data.wglCreateContextAttribsARB=NULL;
    // data.wglSwapInterval=NULL;

    data.created=false;
    data.iconified=false;
    data.focused=true;
    data.lockCursor=false;
    memset(data.inputs, WINDOW_INPUT_UP, sizeof(data.inputs));
  }

  return &data;
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam) {
  struct WindowData *data=window_data();

  // WindowHandle handle = (WindowHandle)GetWindowLong(hWnd,GWL_USERDATA);

  if(!data->created) {
    return DefWindowProc(hWnd,message,wParam,lParam);
  } else if(message==WM_INPUT) {
    // UINT dwSize = 40;
    // static BYTE lpb[40];
    // GetRawInputData((HRAWINPUT)lParam,RID_INPUT,lpb,&dwSize,sizeof(RAWINPUTHEADER));
    // RAWINPUT* raw = (RAWINPUT*)lpb;

    RAWINPUT raw;
    UINT DataSize = sizeof(RAWINPUT);
    GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &raw, &DataSize, sizeof(RAWINPUTHEADER));

    if(raw.header.dwType == RIM_TYPEMOUSE) {
      if(raw.data.mouse.usButtonFlags & RI_MOUSE_WHEEL) {
        data->mouseZ=(int)raw.data.mouse.usButtonData;
      } else if(raw.data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) {
        data->inputs[WINDOW_MOUSE_LEFT]=WINDOW_INPUT_PRESS;
      } else if(raw.data.mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP &&
                data->inputs[WINDOW_MOUSE_LEFT]!=WINDOW_INPUT_UP) {
        data->inputs[WINDOW_MOUSE_LEFT]=WINDOW_INPUT_RELEASE;
      } else if(raw.data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) {
        data->inputs[WINDOW_MOUSE_RIGHT]=WINDOW_INPUT_PRESS;
      } else if(raw.data.mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP &&
                data->inputs[WINDOW_MOUSE_RIGHT]!=WINDOW_INPUT_UP) {
        data->inputs[WINDOW_MOUSE_RIGHT]=WINDOW_INPUT_RELEASE;
      } else if(raw.data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN) {
        data->inputs[WINDOW_MOUSE_MIDDLE]=WINDOW_INPUT_PRESS;
      } else if(raw.data.mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP &&
                data->inputs[WINDOW_MOUSE_MIDDLE]!=WINDOW_INPUT_UP) {
        data->inputs[WINDOW_MOUSE_MIDDLE]=WINDOW_INPUT_RELEASE;
      }

      data->mouseX=(int)raw.data.mouse.lLastX;
      data->mouseY=(int)raw.data.mouse.lLastY;
    } else if(raw.header.dwType == RIM_TYPEKEYBOARD) {
      USHORT vk=raw.data.keyboard.VKey;
      USHORT up=raw.data.keyboard.Flags & RI_KEY_BREAK;
      USHORT right=raw.data.keyboard.Flags & RI_KEY_E0;

      if(up && data->inputs[window_key_convert(vk)]!=WINDOW_INPUT_UP) {
        data->inputs[window_key_convert(vk)]=WINDOW_INPUT_RELEASE;
      }else if(!up) {
        data->inputs[window_key_convert(vk)]=WINDOW_INPUT_PRESS;
      }
    }
  } else if(message==WM_DESTROY) {
    PostQuitMessage(0);
    return 0;
  } else if(message==WM_SIZE) {
    //data->clientWidth=(int)LOWORD(lParam);
    //data->clientHeight=(int)HIWORD(lParam);
    data->sized=true;
    data->iconified=(wParam==SIZE_MINIMIZED);
  } else if(message==WM_KILLFOCUS) {
    data->focused=false;
  } else if(message==WM_SETFOCUS) {
    data->focused=true;
  } else {
    return DefWindowProc(hWnd,message,wParam,lParam);
  }

  return 0;
}

int window_key_convert(int vk) {
  static bool init=false;
  static unsigned int keyMap[256];

  if(!init) {
    init=true;
    memset(keyMap,WINDOW_KEY_UKNOWN, sizeof(keyMap));

    keyMap[VK_SPACE]=WINDOW_KEY_SPACE;
    keyMap[VK_OEM_7]=WINDOW_KEY_APOSTROPHE;
    keyMap[VK_OEM_COMMA]=WINDOW_KEY_COMMA;
    keyMap[VK_OEM_COMMA]=WINDOW_KEY_MINUS;
    keyMap[VK_OEM_PERIOD]=WINDOW_KEY_PERIOD;
    keyMap[VK_OEM_2]=WINDOW_KEY_SLASH;

    keyMap[VK_0]=WINDOW_KEY_0;
    keyMap[VK_1]=WINDOW_KEY_1;
    keyMap[VK_2]=WINDOW_KEY_2;
    keyMap[VK_3]=WINDOW_KEY_3;
    keyMap[VK_4]=WINDOW_KEY_4;
    keyMap[VK_5]=WINDOW_KEY_5;
    keyMap[VK_6]=WINDOW_KEY_6;
    keyMap[VK_7]=WINDOW_KEY_7;
    keyMap[VK_8]=WINDOW_KEY_8;
    keyMap[VK_9]=WINDOW_KEY_9;

    keyMap[VK_OEM_1]=WINDOW_KEY_SEMICOLON;
    keyMap[VK_OEM_PLUS]=WINDOW_KEY_EQUAL;

    keyMap[VK_A]=WINDOW_KEY_A;
    keyMap[VK_B]=WINDOW_KEY_B;
    keyMap[VK_C]=WINDOW_KEY_C;
    keyMap[VK_D]=WINDOW_KEY_D;
    keyMap[VK_E]=WINDOW_KEY_E;
    keyMap[VK_F]=WINDOW_KEY_F;
    keyMap[VK_G]=WINDOW_KEY_G;
    keyMap[VK_H]=WINDOW_KEY_H;
    keyMap[VK_I]=WINDOW_KEY_I;
    keyMap[VK_J]=WINDOW_KEY_J;
    keyMap[VK_K]=WINDOW_KEY_K;
    keyMap[VK_L]=WINDOW_KEY_L;
    keyMap[VK_M]=WINDOW_KEY_M;
    keyMap[VK_N]=WINDOW_KEY_N;
    keyMap[VK_O]=WINDOW_KEY_O;
    keyMap[VK_P]=WINDOW_KEY_P;
    keyMap[VK_Q]=WINDOW_KEY_Q;
    keyMap[VK_R]=WINDOW_KEY_R;
    keyMap[VK_S]=WINDOW_KEY_S;
    keyMap[VK_T]=WINDOW_KEY_T;
    keyMap[VK_U]=WINDOW_KEY_U;
    keyMap[VK_V]=WINDOW_KEY_V;
    keyMap[VK_W]=WINDOW_KEY_W;
    keyMap[VK_X]=WINDOW_KEY_X;
    keyMap[VK_Y]=WINDOW_KEY_Y;
    keyMap[VK_Z]=WINDOW_KEY_Z;

    keyMap[VK_OEM_4]=WINDOW_KEY_LEFT_BRACKET;
    keyMap[VK_OEM_5]=WINDOW_KEY_BACKSLASH;
    keyMap[VK_OEM_6]=WINDOW_KEY_RIGHT_BRACKET;
    keyMap[VK_OEM_3]=WINDOW_KEY_GRAVE_ACCENT;

    keyMap[VK_ESCAPE]=WINDOW_KEY_ESCAPE;
    keyMap[VK_RETURN]=WINDOW_KEY_RETURN;
    keyMap[VK_TAB]=WINDOW_KEY_TAB;
    keyMap[VK_BACK]=WINDOW_KEY_BACKSPACE;
    keyMap[VK_INSERT]=WINDOW_KEY_INSERT;
    keyMap[VK_DELETE]=WINDOW_KEY_DELETE;
    keyMap[VK_RIGHT]=WINDOW_KEY_RIGHT;
    keyMap[VK_LEFT]=WINDOW_KEY_LEFT;
    keyMap[VK_DOWN]=WINDOW_KEY_DOWN;
    keyMap[VK_UP]=WINDOW_KEY_UP;
    keyMap[VK_PRIOR]=WINDOW_KEY_PAGE_UP;
    keyMap[VK_NEXT]=WINDOW_KEY_PAGE_DOWN;
    keyMap[VK_HOME]=WINDOW_KEY_HOME;
    keyMap[VK_END]=WINDOW_KEY_END;
    keyMap[VK_CAPITAL]=WINDOW_KEY_CAPS_LOCK;
    keyMap[VK_SCROLL]=WINDOW_KEY_SCROLL_LOCK;
    keyMap[VK_NUMLOCK]=WINDOW_KEY_NUM_LOCK;
    keyMap[VK_SNAPSHOT]=WINDOW_KEY_PRINT_SCREEN;
    keyMap[VK_PAUSE]=WINDOW_KEY_PAUSE;

    keyMap[VK_F1]=WINDOW_KEY_F1;
    keyMap[VK_F2]=WINDOW_KEY_F2;
    keyMap[VK_F3]=WINDOW_KEY_F3;
    keyMap[VK_F4]=WINDOW_KEY_F4;
    keyMap[VK_F5]=WINDOW_KEY_F5;
    keyMap[VK_F6]=WINDOW_KEY_F6;
    keyMap[VK_F7]=WINDOW_KEY_F7;
    keyMap[VK_F8]=WINDOW_KEY_F8;
    keyMap[VK_F9]=WINDOW_KEY_F9;
    keyMap[VK_F10]=WINDOW_KEY_F10;
    keyMap[VK_F11]=WINDOW_KEY_F11;
    keyMap[VK_F12]=WINDOW_KEY_F12;

    keyMap[VK_NUMPAD0]=WINDOW_KEY_KP_0;
    keyMap[VK_NUMPAD1]=WINDOW_KEY_KP_1;
    keyMap[VK_NUMPAD2]=WINDOW_KEY_KP_2;
    keyMap[VK_NUMPAD3]=WINDOW_KEY_KP_3;
    keyMap[VK_NUMPAD4]=WINDOW_KEY_KP_4;
    keyMap[VK_NUMPAD5]=WINDOW_KEY_KP_5;
    keyMap[VK_NUMPAD6]=WINDOW_KEY_KP_6;
    keyMap[VK_NUMPAD7]=WINDOW_KEY_KP_7;
    keyMap[VK_NUMPAD8]=WINDOW_KEY_KP_8;
    keyMap[VK_NUMPAD9]=WINDOW_KEY_KP_9;

    keyMap[VK_DECIMAL]=WINDOW_KEY_KP_DECIMAL;
    keyMap[VK_DIVIDE]=WINDOW_KEY_KP_DIVIDE;
    keyMap[VK_MULTIPLY]=WINDOW_KEY_KP_MULTIPLY;
    keyMap[VK_SUBTRACT]=WINDOW_KEY_KP_SUBTRACT;
    keyMap[VK_ADD]=WINDOW_KEY_KP_ADD;

    keyMap[VK_SHIFT]=WINDOW_KEY_SHIFT;
    keyMap[VK_CONTROL]=WINDOW_KEY_CONTROL;
    keyMap[VK_MENU]=WINDOW_KEY_ALT;
    keyMap[VK_LWIN]=WINDOW_KEY_WIN;
    keyMap[VK_APPS]=WINDOW_KEY_APPS;

    keyMap[VK_LBUTTON]=WINDOW_MOUSE_LEFT;
    keyMap[VK_RBUTTON]=WINDOW_MOUSE_RIGHT;
    keyMap[VK_MBUTTON]=WINDOW_MOUSE_MIDDLE;

  }

  return (vk<0||vk>=256)?WINDOW_KEY_UKNOWN:keyMap[vk];
}

#endif
