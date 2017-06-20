#include "Window.h"


Window::Window(const std::string &title, int width, int height)
  : window(0), iconified(false), focused(true), clientWidth(0), clientHeight(0)
{
  if(!glfwInit()) {
    std::cout << "GLFW init error.\n";
    return ;
  }

  window = glfwCreateWindow(width, height, title.c_str(), 0, 0);

  if(!window) {
    std::cout << "GLFW window creation error.\n";
    return ;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  //glfw callbacks
  glfwSetKeyCallback(window, onKey);
  glfwSetMouseButtonCallback(window, onMouseButton);
  glfwSetCursorPosCallback(window, onCursor);
  glfwSetScrollCallback(window, onScroll);
  glfwSetWindowIconifyCallback(window,onIconify);
  glfwSetWindowFocusCallback(window,onFocus);
  glfwSetCursorEnterCallback(window, onCursorEnter);
  glfwSetCharCallback(window, onChar);

  //
  glfwSetWindowUserPointer(window, this);

}


Window::~Window() {
  glfwTerminate();
}

bool Window::run() {
  keyInputs.clear();
  mouseButtonInputs.clear();
  cursorInputs.clear();
  scrollInputs.clear();
  charInputs.clear();

  glfwPollEvents();

  if(glfwWindowShouldClose(window)) {
    return false;
  }

  glfwGetWindowSize(window, &clientWidth, &clientHeight);

  return true;
}
void Window::swapBuffers() {
  glfwSwapBuffers(window);
}
void Window::lockCursor(bool lock) {
  glfwSetInputMode(window,GLFW_CURSOR, lock?GLFW_CURSOR_DISABLED:GLFW_CURSOR_NORMAL);
}
int Window::getClientWidth() {
  return clientWidth;
}
int Window::getClientHeight() {
  return clientHeight;
}
bool Window::isIconified() {
  return iconified;
}
bool Window::isFocused() {
  return focused;
}
const std::list<WindowKey> &Window::getKeyInputs() {
  return keyInputs;
}
const std::list<WindowMouseButton> &Window::getMouseButtonInputs() {
  return mouseButtonInputs;
}
const std::list<WindowCursor> &Window::getCursorInputs() {
  return cursorInputs;
}
const std::list<char> &Window::getCharInputs() {
  return charInputs;
}

void Window::onKey(GLFWwindow *window,int key,int scancode,int action, int mods) {
  if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  } else {
    Window *w=(Window*)glfwGetWindowUserPointer(window);
    WindowKey k;
    k.key=key;
    k.action=action;
    k.mods=mods;
    w->keyInputs.push_back(k);
  }
}
void Window::onMouseButton(GLFWwindow *window,int button,int action,int mods) {
  Window *w=(Window*)glfwGetWindowUserPointer(window);
  WindowMouseButton mb;
  mb.button=button;
  mb.action=action;
  mb.mods=mods;
  w->mouseButtonInputs.push_back(mb);
}
void Window::onCursor(GLFWwindow *window,double x,double y) {
  Window *w=(Window*)glfwGetWindowUserPointer(window);
  WindowCursor c;
  c.x=x;
  c.y=y;
  w->cursorInputs.push_back(c);
}
void Window::onScroll(GLFWwindow *window,double x,double y) {
  Window *w=(Window*)glfwGetWindowUserPointer(window);
  WindowScroll s;
  s.x=x;
  s.y=y;
  w->scrollInputs.push_back(s);
}
void Window::onCursorEnter(GLFWwindow *window,int entered) {
  Window *w=(Window*)glfwGetWindowUserPointer(window);
}
void Window::onChar(GLFWwindow *window,unsigned int c) {
  Window *w=(Window*)glfwGetWindowUserPointer(window);
  w->charInputs.push_back(c);
}
void Window::onIconify(GLFWwindow *window, int iconify) {
  Window *w=(Window*)glfwGetWindowUserPointer(window);
  w->iconified=iconify==GL_TRUE;
}
void Window::onFocus(GLFWwindow *window, int focus) {
  Window *w=(Window*)glfwGetWindowUserPointer(window);
  w->focused=focus==GL_TRUE;
}
