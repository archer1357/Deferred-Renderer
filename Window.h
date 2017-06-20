#ifndef WINDOW_H
#define WINDOW_H

#ifndef __cplusplus
#include <stdbool.h>
#endif


#define WINDOW_INPUT_UP 0
#define WINDOW_INPUT_RELEASE 1
#define WINDOW_INPUT_DOWN 2
#define WINDOW_INPUT_PRESS 3

//todo: differentiate between left and right keys

// #define WINDOW_KEY_LEFT_CONTROL 162
// #define WINDOW_KEY_RIGHT_CONTROL 163
// #define WINDOW_KEY_LEFT_ALT 164
// #define WINDOW_KEY_RIGHT_ALT 165
// #define WINDOW_KEY_LEFT_SHIFT 160
// #define WINDOW_KEY_RIGHT_SHIFT 161
// #define WINDOW_KEY_LEFT_WIN 91
// #define WINDOW_KEY_RIGHT_WIN 92
// #define WINDOW_KEY_KP_RETURN

#define WINDOW_KEY_UKNOWN -1

#define WINDOW_KEY_SPACE 32
#define WINDOW_KEY_APOSTROPHE 222
#define WINDOW_KEY_COMMA 188
#define WINDOW_KEY_MINUS 189
#define WINDOW_KEY_PERIOD 190
#define WINDOW_KEY_SLASH 191

#define WINDOW_KEY_0 48
#define WINDOW_KEY_1 49
#define WINDOW_KEY_2 50
#define WINDOW_KEY_3 51
#define WINDOW_KEY_4 52
#define WINDOW_KEY_5 53
#define WINDOW_KEY_6 54
#define WINDOW_KEY_7 55
#define WINDOW_KEY_8 56
#define WINDOW_KEY_9 57

#define WINDOW_KEY_SEMICOLON  186
#define WINDOW_KEY_EQUAL 187

#define WINDOW_KEY_A 65
#define WINDOW_KEY_B 66
#define WINDOW_KEY_C 67
#define WINDOW_KEY_D 68
#define WINDOW_KEY_E 69
#define WINDOW_KEY_F 70
#define WINDOW_KEY_G 71
#define WINDOW_KEY_H 72
#define WINDOW_KEY_I 73
#define WINDOW_KEY_J 74
#define WINDOW_KEY_K 75
#define WINDOW_KEY_L 76
#define WINDOW_KEY_M 77
#define WINDOW_KEY_N 78
#define WINDOW_KEY_O 79
#define WINDOW_KEY_P 80
#define WINDOW_KEY_Q 81
#define WINDOW_KEY_R 82
#define WINDOW_KEY_S 83
#define WINDOW_KEY_T 84
#define WINDOW_KEY_U 85
#define WINDOW_KEY_V 86
#define WINDOW_KEY_W 87
#define WINDOW_KEY_X 88
#define WINDOW_KEY_Y 89
#define WINDOW_KEY_Z 90

#define WINDOW_KEY_LEFT_BRACKET  219
#define WINDOW_KEY_BACKSLASH 220
#define WINDOW_KEY_RIGHT_BRACKET 221
#define WINDOW_KEY_GRAVE_ACCENT  192

#define WINDOW_KEY_ESCAPE 27
#define WINDOW_KEY_RETURN 13
#define WINDOW_KEY_TAB 9
#define WINDOW_KEY_BACKSPACE 8
#define WINDOW_KEY_INSERT 45
#define WINDOW_KEY_DELETE 46
#define WINDOW_KEY_RIGHT 39
#define WINDOW_KEY_LEFT 37
#define WINDOW_KEY_DOWN 40
#define WINDOW_KEY_UP 38
#define WINDOW_KEY_PAGE_UP 33
#define WINDOW_KEY_PAGE_DOWN 34
#define WINDOW_KEY_HOME 36
#define WINDOW_KEY_END 35
#define WINDOW_KEY_CAPS_LOCK 20
#define WINDOW_KEY_SCROLL_LOCK 145
#define WINDOW_KEY_NUM_LOCK 144
#define WINDOW_KEY_PRINT_SCREEN 44
#define WINDOW_KEY_PAUSE 19

#define WINDOW_KEY_F1 112
#define WINDOW_KEY_F2 113
#define WINDOW_KEY_F3 114
#define WINDOW_KEY_F4 115
#define WINDOW_KEY_F5 116
#define WINDOW_KEY_F6 117
#define WINDOW_KEY_F7 118
#define WINDOW_KEY_F8 119
#define WINDOW_KEY_F9 120
#define WINDOW_KEY_F10 121
#define WINDOW_KEY_F11 122
#define WINDOW_KEY_F12 123

#define WINDOW_KEY_KP_0 96
#define WINDOW_KEY_KP_1 97
#define WINDOW_KEY_KP_2 98
#define WINDOW_KEY_KP_3 99
#define WINDOW_KEY_KP_4 100
#define WINDOW_KEY_KP_5 101
#define WINDOW_KEY_KP_6 102
#define WINDOW_KEY_KP_7 103
#define WINDOW_KEY_KP_8 104
#define WINDOW_KEY_KP_9 105

#define WINDOW_KEY_KP_DECIMAL 110
#define WINDOW_KEY_KP_DIVIDE 111
#define WINDOW_KEY_KP_MULTIPLY 106
#define WINDOW_KEY_KP_SUBTRACT 109
#define WINDOW_KEY_KP_ADD 107

#define WINDOW_KEY_SHIFT 16
#define WINDOW_KEY_CONTROL 17
#define WINDOW_KEY_ALT 18
#define WINDOW_KEY_WIN 91
#define WINDOW_KEY_APPS 93

#define WINDOW_MOUSE_LEFT 1
#define WINDOW_MOUSE_RIGHT 2
#define WINDOW_MOUSE_MIDDLE 4

#ifdef __cplusplus
extern "C" {
#endif
  extern bool window_create(const char *caption, int width, int height);
  extern void window_destroy();
  extern bool window_update();
  extern void window_swap_buffers();

  extern int window_client_width();
  extern int window_client_height();
  extern int window_cursor_x();
  extern int window_cursor_y();
  extern int window_mouse_x();
  extern int window_mouse_y();
  extern int window_mouse_z();
  extern bool window_iconified();
  extern bool window_focused();
  extern bool window_sized();

  extern void window_lock_cursor(bool lockCursor);
  extern bool window_cursor_locked();
  extern bool window_input_press(int key);
  extern bool window_input_down(int key);
  extern bool window_input_release(int key);
  
  extern const char *window_input_text();


#ifdef __cplusplus
}
#endif
#endif
