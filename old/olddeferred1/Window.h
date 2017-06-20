#ifndef WINDOW
#define WINDOW

#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <list>
//#include "input.h"

struct WindowKey {int key,action,mods;};
struct WindowMouseButton {int button,action,mods;};
struct WindowCursor {double x,y;};
struct WindowScroll {double x,y;};

class Window {
private:
  GLFWwindow *window;
  bool iconified;
  bool focused;
  int clientWidth,clientHeight;
  std::list<WindowKey> keyInputs;
  std::list<WindowMouseButton> mouseButtonInputs;
  std::list<WindowCursor> cursorInputs;
  std::list<WindowScroll> scrollInputs;
  std::list<char> charInputs;
public:
  Window(const std::string &title, int width, int height);
  ~Window();
  bool run();
  void swapBuffers();
  void lockCursor(bool lock);
  int getClientWidth();
  int getClientHeight();
  bool isIconified();
  bool isFocused();
  const std::list<WindowKey> &getKeyInputs();
  const std::list<WindowMouseButton> &getMouseButtonInputs();
  const std::list<WindowCursor> &getCursorInputs();
  const std::list<WindowScroll> &getScrollInputs();
  const std::list<char> &getCharInputs();
  double getSmoothMouseX();
  double getSmoothMouseY();
  double getDeltaTime();
private:
  static void onKey(GLFWwindow *window,int key,int scancode,int action, int mods);
  static void onMouseButton(GLFWwindow *window,int button,int action,int mods);
  static void onCursor(GLFWwindow *window,double x,double y);
  static void onScroll(GLFWwindow *window,double x,double y);
  static void onCursorEnter(GLFWwindow *window,int entered);
  static void onChar(GLFWwindow *window,unsigned int c);
  static void onIconify(GLFWwindow *window, int iconify);
  static void onFocus(GLFWwindow *window, int focus);
};

#endif
