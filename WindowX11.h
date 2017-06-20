#ifndef WINDOWX11_H
#define WINDOWX11_H

#define GLX_CONTEXT_MAJOR_VERSION_ARB       0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB       0x2092

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XInput2.h>

//#include <time.h>
#include <sched.h>

#include <GL/gl.h>
#include <GL/glx.h>
#include "Window.h"

typedef GLXContext(*glXCreateContextAttribsARBProc)(Display*,GLXFBConfig,GLXContext,Bool,const int*);
typedef void(*PFNGLXSWAPINTERVALEXTPROC) (Display* dpy,GLXDrawable drawable,int interval);
typedef int(*PFNGLXSWAPINTERVALMESAPROC) (unsigned int interval);
typedef int(*PFNGLXSWAPINTERVALSGIPROC) (int interval);

struct WindowData {
  bool created;
  bool justCreated;

  GLXContext ctx;
  Display *display;
  Window win;
  Colormap cmap;
  GLXFBConfig bestFbc;
  bool ctxErrorOccurred;

  glXCreateContextAttribsARBProc glXCreateContextAttribsARB;
  PFNGLXSWAPINTERVALMESAPROC glXSwapIntervalMESA;
  PFNGLXSWAPINTERVALEXTPROC glXSwapIntervalEXT;
  PFNGLXSWAPINTERVALSGIPROC glXSwapIntervalSGI;

  int clientWidth,clientHeight;
  int cursorX,cursorY;
  int mouseX,mouseY,mouseZ;
  bool iconified,focused,sized;
  bool lockCursor,cursorLocked;
  int inputs[256];
  char inputText[32];

  Cursor blankCursor;
  int xiopcode;
};

#ifdef __cplusplus
extern "C" {
#endif

  int window_ctx_error_handler(Display *dpy,XErrorEvent *ev);
  struct WindowData *window_data();
  int window_key_convert(int vk);
  bool isExtensionSupported(const char *extList,const char *extension);

#ifdef __cplusplus
}
#endif

#endif
