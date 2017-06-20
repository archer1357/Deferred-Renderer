#ifndef SCENE_H
#define SCENE_H

#define _USE_MATH_DEFINES
#include <cmath>
#include <lua.hpp>
#include <set>
#include "Mat4.h"


#include "SceneMesh.h"
#include "SceneShadow.h"
#include "ScenePointLight.h"
#include "SceneSpotLight.h"
#include "SceneDirectionalLight.h"
#include "GeometryVaoManager.h"
#include "TextureManager.h"

class Scene {
private:
lua_State *L;
public:
  typedef std::set<SceneMesh*> Meshes;
  typedef std::set<SceneShadow*> Shadows;
  typedef std::set<ScenePointLight*> Pointlights;
  typedef std::set<SceneSpotLight*> Spotlights;
  typedef std::set<SceneDirectionalLight*> Directionallights;
private:
  std::string mainScriptFn;
  float viewMat[16];

  bool shadowDebug;
  bool geometryShadows;
  bool error;
  double accumTime;
  int maxSteps;
  double stepTime;

  Meshes meshes;
  Shadows shadows;
  Pointlights pointlights;
Spotlights spotlights;
Directionallights directionallights;

  std::string envMapTex;
  double lastCursorX,lastCursorY,lastTime;
  bool lastCursorSet;
  bool lockCursor;
  double smoothMouseX,smoothMouseY;
public:
  void setSmoothMouse(double smoothMousex, double smoothMouseY);
Scene( const std::string &mainScriptFn);
  ~Scene();
  void run(double time,int clientWidth,int clientHeight);
  void onKey(int key, int action, int mods);
  void onMouseButton(int button, int action, int mods);
  void onChar(int c);
  void onCursor(double x, double y);
  void onScroll(double x, double y);

  const float *getViewMat();
  bool isShadowDebug();
  bool isGeometryShadows();
  const Meshes &getMeshes();
  const Shadows &getShadows();
  const Pointlights &getPointlights();
  const Spotlights &getSpotlights();
const Directionallights &getDirectionallights();
  void onFileModified(const std::string &fn);
  const std::string &getEnvMapTex();
  bool isLockCursor();
private:
  static int createMesh(lua_State *L);
  static int meshGc(lua_State *L);
  static int meshNewIndex(lua_State *L);
  static int createShadow(lua_State *L);
  static int shadowGc(lua_State *L);
  static int shadowNewIndex(lua_State *L);
  static int createPointlight(lua_State *L);
  static int pointlightGc(lua_State *L);
  static int pointlightNewIndex(lua_State *L);
  static int createSpotlight(lua_State *L);
  static int spotlightGc(lua_State *L);
  static int spotlightNewIndex(lua_State *L);
  static int createDirectionallight(lua_State *L);
  static int directionallightGc(lua_State *L);
  static int directionallightNewIndex(lua_State *L);
  static int renderNewIndex(lua_State *L);
  static int renderIndex(lua_State *L);

};

#endif
