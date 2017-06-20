#ifndef TEXTUREMANAGER
#define TEXTUREMANAGER

#include <string>
#include <map>
#include <GL/glew.h>
#include <iostream>

class TextureManager {
private:
  std::map<std::string,GLuint> texture2ds;
  std::map<std::string,GLuint> textureCubes;
public:
  GLuint get2d(const std::string &fn);
  GLuint getCube(const std::string &fn);
  void onFileModified(const std::string &fn);
};

#endif
