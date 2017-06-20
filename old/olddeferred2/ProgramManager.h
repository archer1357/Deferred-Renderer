#include <GL/glew.h>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <jsoncons/json.hpp>

class ProgramManager {
private:
  std::map<std::string,GLuint> programs;
  // std::map<std::string,std::list<std::string>> programsShaderSources;
  // std::map<std::string,std::list<std::string>> programsShaderSources;
  std::map<std::string,std::string> sources;
  std::map<std::string,GLuint> shaders;
  bool getSource(const std::string &fn,std::string &out);
  bool getShader(GLenum type, const std::string &fn,const std::string &defines,GLuint &out);
  bool geometryShaderSupport;

public:
  ProgramManager();
  ~ProgramManager();
  GLuint get(const std::string &fn);
  void onFileModified(const std::string &fn);
  void clear();
  void setGeometryShaderSupport(bool geometryShaderSupport);

};
