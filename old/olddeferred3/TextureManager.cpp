#include "TextureManager.h"
#include <iostream>
#include <fstream>

//#define GLM_FORCE_RADIANS
#include <gli/gli.hpp>
/*
#include <gli/core/gl.hpp>
#include <gli/core/texture.hpp>
#include <gli/core/texture2d.hpp>
#include <gli/core/texture_cube.hpp>
#include <gli/core/load_dds.hpp>
*/
TextureManager::TextureManager() {
}

TextureManager::~TextureManager() {
}

void TextureManager::clear() {
  //
  for(auto i : texture2ds) {
    glDeleteTextures(1,&i.second);
  }

  texture2ds.clear();

  //
  for(auto i : textureCubes) {
    glDeleteTextures(1,&i.second);
  }

  textureCubes.clear();
}

bool TextureManager::load2d(GLuint tex,const std::string &fn) {

  std::ifstream ifs(fn.c_str());

  if(ifs.is_open()) {
    ifs.close();
  } else {
    std::cout << fn << " : Texture2D : could not open.\n";
    return false;
  }

  glBindTexture(GL_TEXTURE_2D,tex);
   

#if GLI_VERSION==40
  gli::texture2D Texture(gli::loadStorageDDS(fn2));
#else
  gli::texture2D Texture(gli::load_dds(fn.c_str()));
#endif

  if(Texture.empty()) {
    std::cout << fn << " : Texture2D : error.\n";
    return false;
  }


  glPixelStorei(GL_UNPACK_ALIGNMENT,1);
  GLenum internalFormat = gli::internal_format(Texture.format());
  GLenum externalFormat = gli::external_format(Texture.format());
  GLenum type = gli::type_format(Texture.format());

  for(gli::texture2D::size_type Level = 0; Level < Texture.levels(); ++Level) {
    GLsizei w = Texture[Level].dimensions().x;
    GLsizei h = Texture[Level].dimensions().y;
    const void *data = Texture[Level].data();

    if(gli::is_compressed(Texture.format())) {
      GLsizei size = Texture[Level].size();
      glCompressedTexImage2D(GL_TEXTURE_2D,GLint(Level),
        internalFormat,w,h,0,size,data);
    } else {
      glTexImage2D(GL_TEXTURE_2D,GLint(Level),internalFormat,
        w,h,0,externalFormat,type,data);
    }
  }

  if(Texture.levels() == 1) {
    glGenerateMipmap(GL_TEXTURE_2D);
  }

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);

  std::cout << fn << " : Texture2D : loaded.\n";
  return true;
}

bool TextureManager::loadCube(GLuint tex,const std::string &fn) {

  std::ifstream ifs(fn.c_str());

  if(ifs.is_open()) {
    ifs.close();
  } else {
    std::cout << fn << " : TextureCube : could not open.\n";
    return false;
  }

  //
  glBindTexture(GL_TEXTURE_CUBE_MAP,tex);

#if GLI_VERSION==40
  gli::textureCube Texture(gli::loadStorageDDS(fn2));
#else
  gli::textureCube Texture(gli::load_dds(fn.c_str()));
#endif

  if(Texture.empty()) {
    std::cout << fn << " : TextureCube : error.\n";
    return false;
  }

  //
  GLenum internalFormat = gli::internal_format(Texture.format());
  GLenum externalFormat = gli::external_format(Texture.format());
  GLenum type = gli::type_format(Texture.format());

  for(std::size_t Face = 0; Face < 6; ++Face) {
    for(std::size_t Level = 0; Level < Texture.levels(); ++Level) {
      GLenum target = GL_TEXTURE_CUBE_MAP_POSITIVE_X + GLenum(Face);

      GLsizei w = Texture[Face][Level].dimensions().x;
      GLsizei h = Texture[Face][Level].dimensions().y;
      GLsizei size = Texture[Face][Level].size();
      const void *data = Texture[Face][Level].data();

      if(gli::is_compressed(Texture.format())) {
        glCompressedTexImage2D(target,GLint(Level),internalFormat,w,h,0,size,data);
      } else {
        glTexImage2D(target,GLint(Level),internalFormat,w,h,0,externalFormat,type,data);
      }
    }
  }

  if(Texture.levels() == 1) {
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);

  glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP,GL_TEXTURE_WRAP_R,GL_CLAMP_TO_EDGE);

  std::cout << fn << " : TextureCube : loaded.\n";
  return true;
}

GLuint TextureManager::get2d(const std::string &fn) {
  if(fn.empty()) {
    return 0;
  }

  std::map<std::string,GLuint>::iterator it=texture2ds.find(fn);

  if(it != texture2ds.end()) {
    return it->second;
  }

  //
  GLuint tex;
  glGenTextures(1,&tex);
  load2d(tex,fn);

  texture2ds.insert(std::make_pair(fn,tex));
  return tex;
}

GLuint TextureManager::getCube(const std::string &fn) {
  if(fn.empty()) {
    return 0;
  }

  std::map<std::string,GLuint>::iterator it=textureCubes.find(fn);

  if(it != textureCubes.end()) {
    return it->second;
  }

  //
  GLuint tex;
  glGenTextures(1,&tex);

  loadCube(tex,fn);

  textureCubes.insert(std::make_pair(fn,tex));
  return tex;
}

bool TextureManager::reload2d(const std::string &fn) {
  auto it = texture2ds.find(fn);

  if(it != texture2ds.end()) {
    load2d(it->second,fn);
    return true;
  }

  return false;
}

bool TextureManager::reloadCube(const std::string &fn) {
  auto it = textureCubes.find(fn);

  if(it != textureCubes.end()) {
    loadCube(it->second,fn);
    return true;
  }

  return false;
}

bool TextureManager::remove2d(const std::string &fn) {
  auto it = texture2ds.find(fn);

  if(it != texture2ds.end()) {
    glDeleteTextures(1,&it->second);
    texture2ds.erase(it);
    return true;
  }

  return false;
}

bool TextureManager::removeCube(const std::string &fn) {
  auto it = textureCubes.find(fn);

  if(it != textureCubes.end()) {
    glDeleteTextures(1,&it->second);
    textureCubes.erase(it);
    return true;
  }

  return false;
}
