#include "Scene.h"

void Scene::setSmoothMouse(double smoothMousex, double smoothMouseY) {
  this->smoothMouseX=smoothMousex;
  this->smoothMouseY=smoothMouseY;
}
Scene::Scene(const std::string &mainScriptFn)
  : L(0),mainScriptFn(mainScriptFn),shadowDebug(false),geometryShadows(true),
    error(false),accumTime(0.0),stepTime(1.0/60.0),maxSteps(5),
    lastCursorX(0.0),lastCursorY(0.0),lastTime(0.0),lastCursorSet(false),
    lockCursor(false)
{

  smoothMouseX=0;
  smoothMouseY=0;

  mat4::identity(viewMat);


  L = luaL_newstate();
  luaL_openlibs(L);

  //

  //render
  lua_newtable(L);
  int render_ind=lua_gettop(L);

  //mesh_mt
  lua_newtable(L); //push mesh_mt
  int mesh_mt_ind=lua_gettop(L);

  lua_pushlightuserdata(L,this); //push this
  lua_pushcclosure(L,meshGc,1); //push meshGc, pop this
  lua_setfield(L,mesh_mt_ind,"__gc"); //pop meshGc

  lua_pushcfunction(L,meshNewIndex);
  lua_setfield(L,mesh_mt_ind,"__newindex");

  //
  lua_pushlightuserdata(L,this);
  lua_pushvalue(L,mesh_mt_ind);
  lua_pushcclosure(L,createMesh,2);
  lua_setfield(L,render_ind,"mesh");

  lua_pop(L,1); //pop mesh_mt

  //shadow_mt
  lua_newtable(L); //push shadow_mt
  int shadow_mt_ind=lua_gettop(L);

  lua_pushlightuserdata(L,this);
  lua_pushcclosure(L,shadowGc,1);
  lua_setfield(L,shadow_mt_ind,"__gc");

  lua_pushcfunction(L,shadowNewIndex);
  lua_setfield(L,shadow_mt_ind,"__newindex");

  //
  lua_pushlightuserdata(L,this);
  lua_pushvalue(L,shadow_mt_ind);
  lua_pushcclosure(L,createShadow,2);
  lua_setfield(L,render_ind,"shadow");

  lua_pop(L,1); //pop shadow_mt

  //pointlight_mt
  lua_newtable(L); //push pointlight_mt
  int pointlight_mt_ind=lua_gettop(L);

  lua_pushlightuserdata(L,this);
  lua_pushcclosure(L,pointlightGc,1);
  lua_setfield(L,pointlight_mt_ind,"__gc");

  lua_pushcfunction(L,pointlightNewIndex);
  lua_setfield(L,pointlight_mt_ind,"__newindex");

  //
  lua_pushlightuserdata(L,this);
  lua_pushvalue(L,pointlight_mt_ind);
  lua_pushcclosure(L,createPointlight,2);
  lua_setfield(L,render_ind,"pointlight");

  lua_pop(L,1); //pop pointlight_mt

  //spotlight
  lua_newtable(L); //push spotlight_mt
  int spotlight_mt_ind=lua_gettop(L);

  lua_pushlightuserdata(L,this);
  lua_pushcclosure(L,spotlightGc,1);
  lua_setfield(L,spotlight_mt_ind,"__gc");

  lua_pushcfunction(L,spotlightNewIndex);
  lua_setfield(L,spotlight_mt_ind,"__newindex");

  //
  lua_pushlightuserdata(L,this);
  lua_pushvalue(L,spotlight_mt_ind);
  lua_pushcclosure(L,createSpotlight,2);
  lua_setfield(L,render_ind,"spotlight");

  lua_pop(L,1); //pop spotlight_mt


  //directionallight
  lua_newtable(L); //push directionallight_mt
  int directionallight_mt_ind=lua_gettop(L);

  lua_pushlightuserdata(L,this);
  lua_pushcclosure(L,directionallightGc,1);
  lua_setfield(L,directionallight_mt_ind,"__gc");

  lua_pushcfunction(L,directionallightNewIndex);
  lua_setfield(L,directionallight_mt_ind,"__newindex");

  //
  lua_pushlightuserdata(L,this);
  lua_pushvalue(L,directionallight_mt_ind);
  lua_pushcclosure(L,createDirectionallight,2);
  lua_setfield(L,render_ind,"directionallight");

  lua_pop(L,1); //pop spotlight_mt

  //
  lua_pushlightuserdata(L,this);
  lua_pushcclosure(L,renderNewIndex,1);
  lua_setfield(L,render_ind,"__newindex");

  lua_pushlightuserdata(L,this);
  lua_pushcclosure(L,renderIndex,1);
  lua_setfield(L,render_ind,"__index");

  lua_pushvalue(L,render_ind);
  lua_setmetatable(L,render_ind);

  //
  lua_setglobal(L,"render"); //pop render

  //
  if(luaL_dofile(L,mainScriptFn.c_str())) {
    error=true;
    std::cout << lua_tostring(L, -1) << std::endl;
    lua_pop(L,1);
  }
}

Scene::~Scene() {
  lua_close(L);
}

void Scene::run(double time,int clientWidth,int clientHeight) {
/*float zNear=1.0f, zFar=500.0f;

  mat4::perspective(projMat, (float)M_PI_4,(float)clientWidth/(float)clientHeight,zNear,zFar);
  mat4::inverse(invProjMat,projMat);

  lua_newtable(L); 
  int projMatInd=lua_gettop(L);

  for(int i=0;i<16;i++) {
    lua_pushinteger(L,i+1);
    lua_pushnumber(L,(double)projMat[i]);
    lua_settable(L,projMatInd);
  }

  lua_setglobal(L,"projMat");*/


  //
  if(accumTime==0.0) {
    accumTime=time;
  }

  if(lastTime == 0.0) {
    lastTime=time;
  }

  //
  lua_pushinteger(L,clientWidth);
  lua_setglobal(L,"clientWidth");
  lua_pushinteger(L,clientHeight);
  lua_setglobal(L,"clientHeight");
  lua_pushnumber(L,(double)time);
  lua_setglobal(L,"time");
  //

  int stepCount=0;

  while(stepCount<maxSteps && accumTime+stepTime < time) {
    accumTime+=stepTime;
    stepCount++;

    if(!error) {
      lua_getglobal(L,"step");

      if(!lua_isnil(L,-1)) {
        lua_pushnumber(L,(double)stepTime);

        if(lua_pcall(L, 1, 0, 0)) {
          error=true;
          std::cout << lua_tostring(L, -1) << std::endl;
          lua_pop(L,1);
        }
      } else {
        lua_pop(L,1);
      }
    }
  }

  if(stepCount==maxSteps) {
    accumTime=time;
  }

  double interpTime=time-accumTime;
  //
  if(!error) {
    lua_getglobal(L,"run");

    if(!lua_isnil(L,-1)) {
      lua_pushnumber(L,time-lastTime);
      lua_pushnumber(L,(double)stepTime);
      lua_pushnumber(L,(double)interpTime);

      if(lua_pcall(L, 3, 0, 0)) {
        error=true;
        std::cout << lua_tostring(L, -1) << std::endl;
        lua_pop(L,1);
      }
    } else {
      lua_pop(L,1);
    }
  }

  //
  lastTime=time;
}

void Scene::onKey(int key, int action, int mods) {
  lua_getglobal(L,"onKey");

  if(!lua_isnil(L,-1)) {
    lua_pushinteger(L,key);
    lua_pushinteger(L,action);
    lua_pushinteger(L,mods);

    if(lua_pcall(L, 3, 0, 0)) {
      error=true;
      std::cout << lua_tostring(L, -1) << std::endl;
      lua_pop(L,1);
    }
  } else {
    lua_pop(L,1);
  }
}

void Scene::onMouseButton(int button, int action, int mods) {
  lua_getglobal(L,"onMouseButton");

  if(!lua_isnil(L,-1)) {
    lua_pushinteger(L,button);
    lua_pushinteger(L,action);
    lua_pushinteger(L,mods);

    if(lua_pcall(L, 3, 0, 0)) {
      error=true;
      std::cout << lua_tostring(L, -1) << std::endl;
      lua_pop(L,1);
    }
  } else {
    lua_pop(L,1);
  }
}
void Scene::onChar(int c) {
  lua_getglobal(L,"onChar");

  if(!lua_isnil(L,-1)) {
    lua_pushinteger(L,c);

    if(lua_pcall(L, 1, 0, 0)) {
      error=true;
      std::cout << lua_tostring(L, -1) << std::endl;
      lua_pop(L,1);
    }
  } else {
    lua_pop(L,1);
  }
}
void Scene::onCursor(double x, double y) {
  if(!lastCursorSet) {
    lastCursorSet=true;
    lastCursorX=x;
    lastCursorY=y;
  }
  lua_getglobal(L,"onCursor");

  if(!lua_isnil(L,-1)) {
    lua_pushnumber(L,x);
    lua_pushnumber(L,y);
    lua_pushnumber(L,x-lastCursorX);
    lua_pushnumber(L,y-lastCursorY);

    if(lua_pcall(L, 4, 0, 0)) {
      error=true;
      std::cout << lua_tostring(L, -1) << std::endl;
      lua_pop(L,1);
    }
  } else {
    lua_pop(L,1);
  }

  lastCursorX=x;
  lastCursorY=y;
}

void Scene::onScroll(double x, double y) {
  lua_getglobal(L,"onScroll");

  if(!lua_isnil(L,-1)) {
    lua_pushnumber(L,(double)x);
    lua_pushnumber(L,(double)y);

    if(lua_pcall(L, 2, 0, 0)) {
      error=true;
      std::cout << lua_tostring(L, -1) << std::endl;
      lua_pop(L,1);
    }
  } else {
    lua_pop(L,1);
  }

}


const float *Scene::getViewMat() {
  return viewMat;
}

bool Scene::isShadowDebug() {
  return shadowDebug;
}

bool Scene::isGeometryShadows() {
  return geometryShadows;
}

const Scene::Meshes &Scene::getMeshes() {
  return meshes;
}
const Scene::Shadows &Scene::getShadows() {
  return shadows;
}
const Scene::Pointlights &Scene::getPointlights() {
  return pointlights;
}
const Scene::Spotlights &Scene::getSpotlights() {
  return spotlights;
}

const Scene::Directionallights &Scene::getDirectionallights() {
  return directionallights;
}

void Scene::onFileModified(const std::string &fn) {
  if(fn==mainScriptFn) {
    if(luaL_dofile(L,mainScriptFn.c_str())) {
    error=true;
    std::cout << lua_tostring(L, -1) << std::endl;
    lua_pop(L,1);
    }

    lastTime=0.0;
    lastCursorSet=false;

    lua_gc(L,LUA_GCCOLLECT,0);
    error=false;
  }
}

const std::string &Scene::getEnvMapTex() {
  return envMapTex;
}

bool Scene::isLockCursor() {
  return lockCursor;
}

int Scene::createMesh(lua_State *L) {
  Scene *scene=(Scene*)lua_touserdata(L,lua_upvalueindex(1));

  //
  SceneMesh *mesh=new SceneMesh();
  scene->meshes.insert(mesh);

  *(SceneMesh**)lua_newuserdata(L,sizeof(void*))=mesh;
  int userdataInd=lua_gettop(L);
  lua_pushvalue(L,lua_upvalueindex(2));
  lua_setmetatable(L,userdataInd);

  if(lua_istable(L,1)) {
    for(lua_pushnil(L); lua_next(L, 1); lua_pop(L, 1)) {
      int valInd=lua_gettop(L);
      int keyInd=valInd-1;
      lua_pushvalue(L,keyInd);
      lua_pushvalue(L,valInd);
      lua_settable(L, userdataInd);
    }
  }

  return 1;
}
int Scene::meshGc(lua_State *L) {
  Scene *scene=(Scene*)lua_touserdata(L,lua_upvalueindex(1));

  //
  SceneMesh *mesh=*(SceneMesh**)lua_touserdata(L,1);
  scene->meshes.erase(mesh);
  delete mesh;
  return 0;
}
int Scene::meshNewIndex(lua_State *L) {
  Scene *scene=(Scene*)lua_touserdata(L,lua_upvalueindex(1));

 SceneMesh *mesh=*(SceneMesh**)lua_touserdata(L,1);
  std::string k=luaL_checkstring(L,2);

  if(k=="color") {
    for(int i=0;i<3;i++) {
      lua_pushinteger(L,i+1);
      lua_gettable(L,3);
      mesh->color[i]=(float)luaL_checknumber(L,-1);
      lua_pop(L,1);
    }
  } else if(k=="emissive") {
    mesh->emissive=lua_toboolean(L,3)==1;
  } else if(k=="reflective") {
    mesh->reflective=(float)luaL_checknumber(L,3);
  } else if(k=="shininess") {
    mesh->shininess=(float)luaL_checknumber(L,3);
  } else if(k=="colorTex") {
    mesh->colorTex=luaL_checkstring(L,3);
  } else if(k=="normalTex") {
    mesh->normalTex=luaL_checkstring(L,3);
  } else if(k=="heightTex") {
    mesh->heightTex=luaL_checkstring(L,3);
  } else if(k=="specularTex") {
    mesh->specularTex=luaL_checkstring(L,3);
  } else if(k=="reliefTex") {
    mesh->reliefTex=luaL_checkstring(L,3);
  } else if(k=="geometry") {
    mesh->geometry=luaL_checkstring(L,3);
  } else if(k=="vao") {
    mesh->vao=luaL_checkstring(L,3);
  } else if(k=="draw") {
    mesh->draw=luaL_checkstring(L,3);
  } else if(k=="modelMat") {
    for(int i=0;i<16;i++) {
      lua_pushinteger(L,i+1);
      lua_gettable(L,3);
      mesh->modelMat[i]=(float)luaL_checknumber(L,-1);
      lua_pop(L,1);
    }

  } else if(k=="bumpBias") {
    mesh->bumpBias=(float)luaL_checknumber(L,3);
  } else if(k=="bumpScale") {
    mesh->bumpScale=(float)luaL_checknumber(L,3);
  }

  return 0;
}
int Scene::createShadow(lua_State *L) {
  Scene *scene=(Scene*)lua_touserdata(L,lua_upvalueindex(1));

  //
  SceneShadow *shadow=new SceneShadow();

  scene->shadows.insert(shadow);

  *(SceneShadow**)lua_newuserdata(L,sizeof(void*))=shadow;
  int userdataInd=lua_gettop(L);
  lua_pushvalue(L,lua_upvalueindex(2));
  lua_setmetatable(L,userdataInd);

  if(lua_istable(L,1)) {
    for(lua_pushnil(L); lua_next(L, 1); lua_pop(L, 1)) {
      int valInd=lua_gettop(L);
      int keyInd=valInd-1;
      lua_pushvalue(L,keyInd);
      lua_pushvalue(L,valInd);
      lua_settable(L, userdataInd);
    }
  }

  return 1;
}
int Scene::shadowGc(lua_State *L) {
  Scene *scene=(Scene*)lua_touserdata(L,lua_upvalueindex(1));

  //
  SceneShadow *shadow=*(SceneShadow**)lua_touserdata(L,1);
  scene->shadows.erase(shadow);
  delete shadow;

  return 0;
}
int Scene::shadowNewIndex(lua_State *L) {
  Scene *scene=(Scene*)lua_touserdata(L,lua_upvalueindex(1));

  SceneShadow *shadow=*(SceneShadow**)lua_touserdata(L,1);
  std::string k=luaL_checkstring(L,2);

  if(k=="modelMat") {
    for(int i=0;i<16;i++) {
      lua_pushinteger(L,i+1);
      lua_gettable(L,3);
      float a=(float)luaL_checknumber(L,-1);
      lua_pop(L,1);
      
        shadow->modelMat[i]=a;
   
    }
 
  } else if(k=="geometry") {
    std::string fn=luaL_checkstring(L,3);
    shadow->geometry=fn;
  } else if(k=="zpass") {
    bool b=lua_isnil(L,3)||(lua_isboolean(L,3) && lua_toboolean(L,3)==0);
    shadow->zpass=!b;

  } else if(k=="robust") {
    bool b=lua_isnil(L,3)||(lua_isboolean(L,3) && lua_toboolean(L,3)==0);
    shadow->robust=!b;
  }

  return 0;
}
int Scene::createPointlight(lua_State *L) {
  Scene *scene=(Scene*)lua_touserdata(L,lua_upvalueindex(1));

  //
  ScenePointLight *pointlight=new ScenePointLight();
  scene->pointlights.insert(pointlight);

  *(ScenePointLight**)lua_newuserdata(L,sizeof(void*))=pointlight;
  int userdataInd=lua_gettop(L);
  lua_pushvalue(L,lua_upvalueindex(2));
  lua_setmetatable(L,userdataInd);

  if(lua_istable(L,1)) {
    for(lua_pushnil(L); lua_next(L, 1); lua_pop(L, 1)) {
      int valInd=lua_gettop(L);
      int keyInd=valInd-1;
      lua_pushvalue(L,keyInd);
      lua_pushvalue(L,valInd);
      lua_settable(L, userdataInd);
    }
  }

  return 1;
}
int Scene::pointlightGc(lua_State *L) {
  Scene *scene=(Scene*)lua_touserdata(L,lua_upvalueindex(1));

  //
  ScenePointLight *pointlight=*(ScenePointLight**)lua_touserdata(L,1);
  scene->pointlights.erase(pointlight);
  delete pointlight;
  return 0;
}
int Scene::pointlightNewIndex(lua_State *L) {
  Scene *scene=(Scene*)lua_touserdata(L,lua_upvalueindex(1));

ScenePointLight *pointlight=*(ScenePointLight**)lua_touserdata(L,1);
  std::string k=luaL_checkstring(L,2);

  if(k=="color") {
    for(int i=0;i<3;i++) {
      lua_pushinteger(L,i+1);
      lua_gettable(L,3);
      pointlight->color[i]=(float)luaL_checknumber(L,-1);
      // std::cout << pointlight->color[i] << std::endl;
      lua_pop(L,1);
    }
  } else if(k=="attenuation") {
    for(int i=0;i<3;i++) {
      lua_pushinteger(L,i+1);
      lua_gettable(L,3);
      pointlight->attenuation[i]=(float)luaL_checknumber(L,-1);
      lua_pop(L,1);
    }
  } else if(k=="strength") {
    pointlight->strength=(float)luaL_checknumber(L,3);
  } else if(k=="shadow") {
    pointlight->shadow=lua_toboolean(L,3)==1;
  } else if(k=="pos") {
    for(int i=0;i<3;i++) {
      lua_pushinteger(L,i+1);
      lua_gettable(L,3);
      pointlight->pos[i]=(float)luaL_checknumber(L,-1);
      lua_pop(L,1);
    }
  }

  return 0;
}
int Scene::createSpotlight(lua_State *L) {
  Scene *scene=(Scene*)lua_touserdata(L,lua_upvalueindex(1));

  //
  SceneSpotLight *spotlight=new SceneSpotLight();
  scene->spotlights.insert(spotlight);

  *(SceneSpotLight**)lua_newuserdata(L,sizeof(void*))=spotlight;
  int userdataInd=lua_gettop(L);
  lua_pushvalue(L,lua_upvalueindex(2));
  lua_setmetatable(L,userdataInd);

  if(lua_istable(L,1)) {
    for(lua_pushnil(L); lua_next(L, 1); lua_pop(L, 1)) {
      int valInd=lua_gettop(L);
      int keyInd=valInd-1;
      lua_pushvalue(L,keyInd);
      lua_pushvalue(L,valInd);
      lua_settable(L, userdataInd);
    }
  }

  return 1;
}
int Scene::spotlightGc(lua_State *L) {
  Scene *scene=(Scene*)lua_touserdata(L,lua_upvalueindex(1));

  //
  SceneSpotLight *spotlight=*(SceneSpotLight**)lua_touserdata(L,1);
  scene->spotlights.erase(spotlight);
  delete spotlight;
  return 0;
}
int Scene::spotlightNewIndex(lua_State *L) {
  Scene *scene=(Scene*)lua_touserdata(L,lua_upvalueindex(1));

  SceneSpotLight *spotlight=*(SceneSpotLight**)lua_touserdata(L,1);
  std::string k=luaL_checkstring(L,2);

  if(k=="color") {
    for(int i=0;i<3;i++) {
      lua_pushinteger(L,i+1);
      lua_gettable(L,3);
      spotlight->color[i]=(float)luaL_checknumber(L,-1);
      lua_pop(L,1);
    }
  } else if(k=="attenuation") {
    for(int i=0;i<3;i++) {
      lua_pushinteger(L,i+1);
      lua_gettable(L,3);
      spotlight->attenuation[i]=(float)luaL_checknumber(L,-1);
      lua_pop(L,1);
    }
  } else if(k=="strength") {
    spotlight->strength=(float)luaL_checknumber(L,3);
  } else if(k=="spotExponent") {
    spotlight->spotExponent=(float)luaL_checknumber(L,3);
  } else if(k=="spotCutoff") {
    spotlight->spotCutoff=(float)luaL_checknumber(L,3);
  } else if(k=="shadow") {
    spotlight->shadow=lua_toboolean(L,3)==1;
 
  } else if(k=="pos") {
    for(int i=0;i<3;i++) {
      lua_pushinteger(L,i+1);
      lua_gettable(L,3);
      spotlight->pos[i]=(float)luaL_checknumber(L,-1);
      lua_pop(L,1);
    }
  } else if(k=="dir") {
    for(int i=0;i<3;i++) {
      lua_pushinteger(L,i+1);
      lua_gettable(L,3);
      spotlight->dir[i]=(float)luaL_checknumber(L,-1);
      lua_pop(L,1);
    }
  }

  return 0;
}

int Scene::createDirectionallight(lua_State *L) {
  Scene *scene=(Scene*)lua_touserdata(L,lua_upvalueindex(1));

  //
  SceneDirectionalLight *directionallight=new SceneDirectionalLight();
  scene->directionallights.insert(directionallight);

  *(SceneDirectionalLight**)lua_newuserdata(L,sizeof(void*))=directionallight;
  int userdataInd=lua_gettop(L);
  lua_pushvalue(L,lua_upvalueindex(2));
  lua_setmetatable(L,userdataInd);

  if(lua_istable(L,1)) {
    for(lua_pushnil(L); lua_next(L, 1); lua_pop(L, 1)) {
      int valInd=lua_gettop(L);
      int keyInd=valInd-1;
      lua_pushvalue(L,keyInd);
      lua_pushvalue(L,valInd);
      lua_settable(L, userdataInd);
    }
  }

  return 1;
}

int Scene::directionallightGc(lua_State *L) {
  Scene *scene=(Scene*)lua_touserdata(L,lua_upvalueindex(1));

  //
  SceneDirectionalLight *directionallight=*(SceneDirectionalLight**)lua_touserdata(L,1);
  scene->directionallights.erase(directionallight);
  delete directionallight;

  return 0;
}

int Scene::directionallightNewIndex(lua_State *L) {
  Scene *scene=(Scene*)lua_touserdata(L,lua_upvalueindex(1));

  SceneDirectionalLight *directionallight=*(SceneDirectionalLight**)lua_touserdata(L,1);
  std::string k=luaL_checkstring(L,2);

  if(k=="color") {
    for(int i=0;i<3;i++) {
      lua_pushinteger(L,i+1);
      lua_gettable(L,3);
      directionallight->color[i]=(float)luaL_checknumber(L,-1);
      lua_pop(L,1);
    }
  } else if(k=="strength") {
    directionallight->strength=(float)luaL_checknumber(L,3);
  } else if(k=="shadow") {
    directionallight->shadow=lua_toboolean(L,3)==1;
  } else if(k=="dir") {
    for(int i=0;i<3;i++) {
      lua_pushinteger(L,i+1);
      lua_gettable(L,3);
      directionallight->dir[i]=(float)luaL_checknumber(L,-1);
      lua_pop(L,1);
    }

  }

  return 0;
}
int Scene::renderNewIndex(lua_State *L) {
  Scene *scene=(Scene*)lua_touserdata(L,lua_upvalueindex(1));

  std::string k=luaL_checkstring(L,2);

  if(k=="viewMat") {
    for(int i=0;i<16;i++) {
      lua_pushinteger(L,i+1);
      lua_gettable(L,3);
      scene->viewMat[i]=(float)luaL_checknumber(L,-1);
      lua_pop(L,1);
    }
 
  } else if(k=="shadowDebug") {
    bool b=lua_isnil(L,3)||(lua_isboolean(L,3) && lua_toboolean(L,3)==0);
    scene->shadowDebug=!b;
  } else if(k=="geometryShadows") {
    bool b=lua_isnil(L,3)||(lua_isboolean(L,3) && lua_toboolean(L,3)==0);
    scene->geometryShadows=!b;
  } else if(k=="envMapTex") {
    scene->envMapTex=luaL_checkstring(L,3);
  } else if(k=="lockCursor") {
    bool b=lua_isnil(L,3)||(lua_isboolean(L,3) && lua_toboolean(L,3)==0);
    scene->lockCursor=!b;
  }

  return 0;
}

int Scene::renderIndex(lua_State *L) {
  Scene *scene=(Scene*)lua_touserdata(L,lua_upvalueindex(1));

  std::string k=luaL_checkstring(L,2);

  if(k=="shadowDebug") {
    lua_pushboolean(L,scene->shadowDebug?1:0);
  } else if(k=="geometryShadows") {
    lua_pushboolean(L,scene->geometryShadows?1:0);
  } else if(k=="lockCursor") {
    lua_pushboolean(L,scene->lockCursor?1:0);
  } else if(k=="smoothMouseX") {
    lua_pushnumber(L,scene->smoothMouseX);
  } else if(k=="smoothMouseY") {
    lua_pushnumber(L,scene->smoothMouseY);
  } else {
    return 0;
  }

  return 1;
}
