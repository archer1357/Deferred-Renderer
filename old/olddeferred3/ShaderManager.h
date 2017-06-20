#ifndef PROGRAMMANAGER_H
#define PROGRAMMANAGER_H

#include "gl_core_3_3.h"
#include <string>
#include <set>
#include <map>
#include <list>

class ShaderManager {
private:
  struct Source {
    bool ok;
    std::list<std::pair<std::string,bool>> srcs;
    std::set<std::string> vertexShaderDeps,geometryShaderDeps,fragmentShaderDeps;
  };

  struct Shader {
    bool ok;
    GLuint obj;
    std::set<std::string> programDeps,sourceDeps;
  };

  struct Program {
    bool ok;
    GLuint obj;
    std::set<std::string> vertexShaderDeps,geometryShaderDeps,fragmentShaderDeps;
  };

private:
  std::map<std::string,Program*> programs;
  std::map<std::string,Source*> sources;
  std::map<std::string,Shader*> vertexShaders,geometryShaders,fragmentShaders;
public:
  ShaderManager();
  ~ShaderManager();
  GLuint get(const std::string &vsFn,const std::string &gsFn,const std::string &fsFn);
  bool reloadSource(const std::string &fn);
  bool removeProgram(const std::string &vsFn,const std::string &gsFn,const std::string &fsFn);
  void clear();
private:
  Source *getSource(const std::string &fn);
  Shader *getVertexShader(const std::string &fn);
  Shader *getGeometryShader(const std::string &fn);
  Shader *getFragmentShader(const std::string &fn);
  Program *getProgram(const std::string &vsFn,const std::string &gsFn,const std::string &fsFn);
  bool loadSource(Source *source,const std::string &fn);
  bool loadShader(Shader *shader,GLenum type,const std::string &fn);
  bool loadProgram(Program *program,const std::string &vsFn,const std::string &gsFn,const std::string &fsFn);
  bool linkProgram(Program *program,const std::string &key);
};
#endif
