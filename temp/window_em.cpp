#ifdef __EMSCRIPTEN__

#include "window_em.h"


int key_convert_em(int vk) {
  static bool init=false;
  static int keyMap[256];

  if(!init) {
    init=true;
    memset(keyMap,KEY_UKNOWN, sizeof(keyMap));

    keyMap[32]=KEY_SPACE;
    keyMap[222]=KEY_APOSTROPHE;
    keyMap[188]=KEY_COMMA;
    keyMap[189]=KEY_MINUS;
    keyMap[190]=KEY_PERIOD;
    keyMap[191]=KEY_SLASH;

    keyMap[48]=KEY_0;
    keyMap[49]=KEY_1;
    keyMap[50]=KEY_2;
    keyMap[51]=KEY_3;
    keyMap[52]=KEY_4;
    keyMap[53]=KEY_5;
    keyMap[54]=KEY_6;
    keyMap[55]=KEY_7;
    keyMap[56]=KEY_8;
    keyMap[57]=KEY_9;

    keyMap[186]=KEY_SEMICOLON;
    keyMap[187]=KEY_EQUAL;

    keyMap[65]=KEY_A;
    keyMap[66]=KEY_B;
    keyMap[67]=KEY_C;
    keyMap[68]=KEY_D;
    keyMap[69]=KEY_E;
    keyMap[70]=KEY_F;
    keyMap[71]=KEY_G;
    keyMap[72]=KEY_H;
    keyMap[73]=KEY_I;
    keyMap[74]=KEY_J;
    keyMap[75]=KEY_K;
    keyMap[76]=KEY_L;
    keyMap[77]=KEY_M;
    keyMap[78]=KEY_N;
    keyMap[79]=KEY_O;
    keyMap[80]=KEY_P;
    keyMap[81]=KEY_Q;
    keyMap[82]=KEY_R;
    keyMap[83]=KEY_S;
    keyMap[84]=KEY_T;
    keyMap[85]=KEY_U;
    keyMap[86]=KEY_V;
    keyMap[87]=KEY_W;
    keyMap[88]=KEY_X;
    keyMap[89]=KEY_Y;
    keyMap[90]=KEY_Z;

    keyMap[219]=KEY_LEFT_BRACKET;
    keyMap[220]=KEY_BACKSLASH;
    keyMap[221]=KEY_RIGHT_BRACKET;
    keyMap[192]=KEY_GRAVE_ACCENT;

    keyMap[27]=KEY_ESCAPE;
    keyMap[13]=KEY_RETURN;
    keyMap[192]=KEY_TAB;
    keyMap[8]=KEY_BACKSPACE;
    keyMap[45]=KEY_INSERT;
    keyMap[46]=KEY_DELETE;
    keyMap[39]=KEY_RIGHT;
    keyMap[37]=KEY_LEFT;
    keyMap[40]=KEY_DOWN;
    keyMap[38]=KEY_UP;
    keyMap[33]=KEY_PAGE_UP;
    keyMap[34]=KEY_PAGE_DOWN;
    keyMap[36]=KEY_HOME;
    keyMap[35]=KEY_END;
    keyMap[20]=KEY_CAPS_LOCK;
    keyMap[145]=KEY_SCROLL_LOCK;
    keyMap[144]=KEY_NUM_LOCK;
    keyMap[42]=KEY_PRINT_SCREEN;
    keyMap[19]=KEY_PAUSE;

    keyMap[112]=KEY_F1;
    keyMap[113]=KEY_F2;
    keyMap[114]=KEY_F3;
    keyMap[115]=KEY_F4;
    keyMap[116]=KEY_F5;
    keyMap[117]=KEY_F6;
    keyMap[118]=KEY_F7;
    keyMap[119]=KEY_F8;
    keyMap[120]=KEY_F9;
    keyMap[121]=KEY_F10;
    keyMap[122]=KEY_F11;
    keyMap[123]=KEY_F12;

    keyMap[96]=KEY_KP_0;
    keyMap[97]=KEY_KP_1;
    keyMap[98]=KEY_KP_2;
    keyMap[99]=KEY_KP_3;
    keyMap[100]=KEY_KP_4;
    keyMap[101]=KEY_KP_5;
    keyMap[102]=KEY_KP_6;
    keyMap[103]=KEY_KP_7;
    keyMap[104]=KEY_KP_8;
    keyMap[105]=KEY_KP_9;

    keyMap[110]=KEY_KP_DECIMAL;
    keyMap[111]=KEY_KP_DIVIDE;
    keyMap[106]=KEY_KP_MULTIPLY;
    keyMap[109]=KEY_KP_SUBTRACT;
    keyMap[107]=KEY_KP_ADD;

    keyMap[16]=KEY_SHIFT;
    keyMap[17]=KEY_CONTROL;
    keyMap[18]=KEY_ALT;
    keyMap[91]=KEY_WIN;
    keyMap[93]=KEY_APPS;
  }

  return (vk<0||vk>=256)?KEY_UKNOWN:keyMap[vk];
}


EM_BOOL em_focus_callback(int eventType, const EmscriptenFocusEvent *keyEvent, void *userData) {
  // if(eventType == EMSCRIPTEN_EVENT_FOCUS) {
  //   windowData.focused=true;
  //   printf("FOCUS\n");
  // } else if(eventType == EMSCRIPTEN_EVENT_FOCUSIN) {
  //   printf("FOCUSin\n");
  //   windowData.focused=true;
  // } else if(eventType == EMSCRIPTEN_EVENT_FOCUSOUT) {
  //   printf("FOCUSout\n");
  //   windowData.focused=false;
  // }
  //printf("foc %i\n",eventType);
  // windowData.focused=false;
  return 0;
}

EM_BOOL em_visibilitychange_callback(int eventType,
                                     const EmscriptenVisibilityChangeEvent *visibilityStatus,
                                     void *userData) {

  // if(eventType==EMSCRIPTEN_VISIBILITY_VISIBLE) {

  //   windowData.iconified=false;
  // } else {
  //   printf("notvis\n");
  //   windowData.iconified=true;
  //   // windowData.focused=false;
  // }
  //printf("vis %i\n",eventType);
  windowData.restored=true;
  return 0;
}

EM_BOOL em_ui_callback(int eventType, const EmscriptenUiEvent *e,
                       void *userData) {

  // if(eventType==EMSCRIPTEN_EVENT_RESIZE) {
  //   //data->clientWidth=e->documentBodyClientWidth;
  //   //data->clientHeight=e->documentBodyClientHeight;

  //   //sized=true;
  //   //printf("%d %d\n",data->clientWidth,data->clientHeight);
  // }

  return 0;
}


EM_BOOL em_mouse_callback(int eventType,
                          const EmscriptenMouseEvent *mouseEvent,
                          void *userData) {


  if(eventType==EMSCRIPTEN_EVENT_MOUSEDOWN) {
    windowData.emMouseButtonDowns[mouseEvent->button]=true;
  } else if(eventType==EMSCRIPTEN_EVENT_MOUSEUP) {
    windowData.emMouseButtonUps[mouseEvent->button]=true;
  } else if(eventType==EMSCRIPTEN_EVENT_MOUSEMOVE) {
    windowData.cursorX=mouseEvent->canvasX;
    windowData.cursorY=mouseEvent->canvasY;
    windowData.emMovementX=mouseEvent->movementX;
    windowData.emMovementY=mouseEvent->movementY;
  } else if(eventType==EMSCRIPTEN_EVENT_MOUSEENTER) {
    //windowData.focus=true;
    //printf("enter\n");
  } else if(eventType==EMSCRIPTEN_EVENT_MOUSELEAVE) {
    //windowData.focus=false;
    //printf("leave\n");
  }

  return 0;
}

EM_BOOL em_pointerlockchange_callback(int eventType,
                                      const EmscriptenPointerlockChangeEvent *keyEvent,
                                      void *userData) {

  if(!keyEvent->isActive) {
    windowData.lockCursor=false;
    windowData.cursorLocked=false;
    //printf("unlocked3\n");
  }

  return 0;
}

EM_BOOL em_wheel_callback(int eventType,
                          const EmscriptenWheelEvent *keyEvent,
                          void *userData) {

  if(eventType==EMSCRIPTEN_EVENT_WHEEL) {
    windowData.emWheelDeltaY=keyEvent->deltaY;
  }

  return 0;
}

EM_BOOL em_key_callback(int eventType,
                             const EmscriptenKeyboardEvent *keyEvent,
                             void *userData) {

  int k=key_convert_em(keyEvent->keyCode);

  if(eventType==EMSCRIPTEN_EVENT_KEYDOWN) {
    windowData.emKeyDowns[k]=true;
    // printf("key '%s' '%s' '%lu'\n",keyEvent->code,keyEvent->key,keyEvent->keyCode);
  } else if(eventType==EMSCRIPTEN_EVENT_KEYUP) {
    windowData.emKeyUps[k]=true;
  }

  return 0;
}

void lock_cursor_em(bool lock,bool *locked) {
  if(lock && !(*locked)) {
    emscripten_request_pointerlock("#canvas",1);
    *locked=true;
  } else if(!lock && (*locked)) {
    emscripten_exit_pointerlock();
    *locked=false;
  }
}

void update_mouse_inputs_em(const bool *emMouseButtonDowns,const bool *emMouseButtonUps,int *inputs) {
  const int buttonInds[3]={MOUSE_LEFT,MOUSE_MIDDLE,MOUSE_RIGHT};
  int i;

  for(i=0;i<3;i++) {
    if(emMouseButtonDowns[i] && inputs[buttonInds[i]]!=INPUT_DOWN) {
      inputs[buttonInds[i]]=INPUT_PRESS;
    } else if(emMouseButtonUps[i] && inputs[buttonInds[i]]!=INPUT_UP) {
      inputs[buttonInds[i]]=INPUT_RELEASE;
    }
  }
}

void update_key_inputs_em(const bool *emKeyDowns,const bool *emKeyUps,int *inputs) {
  int i;

  for(i=0;i<INPUTS_SIZE;i++) {
    if(emKeyDowns[i] && inputs[i]!=INPUT_DOWN) {
      inputs[i]=INPUT_PRESS;
    }

    if(emKeyUps[i] && inputs[i]!=INPUT_UP) {
      inputs[i]=INPUT_RELEASE;
    }
  }
}

bool init_glcontext_em(EMSCRIPTEN_WEBGL_CONTEXT_HANDLE *context) {

  EmscriptenWebGLContextAttributes attribs;

  attribs.alpha=true;
  attribs.depth=true;
  attribs.stencil=true;
  attribs.antialias=true;
  attribs.premultipliedAlpha=true;
  attribs.preserveDrawingBuffer=false;
  attribs.preferLowPowerToHighPerformance=false;
  attribs.failIfMajorPerformanceCaveat=false;
  attribs.majorVersion=1;
  attribs.minorVersion=0;
  attribs.enableExtensionsByDefault=false;

  *context=emscripten_webgl_create_context(0,&attribs);

  if((*context) <=0) {
    printf("window_create: context creation error.\n");
    return false;
  }

  //
  emscripten_webgl_make_context_current(*context);

  //
#ifndef GL1
  if(!emscripten_webgl_enable_extension(*context,"OES_vertex_array_object")) {
    printf("window_create: could not enable vao ext.\n");
    return false;
  }
#endif
  return true;
}

bool window_create(const char *caption,int width,int height) {
  //
  emscripten_set_focus_callback("#canvas",0,1,em_focus_callback);
  emscripten_set_focusin_callback("#canvas",0,1,em_focus_callback);
  emscripten_set_focusout_callback("#canvas",0,1,em_focus_callback);
  emscripten_set_visibilitychange_callback(0,1,em_visibilitychange_callback);
  emscripten_set_resize_callback(0, 0, 1, em_ui_callback);

  emscripten_set_click_callback(0,0,1, em_mouse_callback);
  emscripten_set_mousedown_callback(0,0,1, em_mouse_callback);
  emscripten_set_mouseup_callback(0,0,1, em_mouse_callback);
  emscripten_set_mousemove_callback(0,0,1, em_mouse_callback);
  emscripten_set_mouseenter_callback(0,0,1, em_mouse_callback);
  emscripten_set_mouseleave_callback(0,0,1, em_mouse_callback);
  emscripten_set_pointerlockchange_callback(0,0,1, em_pointerlockchange_callback);
  emscripten_set_wheel_callback(0,0,1, em_wheel_callback);
  emscripten_set_keydown_callback(0,0,1, em_key_callback);
  emscripten_set_keyup_callback(0,0,1, em_key_callback);

  //
  emscripten_set_canvas_size(width, height);

  //
  if(!init_glcontext_em(&windowData.context)) {
    return false;
  }

  //
  windowData.emMovementX=0;
  windowData.emMovementY=0;
  windowData.emWheelDeltaY=0.0;

  memset(&windowData.emMouseButtonDowns,0,sizeof(windowData.emMouseButtonDowns));
  memset(&windowData.emMouseButtonUps,0,sizeof(windowData.emMouseButtonUps));
  memset(&windowData.emKeyDowns,0,sizeof(windowData.emKeyDowns));
  memset(&windowData.emKeyUps,0,sizeof(windowData.emKeyUps));


  //
  int i;

  //
  windowData.focused=true;

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

  //
  return true;
}

void window_destroy() {
  //unnecessary
}

bool window_update() {
  //
  bool focused=false;

  windowData.mouseX=0;
  windowData.mouseY=0;
  windowData.mouseZ=0;

  //
  int i;

  //
  windowData.mouseX=windowData.emMovementX;
  windowData.mouseY=windowData.emMovementY;
  windowData.mouseZ=-(int)windowData.emWheelDeltaY;

  windowData.emMovementX=0;
  windowData.emMovementY=0;
  windowData.emWheelDeltaY=0.0;

  //
  update_mouse_inputs_em(windowData.emMouseButtonDowns,windowData.emMouseButtonUps,windowData.inputs);
  update_key_inputs_em(windowData.emKeyDowns,windowData.emKeyUps,windowData.inputs);

  //
  memset(&windowData.emMouseButtonDowns,0,sizeof(windowData.emMouseButtonDowns));
  memset(&windowData.emMouseButtonUps,0,sizeof(windowData.emMouseButtonUps));
  memset(&windowData.emKeyDowns,0,sizeof(windowData.emKeyDowns));
  memset(&windowData.emKeyUps,0,sizeof(windowData.emKeyUps));

  //
  memcpy(windowData.inputsOut,windowData.inputs,sizeof(windowData.inputsOut));

  //
  for(i=0;i<INPUTS_SIZE;i++) {
    if(windowData.inputs[i]==INPUT_PRESS) {
      windowData.inputs[i]=INPUT_DOWN;
    } else if(windowData.inputs[i]==INPUT_RELEASE) {
      windowData.inputs[i]=INPUT_UP;
    }
  }

  //
  windowData.restoredOut=windowData.restored;
  windowData.restored=false;

  //
  // while(1) {
  //messages

  //
  // windowData.iconic=IsIconic(windowData.hWnd)==TRUE;

  //
  // if(!windowData.iconic) {
  //   // break;
  // }

  // windowData.restored=true;

  // }

  //
  //todo focused

  //
  int clientWidth,clientHeight;
  //client_size_win32(windowData.hWnd,&clientWidth,&clientHeight);
  double cw,ch;
  emscripten_get_element_css_size(0, &cw,&ch);
  clientWidth=(int)cw;
  clientHeight=(int)ch;
  windowData.sized=windowData.clientWidth!=clientWidth || windowData.clientHeight!=clientHeight;
  windowData.clientWidth=clientWidth;
  windowData.clientHeight=clientHeight;

  //
  //todo cursorpos

  //
  if(!windowData.cursorLocked) {
    windowData.lockedCursorX=windowData.cursorX;
    windowData.lockedCursorY=windowData.cursorY;
  }

  //
  lock_cursor_em(windowData.lockCursor,&windowData.cursorLocked);

  //
  if(/*!focused ||*/ windowData.restoredOut ) {
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

  return true;
}

void window_swap_buffers() {
  //unnecessary
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
  return windowData.restoredOut;
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
  return windowData.inputsOut[k]==INPUT_PRESS;
}

bool window_key_down(int k) {
  return windowData.inputsOut[k]==INPUT_PRESS || windowData.inputsOut[k]==INPUT_DOWN;
}

bool window_key_release(int k) {
  return windowData.inputsOut[k]==INPUT_RELEASE;
}

#endif

/*
--js-library library.js -DUSE_CANVAS

  void window_canvas_paint_init_em(int w,int h);
void window_canvas_paint(const unsigned char *pixels,const char *text);
  #ifdef USE_CANVAS
  window_canvas_paint_init_em(width,height);
  #endif
===========
library.js
===========

mergeInto(
  LibraryManager.library,
  (function() {
    var canvasWidth=0;
    var canvasHeight =0;
    var canvas=null;
    var ctx =null;
    var imageData =null;
    var buf = null;
    var buf8 = null;
    var data= null;
    return {
      window_canvas_paint_init_em:function(w,h) {
	canvas = document.getElementById('canvas');
        canvasWidth  = canvas.width;
        canvasHeight = canvas.height;
        ctx = canvas.getContext('2d');
        imageData = ctx.getImageData(0, 0, canvasWidth, canvasHeight);

        // buf = new ArrayBuffer(imageData.data.length);
        // buf8 = new Uint8ClampedArray(buf);
        // data = new Uint32Array(buf);
        data = imageData.data;

        ctx.font = "16px Arial";
        ctx.fillStyle = 'yellow';
      },

      window_canvas_paint : function(surfaceData,text) {
	for(var i=0;i<canvasWidth*canvasHeight*4;i++) {
	   //buf8[i]=Module.HEAPU8[surfaceData+ i];
	   data[i]=Module.HEAPU8[surfaceData+ i];
	}

	// for(var i=0;i<canvasWidth*canvasHeight;i++) {
	 // data2[i]=Module.HEAPU32[surfaceData+ i*4];
	// }

        //imageData.data.set(buf8);

        ctx.putImageData(imageData, 0, 0);

        var str=Pointer_stringify(text);
        ctx.fillText(str,1,16);

      }
    }
  })()
);
*/
