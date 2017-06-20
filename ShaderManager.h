#ifndef PROGRAMMANAGER_H
#define PROGRAMMANAGER_H

#include "gl_core_3_3.h"
#include <string>
#include <set>
#include <map>
#include <list>
#include <sstream>
#include <fstream>

#include "FileModified.h"

class ShaderManager {
private:
  struct Source {
    bool ok;
    std::string src;
    std::set<std::string> shaderDeps;


    FileModified modified;
  };

  struct Shader {
    bool ok;
    GLuint obj;
    std::set<std::string> programDeps;
    std::string sourceDep;
    std::string defsKey, defsSrc;
    GLenum type;

    std::string src; //for debug
  };

  struct Program {
    bool ok;
    GLuint obj;
    std::set<std::string> shaderDeps;
  };
private:
  std::map<std::string,Program*> programs;
  std::map<std::string,Source*> sources;
  std::map<std::string,Shader*> shaders;
public:
  ShaderManager();
  ~ShaderManager();
  GLuint get(const std::string &vsFn,const std::string &gsFn,const std::string &fsFn,
    const std::string &vsDefs,const std::string &gsDefs,const std::string &fsDefs); //ok
  bool reloadSource(const std::string &fn); //fix
  bool removeProgram(const std::string &vsFn,const std::string &gsFn,const std::string &fsFn,
    const std::string &vsDefs,const std::string &gsDefs,const std::string &fsDefs); //todo
  void clear(); //ignore for now
  void refresh();
private:
  bool reloadSource(Source *source,const std::string &fn,bool force);  
  
  Source *getSource(const std::string &fn); //ok for now ish
  std::map<std::string,Shader*>::iterator getShader(GLenum type,const std::string &fn,const std::string &defs);
  Program *getProgram(const std::string &vsFn,const std::string &gsFn,const std::string &fsFn, //ok
    const std::string &vsDefs,const std::string &gsDefs,const std::string &fsDefs);
  bool loadSource(Source *source,const std::string &fn); //check
  bool loadShader(Shader *shader,const std::string &key); //ok i think
  bool loadProgram(Program *program,const std::string &vsFn,const std::string &gsFn,const std::string &fsFn,
    const std::string &vsDefs,const std::string &gsDefs,const std::string &fsDefs); //okish
  bool linkProgram(Program *program,const std::string &key); //check
  //std::string getProgramKey(const std::string &vsFn,const std::string &gsFn,const std::string &fsFn, //ok
  //  const std::string &vsDefs,const std::string &gsDefs,const std::string &fsDefs);
  //std::string getShaderKey(GLenum type,const std::string &fn,const std::string &defs);
public:
  std::string getShaderSource(GLenum type, const std::string &fn,const std::string &defs);
};
#endif
