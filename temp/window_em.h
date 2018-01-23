#ifndef WINDOW_EM_H
#define WINDOW_EM_H


//todo:
//* fix lock cursor
//* fix release keys/mouse on lost focus

//
#define INPUTS_SIZE 256

//
#define INPUT_UP 0
#define INPUT_RELEASE 1
#define INPUT_DOWN 2
#define INPUT_PRESS 3

//
#include "window.h"
#include <emscripten.h>
#include <html5.h>
#include <stdlib.h>
#include <string.h>


//
struct WindowData {
  EMSCRIPTEN_WEBGL_CONTEXT_HANDLE context;

  int clientWidth,clientHeight;
  bool restored,restoredOut;
  bool sized;
  bool focused;

  bool lockCursor;
  bool cursorLocked;
  int lockedCursorX,lockedCursorY;

  int cursorX,cursorY;
  int mouseX,mouseY,mouseZ;
  int inputs[INPUTS_SIZE];
  int inputsOut[INPUTS_SIZE];

  long emMovementX,emMovementY;
  bool emMouseButtonDowns[3],emMouseButtonUps[3];
  bool emKeyUps[256],emKeyDowns[256];
  double emWheelDeltaY;
} windowData;


#endif
