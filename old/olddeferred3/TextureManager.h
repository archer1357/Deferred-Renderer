#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include "gl_core_3_3.h"

#include <string>
#include <map>

class TextureManager {
private:
  std::map<std::string,GLuint> texture2ds;
  std::map<std::string,GLuint> textureCubes;
public:
  TextureManager();
  ~TextureManager();
  void clear();
private:
  bool load2d(GLuint tex,const std::string &fn);
  bool loadCube(GLuint tex,const std::string &fn);
public:
  GLuint get2d(const std::string &fn);
  GLuint getCube(const std::string &fn);

  bool reload2d(const std::string &fn);
  bool reloadCube(const std::string &fn);
  bool remove2d(const std::string &fn);
  bool removeCube(const std::string &fn);
};
#endif