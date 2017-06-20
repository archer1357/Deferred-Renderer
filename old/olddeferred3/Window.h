

#ifndef WINDOW_HPP
#define WINDOW_HPP

#include <string>
#include <iostream>

#ifdef WIN32
#include <windows.h>

#define WGL_CONTEXT_DEBUG_BIT_ARB 0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x0002
#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB 0x2093
#define WGL_CONTEXT_FLAGS_ARB 0x2094

typedef HGLRC(WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC)
(HDC hDC,HGLRC hShareContext,const int* attribList);
typedef BOOL(WINAPI * PFNWGLSWAPINTERVALEXTPROC) (int interval);
#endif


#ifdef _MSC_VER
#pragma comment(lib, "Gdi32.lib")
#pragma comment(lib, "opengl32.lib")
#endif

#include <map>
#include <set>

#define VK_0	0x30
#define VK_1	0x31
#define VK_2	0x32
#define VK_3	0x33
#define VK_4	0x34
#define VK_5	0x35
#define VK_6	0x36
#define VK_7	0x37
#define VK_8	0x38
#define VK_9	0x39
///////
#define VK_A	0x041
#define VK_B	0x042
#define VK_C	0x043
#define VK_D	0x044
#define VK_E	0x045
#define VK_F	0x046
#define VK_G	0x047
#define VK_H	0x048
#define VK_I	0x049
#define VK_J	0x04A
#define VK_K	0x04B
#define VK_L	0x04C
#define VK_M	0x04D
#define VK_N	0x04E
#define VK_O	0x04F
#define VK_P	0x050
#define VK_Q	0x051
#define VK_R    0x052
#define VK_S	0x053
#define VK_T	0x054
#define VK_U	0x055
#define VK_V	0x056
#define VK_W	0x057
#define VK_X	0x058
#define VK_Y	0x059
#define VK_Z	0x05A
#ifdef LINUX
#define VK_LBUTTON        0x01
#define VK_RBUTTON        0x02
#define VK_CANCEL         0x03
#define VK_MBUTTON        0x04 
#define VK_SHIFT          0x10
#define VK_CONTROL        0x11
#define VK_ESCAPE         0x1B
#define VK_SPACE          0x20
#define VK_END            0x23
#define VK_HOME           0x24
#define VK_LEFT           0x25
#define VK_UP             0x26
#define VK_RIGHT          0x27
#define VK_DOWN           0x28
#define VK_EXECUTE        0x2B
#define VK_INSERT         0x2D
#define VK_DELETE         0x2E

#define VK_NUMPAD0        0x60
#define VK_NUMPAD1        0x61
#define VK_NUMPAD2        0x62
#define VK_NUMPAD3        0x63
#define VK_NUMPAD4        0x64
#define VK_NUMPAD5        0x65
#define VK_NUMPAD6        0x66
#define VK_NUMPAD7        0x67
#define VK_NUMPAD8        0x68
#define VK_NUMPAD9        0x69
#define VK_MULTIPLY       0x6A
#define VK_ADD            0x6B
#define VK_SEPARATOR      0x6C
#define VK_SUBTRACT       0x6D
#define VK_DECIMAL        0x6E
#define VK_DIVIDE         0x6F
#define VK_F1             0x70
#define VK_F2             0x71
#define VK_F3             0x72
#define VK_F4             0x73
#define VK_F5             0x74
#define VK_F6             0x75
#define VK_F7             0x76
#define VK_F8             0x77
#define VK_F9             0x78
#define VK_F10            0x79
#define VK_F11            0x7A
#define VK_F12            0x7B
#define VK_F13            0x7C
#define VK_F14            0x7D
#define VK_F15            0x7E
#define VK_F16            0x7F
#define VK_F17            0x80
#define VK_F18            0x81
#define VK_F19            0x82
#define VK_F20            0x83
#define VK_F21            0x84
#define VK_F22            0x85
#define VK_F23            0x86
#define VK_F24            0x87
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>
#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092
typedef GLXContext(*glXCreateContextAttribsARBProc)(Display*,GLXFBConfig,GLXContext,Bool,const int*);

typedef void(*PFNGLXSWAPINTERVALEXTPROC) (Display* dpy,GLXDrawable drawable,int interval);

typedef int(*PFNGLXSWAPINTERVALMESAPROC) (unsigned int interval);

typedef int(*PFNGLXSWAPINTERVALSGIPROC) (int interval);

#endif
class MyWindow {
public:
  //enum InputCode {
  //  inputUndefined,mouseLeft,mouseRight,keyA,keyD,keyS,keyW
  //};
  enum InputAction {inputUp=0,inputPress=1,inputDown=2};
  typedef void(*MouseMoveCallback)(int x,int y);
  typedef void(*InputCallback)(int code,int action);
private:
  bool initError;
  bool justCreated;
  MouseMoveCallback mouseMoveCallback;
  InputCallback inputCallback;
  bool lockCursor;
  std::map<int,int> inputCodeMap;
  std::set<int> inputCodeDown;
#ifdef WIN32
  HWND hWnd;
  MSG msg;
  HDC hdc;
  HGLRC hglrc;
#endif
#ifdef LINUX
  GLXContext ctx;
  Display *display;
  Window win;
  Colormap cmap;
  XEvent newEvent;
  XWindowAttributes winData;
#endif
  int clientWidth,clientHeight;
  bool iconified,focused,sized,foreground;
public:
  MyWindow(const std::string &title,int width,int height);
  ~MyWindow();
  bool run();
  void swapBuffers();
  void makeContext();
  int getClientWidth();
  int getClientHeight();
  bool isIconified();
  bool isFocused();
  bool isSized();
  bool isInitError();
  void setMouseMoveCallback(MouseMoveCallback mouseMoveCallback);
  void setInputCallback(InputCallback inputCallback);
  void setLockCursor(bool lockCursor);
private:
#ifdef WIN32
  static LRESULT CALLBACK WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
#endif
};

#endif
