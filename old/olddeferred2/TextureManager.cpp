#include "TextureManager.h"
#define GLM_FORCE_RADIANS
#include <gli/gli.hpp>

GLuint TextureManager::get2d(const std::string &fn) {

  std::map<std::string,GLuint>::iterator it=texture2ds.find(fn);

  if(it != texture2ds.end()) {
    return it->second;
  }

  //
  std::string fn2="data/"+fn;
#if GLI_VERSION==40
  gli::texture2D Texture(gli::loadStorageDDS(fn2));
#else
  gli::texture2D Texture(gli::load_dds(fn2.c_str()));
#endif

  if(Texture.empty()) {

    std::cout << "Error opening 2d " << fn << std::endl;
    return 0;
  }

  GLuint tex;
  glGenTextures(1,&tex);
  glBindTexture(GL_TEXTURE_2D,tex);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  GLenum internalFormat=gli::internal_format(Texture.format());
  GLenum externalFormat=gli::external_format(Texture.format());
  GLenum type=gli::type_format(Texture.format());

  for(gli::texture2D::size_type Level = 0; Level < Texture.levels(); ++Level) {
    GLsizei w=Texture[Level].dimensions().x;
    GLsizei h=Texture[Level].dimensions().y;
    const void *data=Texture[Level].data();

    if(gli::is_compressed(Texture.format())) {
      GLsizei size=Texture[Level].size();
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

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

  texture2ds.insert(std::make_pair(fn,tex));
  return tex;
}

GLuint TextureManager::getCube(const std::string &fn) {
  std::map<std::string,GLuint>::iterator it=textureCubes.find(fn);

  if(it != textureCubes.end()) {
    return it->second;
  }


  std::string fn2="data/"+fn;

#if GLI_VERSION==40
  gli::textureCube Texture(gli::loadStorageDDS(fn2));
#else
  gli::textureCube Texture(gli::load_dds(fn2.c_str()));
#endif

  if(Texture.empty()) {
    std::cout << "Error opening 2d " << fn << std::endl;
    return 0;
  }

  //
  GLuint tex;
  glGenTextures(1,&tex);
  glBindTexture(GL_TEXTURE_CUBE_MAP,tex);

  GLenum internalFormat=gli::internal_format(Texture.format());
  GLenum externalFormat=gli::external_format(Texture.format());
  GLenum type=gli::type_format(Texture.format());

  for(std::size_t Face = 0; Face < 6; ++Face) {
    for(std::size_t Level = 0; Level < Texture.levels(); ++Level) {
      GLenum target=GL_TEXTURE_CUBE_MAP_POSITIVE_X + GLenum(Face);

      GLsizei w=Texture[Face][Level].dimensions().x;
      GLsizei h=Texture[Face][Level].dimensions().y;
      GLsizei size=Texture[Face][Level].size();
      const void *data=Texture[Face][Level].data();

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


  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  textureCubes.insert(std::make_pair(fn,tex));
  return tex;
}

void TextureManager::onFileModified(const std::string &fn) {
  //todo
}
