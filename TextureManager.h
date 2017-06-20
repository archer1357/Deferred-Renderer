#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include "gl_core_3_3.h"

#include <string>
#include <map>

#include "FileModified.h"

class TextureManager {
private:
  struct Texture {
    GLuint object;
    bool ok;
    FileModified modified;
  };
  std::map<std::string,Texture*> texture2ds;
  std::map<std::string,Texture*> textureCubes;
public:
  TextureManager();
  ~TextureManager();
  void clear();
private:
  bool load2d(Texture *texture,const std::string &fn);
  bool loadCube(Texture *texture,const std::string &fn);
  void loadPlaceholder2d(Texture *texture);
  void loadPlaceholderCube(Texture *texture);
public:
  GLuint get2d(const std::string &fn);
  GLuint getCube(const std::string &fn);

  bool reload2d(const std::string &fn);
  bool reloadCube(const std::string &fn);
  bool remove2d(const std::string &fn);
  bool removeCube(const std::string &fn);

  void refresh();
};
#endif