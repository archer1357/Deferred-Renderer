#ifndef WINDOW_H
#define WINDOW_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

/*
iconified when returning from been minimised.

on lost focus, mouse unlocked, keys released.

on iconic window_update will sleep.
*/

#ifdef __cplusplus
extern "C" {
#endif

  extern bool window_create(const char *caption,int width,int height);
  extern void window_destroy();
  extern bool window_update();
  extern void window_swap_buffers();

  extern int window_client_width();
  extern int window_client_height();

  extern bool window_sized();
  extern bool window_restored();

  extern void window_lock_cursor();

  extern int window_cursor_x();
  extern int window_cursor_y();
  extern int window_mouse_x();
  extern int window_mouse_y();
  extern int window_mouse_z();

  extern bool window_key_press(int k);
  extern bool window_key_down(int k);
  extern bool window_key_release(int k);

#ifdef __cplusplus
}
#endif

#define KEY_UKNOWN -1

#define KEY_SPACE 32
#define KEY_APOSTROPHE 222
#define KEY_COMMA 188
#define KEY_MINUS 189
#define KEY_PERIOD 190
#define KEY_SLASH 191

#define KEY_0 48
#define KEY_1 49
#define KEY_2 50
#define KEY_3 51
#define KEY_4 52
#define KEY_5 53
#define KEY_6 54
#define KEY_7 55
#define KEY_8 56
#define KEY_9 57

#define KEY_SEMICOLON  186
#define KEY_EQUAL 187

#define KEY_A 65
#define KEY_B 66
#define KEY_C 67
#define KEY_D 68
#define KEY_E 69
#define KEY_F 70
#define KEY_G 71
#define KEY_H 72
#define KEY_I 73
#define KEY_J 74
#define KEY_K 75
#define KEY_L 76
#define KEY_M 77
#define KEY_N 78
#define KEY_O 79
#define KEY_P 80
#define KEY_Q 81
#define KEY_R 82
#define KEY_S 83
#define KEY_T 84
#define KEY_U 85
#define KEY_V 86
#define KEY_W 87
#define KEY_X 88
#define KEY_Y 89
#define KEY_Z 90

#define KEY_LEFT_BRACKET  219
#define KEY_BACKSLASH 220
#define KEY_RIGHT_BRACKET 221
#define KEY_GRAVE_ACCENT  192

#define KEY_ESCAPE 27
#define KEY_RETURN 13
#define KEY_TAB 9
#define KEY_BACKSPACE 8
#define KEY_INSERT 45
#define KEY_DELETE 46
#define KEY_RIGHT 39
#define KEY_LEFT 37
#define KEY_DOWN 40
#define KEY_UP 38
#define KEY_PAGE_UP 33
#define KEY_PAGE_DOWN 34
#define KEY_HOME 36
#define KEY_END 35
#define KEY_CAPS_LOCK 20
#define KEY_SCROLL_LOCK 145
#define KEY_NUM_LOCK 144
#define KEY_PRINT_SCREEN 44
#define KEY_PAUSE 19

#define KEY_F1 112
#define KEY_F2 113
#define KEY_F3 114
#define KEY_F4 115
#define KEY_F5 116
#define KEY_F6 117
#define KEY_F7 118
#define KEY_F8 119
#define KEY_F9 120
#define KEY_F10 121
#define KEY_F11 122
#define KEY_F12 123

#define KEY_KP_0 96
#define KEY_KP_1 97
#define KEY_KP_2 98
#define KEY_KP_3 99
#define KEY_KP_4 100
#define KEY_KP_5 101
#define KEY_KP_6 102
#define KEY_KP_7 103
#define KEY_KP_8 104
#define KEY_KP_9 105

#define KEY_KP_DECIMAL 110
#define KEY_KP_DIVIDE 111
#define KEY_KP_MULTIPLY 106
#define KEY_KP_SUBTRACT 109
#define KEY_KP_ADD 107

#define KEY_SHIFT 16
#define KEY_CONTROL 17
#define KEY_ALT 18
#define KEY_WIN 91
#define KEY_APPS 93

#define MOUSE_LEFT 1
#define MOUSE_RIGHT 2
#define MOUSE_MIDDLE 4

#endif
