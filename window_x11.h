#ifndef WINDOW_X11_H
#define WINDOW_X11_H

//todo:
//* fix mouse wheel

#define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092

//
#define INPUTS_SIZE 256

//
#define INPUT_UP 0
#define INPUT_RELEASE 1
#define INPUT_DOWN 2
#define INPUT_PRESS 3

//
#include "window.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <GL/glx.h>
#include <GL/gl.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XInput2.h>
#include <unistd.h>
#include <sched.h>
#include <limits.h>

//
typedef GLXContext(*glXCreateContextAttribsARBProc)(Display*,GLXFBConfig,GLXContext,Bool,const int*);
typedef void(*PFNGLXSWAPINTERVALEXTPROC) (Display* dpy,GLXDrawable drawable,int interval);
typedef int(*PFNGLXSWAPINTERVALMESAPROC) (unsigned int interval);
typedef int(*PFNGLXSWAPINTERVALSGIPROC) (int interval);

//
struct WindowData {
  Display *display;
  Window win;
  Colormap cmap;
  Atom wmDelete;

  GLXContext ctx;
  GLXFBConfig bestFbc;
  bool ctxErrorOccurred;

  Cursor blankCursor;
  int xiopcode;

  int clientWidth,clientHeight;
  bool iconic,restored;
  bool sized;

  bool lockCursor;
  bool cursorLocked;
  int lockedCursorX,lockedCursorY;

  int cursorX,cursorY;
  int mouseX,mouseY,mouseZ;
  int inputs[INPUTS_SIZE];

} windowData;

#endif
