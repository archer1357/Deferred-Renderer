//gcc luatest.c -llua -o luatest && ./luatest
//gcc luatest.c -llua -DUSE_MODULE -o luatest.so -shared -Wl,-soname,luatest.so.1
// -ldl -fPIC

#ifdef _WIN32
#define LIBRARY_API __declspec(dllexport)
#else
#define LIBRARY_API
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

struct Object {
  int a,b;
};

int objectCreate(lua_State *L) {
  int n=lua_gettop(L);

  int mtInd=lua_upvalueindex(1);

  struct Object *obj;
  obj=(struct Object*)lua_newuserdata(L,sizeof(struct Object));
  obj->a=0;
  obj->b=0;
  int objInd=lua_gettop(L);

  lua_pushvalue(L,mtInd);
  lua_setmetatable(L,objInd);

  return 1;
}

int objectDestroy(lua_State *L) {
  struct Object *obj;
  obj=(struct Object*)lua_touserdata(L,1);
  printf("object destroyed (%i %i).\n",obj->a,obj->b);
  return 0;
}

int objectCalc(lua_State *L) {
  struct Object *obj;
  obj=(struct Object*)lua_touserdata(L,1);
  int r=obj->a+obj->b;
  lua_pushinteger(L,r);
  return 1;
}

int objectIndex(lua_State *L) {
  int methodTblInd=lua_upvalueindex(1);

  struct Object *obj;
  obj=(struct Object*)lua_touserdata(L,1);
  const char *k=luaL_checkstring(L,2);
  // printf("nil %i\n",lua_isnil(L,1));
  // printf("%s\n",luaL_typename(L,1));


  lua_pushvalue(L,2);
  lua_gettable(L,methodTblInd);//left unpopped

  if(!lua_isnil(L,-1)) {
  } else if(0==strcmp(k,"a")) {
    lua_pushinteger(L,obj->a);
  } else if(0==strcmp(k,"b")) {
    lua_pushinteger(L,obj->b);
  } else {
    return 0;
  }

  return 1;
}

int objectNewIndex(lua_State *L) {
  struct Object *obj;
  obj=(struct Object*)lua_touserdata(L,1);
  const char *k=luaL_checkstring(L,2);

  if(0==strcmp(k,"a")) {
    obj->a=luaL_checkinteger(L,3);
  } else if(0==strcmp(k,"b")) {
    obj->b=luaL_checkinteger(L,3);
  } else {
    return 0;
  }

  return 1;
}

int objectToString(lua_State *L) {
  struct Object *obj;
  char tmp[128];
  obj=(struct Object*)lua_touserdata(L,1);
  sprintf(tmp,"Object(%i,%i)",obj->a,obj->b);
  lua_pushstring(L,tmp);
  return 1;
}

void objectModule(lua_State *L) {
  lua_newtable(L); //push mt
  int mtInd=lua_gettop(L);

  lua_newtable(L); //push methodTbl
  int methodTblInd=lua_gettop(L);

  lua_pushcfunction(L,objectNewIndex);
  lua_setfield(L,mtInd,"__newindex");

  lua_pushvalue(L,methodTblInd); //push methodTbl2
  lua_pushcclosure(L,objectIndex,1); //pop methodTbl2
  // lua_pushcfunction(L,objectIndex);
  lua_setfield(L,mtInd,"__index");

  lua_pushcfunction(L,objectDestroy);
  lua_setfield(L,mtInd,"__gc");

  lua_pushcfunction(L,objectToString);
  lua_setfield(L,mtInd,"__tostring");

  lua_pushcfunction(L,objectCalc);
  lua_setfield(L,methodTblInd,"calc");

  lua_pushvalue(L,mtInd); //push mt2
  lua_pushcclosure(L,objectCreate,1); //pop mt2
  lua_setglobal(L,"createObject");

  lua_pop(L,2); //pop mt, methodTbl
}

int loadScript(lua_State *L) {
  int params=lua_gettop(L);
  const char *fn=luaL_checkstring(L,1);

  //new_env
  lua_newtable(L); //push new_env
  int new_env=lua_gettop(L);

  //new_env.mt=new_env
  lua_pushvalue(L,new_env); //push new_env#2
  lua_setmetatable(L,new_env); //pop new_env#2

  //thunk
  // if(luaL_loadfile(L, fn)) { //push thunk
  //   luaL_error(L,lua_tostring(L, -1));
  // }

  if(luaL_loadstring(L, fn)) { //push thunk
    luaL_error(L,lua_tostring(L, -1));
  }

  int thunk=lua_gettop(L);

  //new_env.__index = arg2 or thunk.env
  if(params>1 && lua_istable(L,2)) {
    lua_pushvalue(L,2);
  } else {
#if LUA_VERSION_NUM==501
    lua_getfenv(L,thunk); //push old_env
#else
    lua_getupvalue(L,thunk,1); //push old_env
#endif
  }

  lua_setfield(L,new_env,"__index"); //pop old_env

  //thunk.env=newenv
  lua_pushvalue(L,new_env); //push new_env#3

#if LUA_VERSION_NUM==501
  lua_setfenv(L,thunk); //pop new_env#3
#else
  lua_setupvalue(L,thunk,1); //pop new_env#3
#endif

  //call thunk
  lua_call(L, 0, 0); //pop thunk

  //remove new_env mt
  lua_pushnil(L);
  lua_setmetatable(L,new_env);

  //return new_env
  return 1;
}

int myfunc(lua_State *L) {
  int n=lua_gettop(L);

  int ind=1;
  if(n>0 && lua_istable(L,ind)) {
    printf("begin %i\n",lua_gettop(L));

    for(lua_pushnil(L);lua_next(L, ind);lua_pop(L, 1)) {
      int valInd=lua_gettop(L);
      int keyInd=valInd-1;

      printf("  ");

      if(lua_isnumber(L,keyInd) || lua_isstring(L,keyInd)) {
        printf("%s",lua_tostring(L,keyInd));
      } else {
        printf("%p",lua_topointer(L,keyInd));
      }

      printf(" => ");

      if(lua_isnumber(L,valInd) || lua_isstring(L,valInd)) {
        printf("%s",lua_tostring(L,valInd));
      } else {
        printf("%p",lua_topointer(L,valInd));
      }

      printf("\n");

    }

    printf("end %i\n",lua_gettop(L));
  }

  return 0;
}

int callAdd3Nums(lua_State *L,int a,int b,int c) {
  lua_getglobal (L,"add3nums");

  if(!lua_isfunction(L,-1)) {
    lua_pop(L,1);
    return 0;
  }

  //
  lua_pushinteger(L,a);
  lua_pushinteger(L,b);
  lua_pushinteger(L,c);

  if(lua_pcall(L, 3, 1, 0)) {
    printf("%s\n",lua_tostring(L,-1));
    lua_pop(L,1);
  }

  if(!lua_isnumber(L,-1)) {
    return 0;
  }

  return lua_tointeger(L,-1);
}

#ifndef USE_MODULE
int main() {
  lua_State *L;
  L = luaL_newstate();
  luaL_openlibs(L);

  lua_pushcfunction(L,myfunc);
  lua_setglobal(L,"myfunc");

  lua_pushcfunction(L,loadScript);
  lua_setglobal(L,"loadScript");

  objectModule(L);

  // if(luaL_dofile(L,"script.lua")) {
  //   const char *e=lua_tostring(L,-1);
  //   printf("%s\n",e);
  //   lua_pop(L,1);
  //   return 1;
  // }

  const char script[]=
    "q={}; q2={}; u={a=1,b=q2};u[q]=2;"
    "setmetatable(u,{__mode='kv'});"
    "q=nil; q2=nil;"
    "myfunc(u); collectgarbage(); myfunc(u);"
    "print('-----------------------------------------------')"
    "t=loadScript(\"print('another'); val=123; abc={}\");"
    "for k,v in pairs(t) do print('',k,v) end;"
    "print('-----------------------------------------------')"
    "obj=createObject();"
    "obj.a=123; obj.b=456;"
    "print(obj,obj.a,obj.b,obj:calc());"
    "obj=null;collectgarbage();"
    "print('-----------------------------------------------')"
    "function add3nums(a,b,c) return a+b+c; end;";

  if(luaL_dostring(L,script)) {
    const char *e=lua_tostring(L,-1);
    printf("%s\n",e);
    lua_pop(L,1);
    return 1;
  }

  printf("calling add3nums = %i\n",callAdd3Nums(L,1,2,3));

  // lua_gc(L,LUA_GCCOLLECT,0);
  lua_close(L);
  return 0;
}
#else
LIBRARY_API int luaopen_luatest(lua_State *L) {
  lua_register(L, "loadScript",loadScript);
  lua_register(L, "myfunc",myfunc);
  objectModule(L);
  return 0;
}
#endif
