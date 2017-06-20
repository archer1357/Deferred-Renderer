#define _USE_MATH_DEFINES

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <set>
#include "ProgramManager.h"
#include "GeometryVaoManager.h"
#include "TextureManager.h"

#include <lua.hpp>

#include "Mesh.h"
#include "Shadow.h"
#include "Pointlight.h"
#include "Spotlight.h"

//#include "FileWatcher.h"

bool checkGlError();
void deferredInit();
bool deferredSetup(int clientWidth, int clientHeight);
void printGLExts();
void printVers();

void onKey(GLFWwindow *window,int key,int scancode,int action, int mods);
void onMouseButton(GLFWwindow *window,int button,int action,int mods);
void onCursor(GLFWwindow *window,double x,double y);
void onScroll(GLFWwindow *window,double xoffset,double yoffset);
void onCursorEnter(GLFWwindow *window,int entered);
void onChar(GLFWwindow *window,unsigned int c);

