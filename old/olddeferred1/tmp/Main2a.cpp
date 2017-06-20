
void doMain(lua_State *L) {
  if(luaL_dofile(L,"data/script/Main.lua")) {
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
  
  } else if(k=="cubeEnvTex") {
      cubeEnvTex=textureManager->getCube(luaL_checkstring(L,3));

  }

  return 0;
}

int mylua_createMesh(lua_State *L) {
 
}

int mylua_Mesh_gc(lua_State *L) {
  
  return 0;
}

int mylua_Mesh_newindex(lua_State *L) {
 

  return 0;
}

int mylua_createShadow(lua_State *L) {

}

int mylua_Shadow_gc(lua_State *L) {

  return 0;
}

int mylua_Shadow_newindex(lua_State *L) {

  return 0;
}

int mylua_createPointlight(lua_State *L) {

}

int mylua_Pointlight_gc(lua_State *L) {

  return 0;
}

int mylua_Pointlight_newindex(lua_State *L) {
  

  return 0;
}

int mylua_createSpotlight(lua_State *L) {

}

int mylua_Spotlight_gc(lua_State *L) {

  return 0;
}

int mylua_Spotlight_newindex(lua_State *L) {
  
}

bool initLua() {
 

  //paths
  lua_getglobal(L, "package"); //push package
  lua_getfield(L, -1, "path"); //push path
  std::string path = std::string(lua_tostring(L,-1))+";data/script/?.lua;data/script/?.so;data/script/?.dll";
  lua_pop(L,1); //pop path
  lua_pushstring(L, path.c_str());
  lua_setfield(L,-2,"path");
  lua_pop(L,1); //pop package


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



  int majorVer,minorVer;
  glGetIntegerv(GL_MAJOR_VERSION,&majorVer);
  glGetIntegerv(GL_MINOR_VERSION,&minorVer);
  //std::cout << majorVer<<" "<<minorVer << std::endl;


  //
  
//
  if(majorVer>3 || minorVer>=3) {
    geometryShaderSupport=true;
    useGeometryShadows=true;
    programManager->setGeometryShaderSupport(true);
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

  int clientWidth=0,clientHeight=0,

  //
  UpdateListener *fileListener=new UpdateListener();
  FW::FileWatcher fileWatcher;
  FW::WatchID watchID0 = fileWatcher.addWatch("./data/shader", fileListener);
  FW::WatchID watchID1 = fileWatcher.addWatch("./data/script", fileListener);
  FW::WatchID watchID2 = fileWatcher.addWatch("./data/texture", fileListener);

  FW::WatchID watchID3 = fileWatcher.addWatch("./data/geometry", fileListener);

  //
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
    
    //

    if(!iconified) {
      //
      if(!errorFromGl && checkGlError()) {
        errorFromGl=true;
      }

      //

      //
      fileWatcher.update();
    

      //


      //
     

      render(clientWidth,clientHeight);


      //
      lastTime=time;

    } else {
      accumTime=time;
      lastTime=time;
    }

    //

  }

  //
 
  return 0;
}



void deferredInit() {
}

bool deferredSetup(int clientWidth, int clientHeight) {

}




