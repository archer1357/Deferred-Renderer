#include "Main.h"

GLFWwindow *window;
lua_State *L;

bool scriptError=false;
bool iconified=false, focused=true;
bool geometryShaderSupport=false;
bool errorFromGl=false;
bool useGeometryShadows=false;
bool shadowDebug=false;

ProgramManager programManager;
TextureManager textureManager;
GeometryVaoManager geometryVaoManager;

GLuint deferredFbo=0,deferredColorTex=0,deferredNormalTex=0,deferredDepthTex=0;

float projMat[16];
float invProjMat[16];
float viewRotProjMat[16];
// float shadowProjMat[16];

std::set<Mesh*> meshes;
std::set<Shadow*> shadows;
std::set<Pointlight*> pointlights;
std::set<Spotlight*> spotlights;

int mylua_enableGeometryShadows(lua_State *L) {
  if(lua_isnil(L,1)) {
    useGeometryShadows=false;
  } else if(lua_isboolean(L,1)  && lua_toboolean(L,1)==0) {
    useGeometryShadows=false;
  } else {
    useGeometryShadows=true;
  }

  return 0;
}

int mylua_enableShadowDebug(lua_State *L) {
  if(lua_isnil(L,1)) {
    shadowDebug=false;
  } else if(lua_isboolean(L,1)  && lua_toboolean(L,1)==0) {
    shadowDebug=false;
  } else {
    shadowDebug=true;
  }

  return 0;
}

void doMain(lua_State *L) {
  if(luaL_dofile(L,"data/Main.lua")) {
    scriptError=true;
    std::cout << lua_tostring(L, -1) << std::endl;
    lua_pop(L,1);
  }

  lua_gc(L,LUA_GCCOLLECT,0);
  scriptError=false;
}

int mylua_lockCursor(lua_State *L) {
  if(lua_gettop(L) ==1 && lua_isboolean(L,1)) {
    if(lua_toboolean(L,1)==0) {
      glfwSetInputMode(window,GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
      glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_NORMAL);
    }
  } else {
    luaL_error(L,"Invalid input.");
  }

  return 0;
}

int mylua_render_newindex(lua_State *L) {
  std::string k=luaL_checkstring(L,2);

  if(k=="invProjMat") {
    for(int i=0;i<16;i++) {
      lua_pushinteger(L,i+1);
      lua_gettable(L,3);
      invProjMat[i]=(float)luaL_checknumber(L,-1);
      lua_pop(L,1);
    }
  } else if(k=="viewRotProjMat") {
    for(int i=0;i<16;i++) {
      lua_pushinteger(L,i+1);
      lua_gettable(L,3);
      viewRotProjMat[i]=(float)luaL_checknumber(L,-1);
      lua_pop(L,1);
    }
  } else if(k=="projMat") {
    for(int i=0;i<16;i++) {
      lua_pushinteger(L,i+1);
      lua_gettable(L,3);
      projMat[i]=(float)luaL_checknumber(L,-1);
      lua_pop(L,1);
    }
    // } else if(k=="shadowProjMat") {
    //   for(int i=0;i<16;i++) {
    //     lua_pushinteger(L,i+1);
    //     lua_gettable(L,3);
    //     shadowProjMat[i]=(float)luaL_checknumber(L,-1);
    //     lua_pop(L,1);
    //   }
  }

  return 0;
}

int mylua_createMesh(lua_State *L) {
  Mesh *mesh=new Mesh;
  meshes.insert(mesh);


  *(Mesh**)lua_newuserdata(L,sizeof(void*))=mesh;
  int userdataInd=lua_gettop(L);
  lua_pushvalue(L,lua_upvalueindex(1));
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

int mylua_Mesh_gc(lua_State *L) {
  Mesh *mesh=*(Mesh**)lua_touserdata(L,1);
  meshes.erase(mesh);
  delete mesh;
  return 0;
}

int mylua_Mesh_newindex(lua_State *L) {
  Mesh *mesh=*(Mesh**)lua_touserdata(L,1);
  std::string k=luaL_checkstring(L,2);

  if(k=="color") {
    for(int i=0;i<3;i++) {
      lua_pushinteger(L,i+1);
      lua_gettable(L,3);
      mesh->color[i]=(float)luaL_checknumber(L,-1);
      lua_pop(L,1);
    }
  } else if(k=="emissive") {
    mesh->emissive=(float)luaL_checknumber(L,3);
  } else if(k=="reflective") {
    mesh->reflective=(float)luaL_checknumber(L,3);
  } else if(k=="shininess") {
    mesh->shininess=(float)luaL_checknumber(L,3);
  } else if(k=="colorTex") {
    mesh->colorTex=lua_isnil(L,3)?0:textureManager.get2d(luaL_checkstring(L,3));
  } else if(k=="normalTex") {
    mesh->normalTex=lua_isnil(L,3)?0:textureManager.get2d(luaL_checkstring(L,3));
  } else if(k=="heightTex") {
    mesh->heightTex=lua_isnil(L,3)?0:textureManager.get2d(luaL_checkstring(L,3));
  } else if(k=="specularTex") {
    mesh->specularTex=lua_isnil(L,3)?0:textureManager.get2d(luaL_checkstring(L,3));
  } else if(k=="geometryVaoDraw") {
    if(lua_isnil(L,3)) {
      mesh->vao=0;
      mesh->draw=0;
    } else {
      lua_pushinteger(L,1);
      lua_gettable(L,3);
      std::string geometry=luaL_checkstring(L,-1);

      lua_pushinteger(L,2);
      lua_gettable(L,3);
      std::string vao=luaL_checkstring(L,-1);

      lua_pushinteger(L,3);
      lua_gettable(L,3);
      std::string draw=luaL_checkstring(L,-1);

      if(GeometryVao *gv=geometryVaoManager.get(geometry,vao)) {
        mesh->vao=gv->getVao();
        mesh->draw=gv->getDraw(draw);
      }
    }
  } else if(k=="modelViewProjMat") {
    for(int i=0;i<16;i++) {
      lua_pushinteger(L,i+1);
      lua_gettable(L,3);
      mesh->modelViewProjMat[i]=(float)luaL_checknumber(L,-1);
      lua_pop(L,1);
    }
  } else if(k=="normalMat") {
    for(int i=0;i<9;i++) {
      lua_pushinteger(L,i+1);
      lua_gettable(L,3);
      mesh->normalMat[i]=(float)luaL_checknumber(L,-1);
      lua_pop(L,1);
    }
  } else if(k=="modelViewMat") {
    for(int i=0;i<16;i++) {
      lua_pushinteger(L,i+1);
      lua_gettable(L,3);
      mesh->modelViewMat[i]=(float)luaL_checknumber(L,-1);
      lua_pop(L,1);
    }
  } else if(k=="material") {
    std::string v=luaL_checkstring(L,3);

    if(v=="Color") {
      mesh->material=Mesh::Color;
    } else if(v=="Normal") {
      mesh->material=Mesh::Normal;
    } else if(v=="Bump") {
      mesh->material=Mesh::Bump;
    } else if(v=="Relief") {
      mesh->material=Mesh::Relief;
    } else if(v=="Parallax") {
      mesh->material=Mesh::Parallax;
    }

  } else if(k=="bumpBias") {
    mesh->bumpBias=(float)luaL_checknumber(L,3);
  } else if(k=="bumpScale") {
    mesh->bumpScale=(float)luaL_checknumber(L,3);
  } else if(k=="unlit") {
    mesh->unlit=lua_toboolean(L,3)==1;
  }

  return 0;
}

int mylua_createShadow(lua_State *L) {
  Shadow *shadow=new Shadow();

  if(geometryShaderSupport) {
    shadow->gpuShadow=new GpuShadow();
  }

  shadow->cpuShadow=new CpuShadow();

  shadows.insert(shadow);


  *(Shadow**)lua_newuserdata(L,sizeof(void*))=shadow;
  int userdataInd=lua_gettop(L);
  lua_pushvalue(L,lua_upvalueindex(1));
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

int mylua_Shadow_gc(lua_State *L) {
  Shadow *shadow=*(Shadow**)lua_touserdata(L,1);
  shadows.erase(shadow);
  delete shadow;
  return 0;
}

int mylua_Shadow_newindex(lua_State *L) {
  Shadow *shadow=*(Shadow**)lua_touserdata(L,1);
  std::string k=luaL_checkstring(L,2);

  if(k=="modelMat") {
    for(int i=0;i<16;i++) {
      lua_pushinteger(L,i+1);
      lua_gettable(L,3);
      float a=(float)luaL_checknumber(L,-1);
      lua_pop(L,1);

      if(shadow->cpuShadow) {
        shadow->cpuShadow->modelMat[i]=a;
      }

      if(shadow->gpuShadow) {
        shadow->gpuShadow->modelMat[i]=a;
      }

    }
  } else if(k=="modelViewMat") {
    for(int i=0;i<16;i++) {
      lua_pushinteger(L,i+1);
      lua_gettable(L,3);
      float a=(float)luaL_checknumber(L,-1);
      lua_pop(L,1);

      if(shadow->cpuShadow) {
        shadow->cpuShadow->modelViewMat[i]=a;
      }

      if(shadow->gpuShadow) {
        shadow->gpuShadow->modelViewMat[i]=a;
      }
    }
  } else if(k=="geometry_cpu" && shadow->cpuShadow) {
    std::string fn=luaL_checkstring(L,3);

    shadow->cpuShadow->shadowGeometry=geometryVaoManager.getShadow(fn);

  } else if(k=="geometry_gpu" && shadow->gpuShadow) {
    std::string fn=luaL_checkstring(L,3);
    GeometryVao *gv=geometryVaoManager.get(fn,"Positions.vao.json");
    shadow->gpuShadow->vao=gv->getVao();
    shadow->gpuShadow->draw=gv->getDraw("default");
  }

  return 0;
}

int mylua_createPointlight(lua_State *L) {
  Pointlight *pointlight=new Pointlight;
  pointlights.insert(pointlight);

  *(Pointlight**)lua_newuserdata(L,sizeof(void*))=pointlight;
  int userdataInd=lua_gettop(L);
  lua_pushvalue(L,lua_upvalueindex(1));
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

int mylua_Pointlight_gc(lua_State *L) {
  Pointlight *pointlight=*(Pointlight**)lua_touserdata(L,1);
  pointlights.erase(pointlight);
  delete pointlight;
  return 0;
}

int mylua_Pointlight_newindex(lua_State *L) {
  Pointlight *pointlight=*(Pointlight**)lua_touserdata(L,1);
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
  } else if(k=="viewPos") {
    for(int i=0;i<4;i++) {
      lua_pushinteger(L,i+1);
      lua_gettable(L,3);
      pointlight->viewPos[i]=(float)luaL_checknumber(L,-1);
      lua_pop(L,1);
    }
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

int mylua_createSpotlight(lua_State *L) {
  Spotlight *spotlight=new Spotlight;
  spotlights.insert(spotlight);


  *(Spotlight**)lua_newuserdata(L,sizeof(void*))=spotlight;
  int userdataInd=lua_gettop(L);
  lua_pushvalue(L,lua_upvalueindex(1));
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

int mylua_Spotlight_gc(lua_State *L) {
  Spotlight *spotlight=*(Spotlight**)lua_touserdata(L,1);
  spotlights.erase(spotlight);
  delete spotlight;
  return 0;
}

int mylua_Spotlight_newindex(lua_State *L) {
  Spotlight *spotlight=*(Spotlight**)lua_touserdata(L,1);
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
  } else if(k=="viewPos") {
    for(int i=0;i<4;i++) {
      lua_pushinteger(L,i+1);
      lua_gettable(L,3);
      spotlight->viewPos[i]=(float)luaL_checknumber(L,-1);
      lua_pop(L,1);
    }
  } else if(k=="viewDir") {
    for(int i=0;i<3;i++) {
      lua_pushinteger(L,i+1);
      lua_gettable(L,3);
      spotlight->viewDir[i]=(float)luaL_checknumber(L,-1);
      lua_pop(L,1);
    }
  }

  return 0;
}

bool initLua() {
  L = luaL_newstate();
  luaL_openlibs(L);

  //paths
  lua_getglobal(L, "package"); //push package
  lua_getfield(L, -1, "path"); //push path
  std::string path = std::string(lua_tostring(L,-1))+";data/?.lua;data/?.so;data/?.dll";
  lua_pop(L,1); //pop path
  lua_pushstring(L, path.c_str());
  lua_setfield(L,-2,"path");
  lua_pop(L,1); //pop package

  //render
  lua_newtable(L);
  int render_ind=lua_gettop(L);

  //mesh_mt
  lua_newtable(L); //push mesh_mt
  int mesh_mt_ind=lua_gettop(L);

  lua_pushcfunction(L,mylua_Mesh_gc);
  lua_setfield(L,mesh_mt_ind,"__gc");

  lua_pushcfunction(L,mylua_Mesh_newindex);
  lua_setfield(L,mesh_mt_ind,"__newindex");

  //
  lua_pushcclosure(L,mylua_createMesh,1); //pop mesh_mt
  lua_setfield(L,render_ind,"mesh");

  //shadow_mt
  lua_newtable(L); //push shadow_mt
  int shadow_mt_ind=lua_gettop(L);

  lua_pushcfunction(L,mylua_Shadow_gc);
  lua_setfield(L,shadow_mt_ind,"__gc");

  lua_pushcfunction(L,mylua_Shadow_newindex);
  lua_setfield(L,shadow_mt_ind,"__newindex");

  //
  lua_pushcclosure(L,mylua_createShadow,1); //pop mesh_mt
  lua_setfield(L,render_ind,"shadow");

  //pointlight_mt
  lua_newtable(L); //push pointlight_mt
  int pointlight_mt_ind=lua_gettop(L);

  lua_pushcfunction(L,mylua_Pointlight_gc);
  lua_setfield(L,pointlight_mt_ind,"__gc");

  lua_pushcfunction(L,mylua_Pointlight_newindex);
  lua_setfield(L,pointlight_mt_ind,"__newindex");

  //
  lua_pushcclosure(L,mylua_createPointlight,1); //pop pointlight_mt
  lua_setfield(L,render_ind,"pointlight");

  //spotlight
  lua_newtable(L); //push spotlight_mt
  int spotlight_mt_ind=lua_gettop(L);

  lua_pushcfunction(L,mylua_Spotlight_gc);
  lua_setfield(L,spotlight_mt_ind,"__gc");

  lua_pushcfunction(L,mylua_Spotlight_newindex);
  lua_setfield(L,spotlight_mt_ind,"__newindex");

  //
  lua_pushcclosure(L,mylua_createSpotlight,1); //pop spotlight_mt
  lua_setfield(L,render_ind,"spotlight");

  //

  lua_pushcfunction(L,mylua_render_newindex);
  lua_setfield(L,render_ind,"__newindex");

  lua_pushvalue(L,render_ind);
  lua_setmetatable(L,render_ind);

  //
  lua_setglobal(L,"render"); //pop render

  //
  lua_pushcfunction(L,mylua_lockCursor);
  lua_setglobal(L,"lockCursor");

  //
  lua_pushcfunction(L,mylua_enableGeometryShadows);
  lua_setglobal(L,"enableGeometryShadows");

  //
  lua_pushcfunction(L,mylua_enableShadowDebug);
  lua_setglobal(L,"enableShadowDebug");
  //
  doMain(L);
  return true;
}

void render(int clientWidth, int clientHeight) {
  GLuint screenVao=geometryVaoManager.get("Screen.geomvao.json")->getVao();
  GeometryDraw *screenDraw=geometryVaoManager.get("Screen.geomvao.json")->getDraw();

  glViewport(0,0,clientWidth,clientHeight);

  //states
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  glDisable(GL_BLEND);
  glDisable(GL_STENCIL_TEST);

  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LEQUAL);

  //bind fbo
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, deferredFbo);

  //clear
  glClearColor(0.0f,0.0f,0.0f,0.0f);
  glClearDepth(1);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  //states
  glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

  //draw entity geom depths

  //states
  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

  //draw entity geoms
  {
    for(Mesh *m : meshes) {
      GLuint prog=0;


      if(m->material==Mesh::Color) {
        prog=programManager.get("GeometryColored.prog.json");

      } else if(m->material==Mesh::Parallax) {
        prog=programManager.get("GeometryBump.prog.json");
        // std::cout << "p\n";
      }

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D,m->colorTex);

      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D,m->normalTex);

      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D,m->heightTex);

      glUseProgram(prog);
      int modelViewProjMatLoc=glGetUniformLocation(prog,"u_modelViewProjMat");
      int normalMatLoc=glGetUniformLocation(prog,"u_normalMat");
      int modelViewMatLoc=glGetUniformLocation(prog,"u_modelViewMat");

      int colLoc=glGetUniformLocation(prog,"u_col");
      int shininessLoc=glGetUniformLocation(prog,"u_shininess");
      int emissiveLoc=glGetUniformLocation(prog,"u_emissive");
      int reflectiveLoc=glGetUniformLocation(prog,"u_reflective");


      int bumpScaleLoc=glGetUniformLocation(prog,"u_bumpScale");
      int bumpBiasLoc=glGetUniformLocation(prog,"u_bumpBias");
      int unlitLoc=glGetUniformLocation(prog,"u_unlit");

      if(colLoc!=-1) {
        glUniform3fv(colLoc,1,m->color);
      }

      glUniformMatrix3fv(normalMatLoc,1,GL_TRUE,m->normalMat);
      glUniformMatrix4fv(modelViewProjMatLoc,1,GL_TRUE,m->modelViewProjMat);

      if(modelViewMatLoc!=-1) {
        glUniformMatrix4fv(modelViewMatLoc,1,GL_TRUE,m->modelViewMat);
      }

      if(shininessLoc!=-1) {
        glUniform1fv(shininessLoc,1,&m->shininess);
      }

      if(emissiveLoc!=-1) {
        glUniform1fv(emissiveLoc,1,&m->emissive);
      }

      if(reflectiveLoc!=-1) {
        glUniform1fv(reflectiveLoc,1,&m->reflective);
      }

      if(bumpScaleLoc!=-1) {
        glUniform1fv(bumpScaleLoc,1,&m->bumpScale);
      }

      if(bumpBiasLoc!=-1) {
        glUniform1fv(bumpBiasLoc,1,&m->bumpBias);
      }

      if(unlitLoc!=-1) {
        int unlit=m->unlit?1:0;
        glUniform1iv(unlitLoc,1,&unlit);
      }
      if(m->vao) {
        glBindVertexArray(m->vao);

        if(m->draw) {
          m->draw->draw();
        }
      }
    }
  }

  //bind fbo
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //bind textures
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, deferredColorTex);
  glBindSampler(0,0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, deferredNormalTex);
  glBindSampler(1,0);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, deferredDepthTex);
  glBindSampler(2,0);

  //states
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

  //clear
  glClearDepth(1.0f);
  glClear(GL_DEPTH_BUFFER_BIT);

  //render deferred depth texture
  glUseProgram(programManager.get("DepthCopy.prog.json"));

  glBindVertexArray(screenVao);
  screenDraw->draw();

  //states
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glDepthMask(GL_FALSE);

  //clear
  glClearColor(0.0f,0.0f,0.0f,1.0f);
  glClearStencil(0);
  glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);


  //bind vao
  glBindVertexArray(screenVao);

  //states
  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_ONE, GL_ONE);

  //draw point lights
  {
    GLuint p=programManager.get("DeferredPointLight.prog.json");
    glUseProgram(p);
    GLint invProjLoc=glGetUniformLocation(p,"u_invProjMat");
    glUniformMatrix4fv(invProjLoc,1,GL_TRUE,invProjMat);

    GLint lightPosLoc=glGetUniformLocation(p,"u_lightPos");
    GLint lightAttenLoc=glGetUniformLocation(p,"u_lightAtten");
    GLint lightColLoc=glGetUniformLocation(p,"u_lightCol");
    GLint strengthLoc=glGetUniformLocation(p,"u_strength");

    for(Pointlight *pl : pointlights) {
      if(!pl->shadow) {
        glUniform3fv(lightPosLoc,1,pl->viewPos);
        glUniform3fv(lightAttenLoc,1,pl->attenuation);
        glUniform3fv(lightColLoc,1,pl->color);
        glUniform1fv(strengthLoc,1,&pl->strength);
        screenDraw->draw();
      }
    }
  }

  //states
  glEnable(GL_STENCIL_TEST);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_ONE, GL_ONE);

  //draw point lights with shadows
  {
    //
    GLuint lightProg=programManager.get("DeferredPointLight.prog.json");

    GLuint invProjLoc=glGetUniformLocation(lightProg,"u_invProjMat");
    GLint lightPosLoc=glGetUniformLocation(lightProg,"u_lightPos");
    GLint lightAttenLoc=glGetUniformLocation(lightProg,"u_lightAtten");
    GLint lightColLoc=glGetUniformLocation(lightProg,"u_lightCol");
    GLint strengthLoc=glGetUniformLocation(lightProg,"u_strength");

    glUseProgram(lightProg);
    glUniformMatrix4fv(invProjLoc,1,GL_TRUE,invProjMat);

    //
    GLuint shadowProg=0;

    if(geometryShaderSupport && useGeometryShadows) {
      shadowProg=programManager.get("ShadowVolumeGpu.prog.json");
    } else {
      shadowProg=programManager.get("ShadowVolumeCpu.prog.json");
    }

    GLint projMatLoc2=glGetUniformLocation(shadowProg,"u_projMat");
    GLint modelViewMatLoc2=glGetUniformLocation(shadowProg,"u_modelViewMat");
    GLint lightPosLoc2=glGetUniformLocation(shadowProg,"u_lightPos");

    glUseProgram(shadowProg);
    glUniformMatrix4fv(projMatLoc2,1,GL_TRUE,projMat);

    //
    for(Pointlight *pl : pointlights) {
      if(pl->shadow) {
        glDisable(GL_BLEND);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glDisable(GL_CULL_FACE);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_DEPTH_CLAMP);
        glStencilFunc(GL_ALWAYS, 0, 0xff);
        glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
        glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(1.0f,1.0f);

        glClearStencil(0);
        glClear(GL_STENCIL_BUFFER_BIT);

        //use shadow program
        glUseProgram(shadowProg);
        glUniform4fv(lightPosLoc2,1,pl->viewPos);

        //
        if(geometryShaderSupport && useGeometryShadows) {
          glEnable(GL_PRIMITIVE_RESTART);

          for(Shadow *shadow : shadows) {
            if(shadow->gpuShadow && shadow->gpuShadow->vao &&
               shadow->gpuShadow->draw) {
              glUniformMatrix4fv(modelViewMatLoc2,1,GL_TRUE,shadow->gpuShadow->modelViewMat);

              glBindVertexArray(shadow->gpuShadow->vao);
              shadow->gpuShadow->draw->draw();
            }
          }

          glDisable(GL_PRIMITIVE_RESTART);
        } else {
          for(Shadow *shadow : shadows) {
            if(shadow->cpuShadow && shadow->cpuShadow->shadowGeometry) {
              glUniformMatrix4fv(modelViewMatLoc2,1,GL_TRUE,shadow->cpuShadow->modelViewMat);

              shadow->cpuShadow->calc(pl,false);
              glBindVertexArray(shadow->cpuShadow->vao);
              shadow->cpuShadow->draw();
            }
          }
        }

        //draw light

        glDisable(GL_POLYGON_OFFSET_FILL);
        glEnable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_DEPTH_CLAMP);
        glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

        glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
        glStencilFunc(GL_EQUAL, 0x0, 0xFF);

        glUseProgram(lightProg);
        glUniform3fv(lightPosLoc,1,pl->viewPos);
        glUniform3fv(lightAttenLoc,1,pl->attenuation);
        glUniform3fv(lightColLoc,1,pl->color);
        glUniform1fv(strengthLoc,1,&pl->strength);
        glBindVertexArray(screenVao);

        screenDraw->draw();
      }
    }
  }

  //
  glDisable(GL_STENCIL_TEST);

  //states
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  glDepthMask(GL_FALSE);
  glDisable(GL_DEPTH_TEST);
  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

  //bind vao
  glBindVertexArray(screenVao);

  //draw ambients
  {
    // GLuint prog=programManager.get("DeferredAmbient.prog.json");
    // GLuint ambienceLoc=glGetUniformLocation(prog,"u_ambience");
    // glUseProgram(prog);
    // glUniform1f(ambienceLoc,0.05f);
  }
  // screenDraw->draw();

  //draw emissives
  glUseProgram(programManager.get("DeferredEmissive.prog.json"));
  screenDraw->draw();

  //draw color test
  // glUseProgram(programManager.get("DeferredColors.prog.json"));
  // screenDraw->draw();
/*
  //states
  glDisable(GL_BLEND);
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_ALWAYS, 0, 0xff);
  glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
  
  //clear stencil
  glClearStencil(0);
  glClear(GL_STENCIL_BUFFER_BIT);

  //stencil out sky area
  glUseProgram(programManager.get("DepthStencil.prog.json"));
  //screenDraw->draw();

  //states
  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
  glStencilFunc(GL_EQUAL, 0x0, 0xff);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

  //draw sky
  if(0){
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_CUBE_MAP,textureManager.getCube("blue_light.cube"));

    GLuint skyProg=programManager.get("SkyBox.prog.json");
    GeometryVao *gv=geometryVaoManager.get("SkyBox.geomvao.json");

    glUseProgram(skyProg);
    GLuint loc=glGetUniformLocation(skyProg,"u_viewRotProjMat");
    glUniformMatrix4fv(loc,1,GL_TRUE,viewRotProjMat);

    glBindVertexArray(gv->getVao());
    gv->getDraw()->draw();
  }

  //temporary test
  if(shadowDebug) {
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_DEPTH_CLAMP);

    GLuint shadowProg=0;

    if(geometryShaderSupport && useGeometryShadows) {
      shadowProg=programManager.get("ShadowVolumeGpuDebug.prog.json");
      //shadowProg=programManager.get("ShadowVolumeGpu.prog.json");
    } else {
      shadowProg=programManager.get("ShadowVolumeCpu.prog.json");
    }

    glUseProgram(shadowProg);
    GLint projMatLoc2=glGetUniformLocation(shadowProg,"u_projMat");
    glUniformMatrix4fv(projMatLoc2,1,GL_TRUE,projMat);

    GLint modelViewMatLoc2=glGetUniformLocation(shadowProg,"u_modelViewMat");
    GLint lightPosLoc2=glGetUniformLocation(shadowProg,"u_lightPos");

    for(Pointlight *pl : pointlights) {
      if(pl->shadow) {
        glUniform4fv(lightPosLoc2,1,pl->viewPos);

        if(geometryShaderSupport && useGeometryShadows) {
          glEnable(GL_PRIMITIVE_RESTART);


          for(Shadow *shadow : shadows) {
            if(shadow->gpuShadow && shadow->gpuShadow->vao && shadow->gpuShadow->draw) {
              glUniformMatrix4fv(modelViewMatLoc2,1,GL_TRUE,shadow->gpuShadow->modelViewMat);

              glBindVertexArray(shadow->gpuShadow->vao);
              shadow->gpuShadow->draw->draw();
            }
          }

          glDisable(GL_PRIMITIVE_RESTART);
        } else {

          for(Shadow *shadow : shadows) {
            if(shadow->cpuShadow && shadow->cpuShadow->shadowGeometry) {
              glUniformMatrix4fv(modelViewMatLoc2,1,GL_TRUE,shadow->cpuShadow->modelViewMat);

              shadow->cpuShadow->calc(pl,true);
              glBindVertexArray(shadow->cpuShadow->vao);

              shadow->cpuShadow->draw();

            }
          }
        }
      }
    }
    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_CLAMP);
  }*/
}
/*
class UpdateListener : public FW::FileWatchListener {
public:
  void handleFileAction(FW::WatchID watchid,const FW::String& dir,
                        const FW::String& filename,FW::Action action) {

    //
    if(action == FW::Action::Delete || action == FW::Action::Modified) {
      programManager.onFileModified(filename);
      textureManager.onFileModified(filename);
      geometryVaoManager.onFileModified(filename);
    }

    //
    if(action == FW::Action::Modified) {
      lua_getglobal(L,"onFileModified");
      lua_pushstring(L,filename.c_str());

      if(lua_pcall(L, 1, 1, 0)) {
        scriptError=true;
        std::cout << lua_tostring(L, -1) << std::endl;
        lua_pop(L,1);
      }

      if(!lua_isnil(L,-1) && (!lua_isboolean(L,-1) || lua_toboolean(L,-1))) {
        doMain(L);
      }

      lua_pop(L,1);
    }

    //
    std::cout << "\"" << filename << "\" ";

    if(action == FW::Action::Add) {
      std::cout << "added";
    } else if(action == FW::Action::Delete) {
      std::cout << "deleted";
    } else if(action == FW::Action::Modified) {
      std::cout << "modified";
    }

    std::cout << "." << std::endl;
  }
};
*/
void windowiconify(GLFWwindow *window, int iconify) {
  iconified=iconify==GL_TRUE;
}
void windowfocus(GLFWwindow *window, int focus) {
  focused=focus==GL_TRUE;
}


int main() {
  //glfw
  if(!glfwInit()) {
    return -1;
  }

  window = glfwCreateWindow(640, 480, "Hello World", 0, 0);

  if(!window) {
    glfwTerminate();
    return -2;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  glfwSetKeyCallback(window, onKey);
  glfwSetMouseButtonCallback(window, onMouseButton);
  glfwSetCursorPosCallback(window, onCursor);
  glfwSetScrollCallback(window, onScroll);
  glfwSetWindowIconifyCallback(window,windowiconify);
  glfwSetWindowFocusCallback(window,windowfocus);
  glfwSetCursorEnterCallback(window, onCursorEnter);
  glfwSetCharCallback(window, onChar);

  //glew
  GLenum glewErr = glewInit();

  if(GLEW_OK != glewErr) {
    std::cout << "GLEW Error " << glewGetErrorString(glewErr) << std::endl;
    return -3;
  }

  int majorVer,minorVer;
  glGetIntegerv(GL_MAJOR_VERSION,&majorVer);
  glGetIntegerv(GL_MINOR_VERSION,&minorVer);
  //std::cout << majorVer<<" "<<minorVer << std::endl;

  if(majorVer>3 || minorVer>=3) {
    geometryShaderSupport=true;
    useGeometryShadows=true;
    programManager.setGeometryShaderSupport(true);
    // std::cout <<"geometryShaderSupport=true\n";
    glPrimitiveRestartIndex(-1);
  }

  //
  printVers();
  // printGLExts();

  //
  deferredInit();

  //
  initLua();
  //
  const double stepTime=1.0/60.0;
  double accumTime=0.0;
  const int maxSteps=5;

  int clientWidth=0,clientHeight=0,lastClientWidth=0,lastClientHeight=0;

  //
  //FW::FileWatcher fileWatcher;
  //FW::WatchID watchID = fileWatcher.addWatch("./data", new UpdateListener());

  //
  while(!glfwWindowShouldClose(window)) {
    double time = glfwGetTime();
    static double lastTime=time;

    //

    if(!iconified) {
      //
      if(!errorFromGl && checkGlError()) {
        errorFromGl=true;
      }

      //

      glfwGetWindowSize(window, &clientWidth, &clientHeight);

      //
      //fileWatcher.update();
      //
      lua_pushinteger(L,clientWidth);
      lua_setglobal(L,"clientWidth");
      lua_pushinteger(L,clientHeight);
      lua_setglobal(L,"clientHeight");
      lua_pushnumber(L,(double)time);
      lua_setglobal(L,"time");

      //
      if(accumTime==0) {
        accumTime=time;
      }

      int stepCount=0;

      while(stepCount<maxSteps && accumTime+stepTime < time) {
        accumTime+=stepTime;
        stepCount++;

        if(!scriptError) {
          lua_getglobal(L,"step");

          if(!lua_isnil(L,-1)) {
            lua_pushnumber(L,(double)stepTime);

            if(lua_pcall(L, 1, 0, 0)) {
              scriptError=true;
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
      if(!scriptError) {
        lua_getglobal(L,"run");

        if(!lua_isnil(L,-1)) {
          lua_pushnumber(L,(double)stepTime);
          lua_pushnumber(L,(double)interpTime);

          if(lua_pcall(L, 2, 0, 0)) {
            scriptError=true;
            std::cout << lua_tostring(L, -1) << std::endl;
            lua_pop(L,1);
          }
        } else {
          lua_pop(L,1);
        }
      }

      //
      if(lastClientWidth != clientWidth || lastClientHeight != clientHeight) {
        deferredSetup(clientWidth?clientWidth:1, clientHeight?clientHeight:1);
      }

      render(clientWidth,clientHeight);


      //
      lastClientWidth=clientWidth;
      lastClientHeight=clientHeight;
      lastTime=time;

    } else {
      accumTime=time;
      lastTime=time;
    }

    //
    glfwSwapBuffers(window);
    glfwPollEvents();

  }

  //
  glfwTerminate();
  return 0;
}


bool checkGlError() {
  static const char *errors[]={
    "invalid enum","invalid value","invalid operation",
    "invalid frame buffer operation","out of memory"};

  static const GLenum codes[]={
    GL_INVALID_ENUM,GL_INVALID_VALUE,GL_INVALID_OPERATION,
    GL_INVALID_FRAMEBUFFER_OPERATION,GL_OUT_OF_MEMORY};

  int errCode = glGetError();

  if(errCode != GL_NO_ERROR) {


    for(int i=0;i<5;i++) {
      if(codes[i]==errCode) {
        std::cout << "gl error : " << errors[i] << std::endl;
        return true;
      }
    }
  }

  return false;
}

void deferredInit() {
  glGenFramebuffers(1,&deferredFbo);
  glGenTextures(1,&deferredColorTex);
  glGenTextures(1,&deferredNormalTex);
  glGenTextures(1,&deferredDepthTex);
}

bool deferredSetup(int clientWidth, int clientHeight) {
  std::cout << "deferredSetup " << clientWidth << ", "
            << clientHeight << std::endl;

  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, deferredFbo);

  //color
  glBindTexture(GL_TEXTURE_2D, deferredColorTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,clientWidth,clientHeight,0,GL_RGBA,GL_FLOAT,0);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,
                         GL_TEXTURE_2D,deferredColorTex,0);

  //normal
  glBindTexture(GL_TEXTURE_2D, deferredNormalTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D,0,
               // GL_RGBA16F
               GL_RGBA32F
               ,clientWidth,clientHeight,0,GL_RGBA,
               // GL_HALF_FLOAT
               GL_FLOAT
               ,0);

  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT1,
                         GL_TEXTURE_2D,deferredNormalTex,0);

  //depth
  glBindTexture(GL_TEXTURE_2D, deferredDepthTex);
  glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT32F,clientWidth,clientHeight,
               0,GL_DEPTH_COMPONENT,GL_FLOAT,0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,
                         GL_TEXTURE_2D,deferredDepthTex,0);

  //
  GLenum drawBufs[]={GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1};
  glDrawBuffers(2,drawBufs);

  //check fbo status
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

  if(status == GL_FRAMEBUFFER_COMPLETE) {
    //restore default FBO
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
  } else if(status==GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
    std::cout << "fbo error: incomplete attachment\n";
  } else if(status==GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) {
    std::cout << "fbo error: incomplete missing attachment\n";
  } else if(status==GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER) {
    std::cout << "fbo error: incomplete draw buffer\n";
  } else if(status==GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER) {
    std::cout << "fbo error: incomplete read buffer\n";
  } else if(status==GL_FRAMEBUFFER_UNSUPPORTED) {
    std::cout << "fbo error: unsupported\n";
  }

  return false;
}


void printGLExts() {
  int extNum;
  glGetIntegerv(GL_NUM_EXTENSIONS,&extNum);

  for(int i=0;i<extNum;i++) {
    std::cout << glGetStringi(GL_EXTENSIONS,i) << std::endl;
  }
}

void printVers() {
  std::cout << "GLFW " << glfwGetVersionString() << std::endl;
  std::cout << "GLEW " << glewGetString(GLEW_VERSION) << std::endl;
  std::cout << "GL " << glGetString(GL_VERSION) << std::endl;
  std::cout << "Shader version " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  std::cout << std::endl;
}


void onKey(GLFWwindow *window,int key,int scancode,int action, int mods) {
  if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  } else {
    lua_getglobal(L,"onKey");

    if(!lua_isnil(L,-1)) {
      lua_pushinteger(L,key);
      lua_pushinteger(L,scancode);
      lua_pushinteger(L,action);
      lua_pushinteger(L,mods);

      if(lua_pcall(L, 4, 0, 0)) {
        scriptError=true;
        std::cout << lua_tostring(L, -1) << std::endl;
        lua_pop(L,1);
      }
    } else {
      lua_pop(L,1);
    }
  }


}

void onMouseButton(GLFWwindow *window,int button,int action,int mods) {
  lua_getglobal(L,"onMouseButton");

  if(!lua_isnil(L,-1)) {
    lua_pushinteger(L,button);
    lua_pushinteger(L,action);
    lua_pushinteger(L,mods);

    if(lua_pcall(L, 3, 0, 0)) {
      scriptError=true;
      std::cout << lua_tostring(L, -1) << std::endl;
      lua_pop(L,1);
    }
  } else {
    lua_pop(L,1);
  }
}

void onCursor(GLFWwindow *window,double x,double y) {
  lua_getglobal(L,"onCursor");

  if(!lua_isnil(L,-1)) {
    lua_pushnumber(L,(double)x);
    lua_pushnumber(L,(double)y);

    if(lua_pcall(L, 2, 0, 0)) {
      scriptError=true;
      std::cout << lua_tostring(L, -1) << std::endl;
      lua_pop(L,1);
    }
  } else {
    lua_pop(L,1);
  }
}

void onScroll(GLFWwindow *window,double x,double y) {
  lua_getglobal(L,"onScroll");

  if(!lua_isnil(L,-1)) {
    lua_pushnumber(L,(double)x);
    lua_pushnumber(L,(double)y);

    if(lua_pcall(L, 2, 0, 0)) {
      scriptError=true;
      std::cout << lua_tostring(L, -1) << std::endl;
      lua_pop(L,1);
    }
  } else {
    lua_pop(L,1);
  }
}

void onCursorEnter(GLFWwindow *window,int entered) {
}

void onChar(GLFWwindow *window,unsigned int c) {
  lua_getglobal(L,"onChar");

  if(!lua_isnil(L,-1)) {
    lua_pushinteger(L,c);

    if(lua_pcall(L, 1, 0, 0)) {
      scriptError=true;
      std::cout << lua_tostring(L, -1) << std::endl;
      lua_pop(L,1);
    }
  } else {
    lua_pop(L,1);
  }
}
