#ifndef WINDOWWIN32_H
#define WINDOWWIN32_H

#ifdef _MSC_VER
#pragma comment(lib, "Gdi32.lib")
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Opengl32.lib")
#endif

#define HID_USAGE_PAGE_GENERIC ((USHORT) 0x01)
#define HID_USAGE_GENERIC_MOUSE ((USHORT) 0x02)
#define HID_USAGE_GENERIC_KEYBOARD ((USHORT) 0x06)

//context flags
#define WGL_CONTEXT_DEBUG_BIT_ARB 0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x0002

//profile mask
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#define WGL_CONTEXT_ES2_PROFILE_BIT_EXT 0x00000004

#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB 0x2093
#define WGL_CONTEXT_FLAGS_ARB 0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126

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
#define VK_R	0x052
#define VK_S	0x053
#define VK_T	0x054
#define VK_U	0x055
#define VK_V	0x056
#define VK_W	0x057
#define VK_X	0x058
#define VK_Y	0x059
#define VK_Z	0x05A

#define WIN32_LEAN_AND_MEAN
#define WINVER 0x0501
#define WIN32_WINNT 0x0501

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Window.h"


typedef HGLRC(WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC)
  (HDC hDC,HGLRC hShareContext,const int* attribList);
typedef BOOL(WINAPI * PFNWGLSWAPINTERVALEXTPROC) (int interval);

struct WindowData {
  bool created;
  bool justCreated;

  LPCTSTR className;
  HWND hWnd;
  HDC hdc;
  HGLRC hglrc;

  PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
  PFNWGLSWAPINTERVALEXTPROC wglSwapInterval;

  int clientWidth,clientHeight;
  int cursorX,cursorY;
  int mouseX,mouseY,mouseZ;
  bool iconified,focused,sized;
  bool lockCursor;
  // bool grabCursor;
  int inputs[256];	
  char inputText[32];
};

#ifdef __cplusplus
extern "C" {

#endif
  struct WindowData *window_data();
  int window_key_convert(int vk);
  LRESULT CALLBACK WndProc(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);

#ifdef __cplusplus
}
#endif

#endif
