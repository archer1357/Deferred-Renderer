#include "UniformManager.h"
#include <iostream>

void UniformManager::uniform1i(GLint location,GLsizei count,GLboolean transpose,const void*value) {
  glUniform1iv(location,count,(const GLint*)value);
}
void UniformManager::uniform1f(GLint location,GLsizei count,GLboolean transpose,const void*value) {
  glUniform1fv(location,count,(const GLfloat*)value);
}
void UniformManager::uniform2f(GLint location,GLsizei count,GLboolean transpose,const void*value) {
  glUniform2fv(location,count,(const GLfloat*)value);
}
void UniformManager::uniform3f(GLint location,GLsizei count,GLboolean transpose,const void*value) {
  glUniform3fv(location,count,(const GLfloat*)value);
}
void UniformManager::uniform4f(GLint location,GLsizei count,GLboolean transpose,const void*value) {
  glUniform4fv(location,count,(const GLfloat*)value);
}
void UniformManager::uniformMatrix3f(GLint location,GLsizei count,GLboolean transpose,const void*value) {
  glUniformMatrix3fv(location,count,transpose,(const GLfloat*)value);
}
void UniformManager::uniformMatrix4f(GLint location,GLsizei count,GLboolean transpose,const void*value) {
  glUniformMatrix4fv(location,count,transpose,(const GLfloat*)value);
}
bool UniformManager::uniform1iType(GLenum type) {
  return GL_INT==type || GL_BOOL==type;
}
bool UniformManager::uniform1fType(GLenum type) {
  return GL_FLOAT==type;
}
bool UniformManager::uniform2fType(GLenum type) {
  return GL_FLOAT_VEC2==type;
}
bool UniformManager::uniform3fType(GLenum type) {
  return GL_FLOAT_VEC3==type;
}
bool UniformManager::uniform4fType(GLenum type) {
  return GL_FLOAT_VEC4==type;
}
bool UniformManager::uniformMatrix3fType(GLenum type) {
  return GL_FLOAT_MAT3==type;
}
bool UniformManager::uniformMatrix4fType(GLenum type) {
  return GL_FLOAT_MAT4==type;
}
UniformManager::UniformManager() {
}

UniformManager::~UniformManager() {
}
void UniformManager::apply(GLuint p) {
  auto it = locs.find(p);

  if(it == locs.end()) {
    it = locs.insert(std::make_pair(p,std::map<std::string,ProgUniform*>())).first;

    GLint numActiveUniforms = 0;
    glGetProgramiv(p,GL_ACTIVE_UNIFORMS,&numActiveUniforms);

    for(int i = 0; i < numActiveUniforms; i++) {
      GLint size;
      GLenum type;
      char name[256];
      glGetActiveUniform(p,i,256,0,&size,&type,name);

      if(type == GL_INT || type == GL_BOOL || type == GL_FLOAT ||
        type == GL_FLOAT_VEC2 || type == GL_FLOAT_VEC3 || type == GL_FLOAT_VEC4 ||
        type == GL_FLOAT_MAT3 || type == GL_FLOAT_MAT4) {

        ProgUniform *loc = new ProgUniform();
        loc->location = glGetUniformLocation(p,name);
        loc->set = false;
        loc->type=type;
        it->second.insert(std::make_pair(name,loc));
      }
      //std::cout << name << std::endl;
    }
  }

  for(auto i : it->second) {
    if(!i.second->set) {
      std::string valName = i.first;
      auto valIt = vals.find(valName);

      if(valIt != vals.end()) {
        valIt->second->run(i.second->location);

        if(!valIt->second->checkType(i.second->type)) {
          std::cout << "Uniform \"" << valName << "\" wrong type.\n";
        }
      } else {
        std::cout << "Uniform \"" << valName << "\" not set.\n";
      }


      i.second->set = true;
    }
  }
}

void UniformManager::clearProgram(GLuint p) {
  auto it=locs.find(p);

  if(it!=locs.end()) {
    for(auto i : it->second) {
      delete i.second;
    }

    locs.erase(it);
  }
}
void UniformManager::clearPrograms() {
  for(auto i : locs) {
    for(auto j : i.second) {
      delete j.second;
    }
  }

  locs.clear();
}
void UniformManager::clear() {
  clearPrograms();

  for(auto i : vals) {
    delete i.second;
  }
}
void UniformManager::setUniform1iv(const std::string &n,int c,const int *v) {
  auto it = vals.find(n);

  if(it != vals.end()) {
    delete it->second;
    vals.erase(it);
  }

  vals.insert(std::make_pair(n,new Val<GLint,1,uniform1i,uniform1iType>(c,v)));

  //
  for(auto i : locs) {
    auto it2 = i.second.find(n);

    if(it2 != i.second.end()) {
      it2->second->set = false;
    }
  }
}

void UniformManager::setUniform1fv(const std::string &n,int c,const float *v) {
  auto it = vals.find(n);

  if(it != vals.end()) {
    delete it->second;
    vals.erase(it);
  }

  vals.insert(std::make_pair(n,new Val<GLfloat,1,uniform1f,uniform1fType>(c,v)));
  
  //
  for(auto i : locs) {
    auto it2 = i.second.find(n);

    if(it2 != i.second.end()) {
      it2->second->set = false;
    }
  }
}

void UniformManager::setUniform2fv(const std::string &n,int c,const float *v) {
  auto it = vals.find(n);

  if(it != vals.end()) {
    delete it->second;
    vals.erase(it);
  }

  vals.insert(std::make_pair(n,new Val<GLfloat,2,uniform2f,uniform2fType>(c,v)));

  //
  for(auto i : locs) {
    auto it2 = i.second.find(n);

    if(it2 != i.second.end()) {
      it2->second->set = false;
    }
  }
}

void UniformManager::setUniform3fv(const std::string &n,int c,const float *v) {
  auto it = vals.find(n);

  if(it != vals.end()) {
    delete it->second;
    vals.erase(it);
  }

  vals.insert(std::make_pair(n,new Val<GLfloat,3,uniform3f,uniform3fType>(c,v)));

  //
  for(auto i : locs) {
    auto it2 = i.second.find(n);

    if(it2 != i.second.end()) {
      it2->second->set = false;
    }
  }
}

void UniformManager::setUniform4fv(const std::string &n,int c,const float *v) {
  auto it = vals.find(n);

  if(it != vals.end()) {
    delete it->second;
    vals.erase(it);
  }

  vals.insert(std::make_pair(n,new Val<GLfloat,4,uniform4f,uniform4fType>(c,v)));

  //
  for(auto i : locs) {
    auto it2 = i.second.find(n);

    if(it2 != i.second.end()) {
      it2->second->set = false;
    }
  }
}

void UniformManager::setUniformMatrix3fv(const std::string &n,int c,bool tranpose,const float *v) {
  auto it = vals.find(n);

  if(it != vals.end()) {
    delete it->second;
    vals.erase(it);
  }

  vals.insert(std::make_pair(n,new Val<GLfloat,9,uniformMatrix3f,uniformMatrix3fType>(c,tranpose ? GL_TRUE : GL_FALSE,v)));

  //
  for(auto i : locs) {
    auto it2 = i.second.find(n);

    if(it2 != i.second.end()) {
      it2->second->set = false;
    }
  }
}

void UniformManager::setUniformMatrix4fv(const std::string &n,int c,bool tranpose,const float *v) {
  auto it = vals.find(n);

  if(it != vals.end()) {
    delete it->second;
    vals.erase(it);
  }

  vals.insert(std::make_pair(n,new Val<GLfloat,16,uniformMatrix4f,uniformMatrix4fType>(c,tranpose ? GL_TRUE : GL_FALSE,v)));

  //
  for(auto i : locs) {
    auto it2 = i.second.find(n);

    if(it2 != i.second.end()) {
      it2->second->set = false;
    }
  }
}

void UniformManager::setUniform1i(const std::string &n,int v0) {
  setUniform1iv(n,1,&v0);
}

void UniformManager::setUniform1f(const std::string &n,float v0) {
  setUniform1fv(n,1,&v0);
}

void UniformManager::setUniform2f(const std::string &n,float v0,float v1) {
  float v[] ={ v0,v1 };
  setUniform2fv(n,1,v);
}

void UniformManager::setUniform3f(const std::string &n,float v0,float v1,float v2) {
  float v[] ={ v0,v1,v2 };
  setUniform3fv(n,1,v);
}

void UniformManager::setUniform4f(const std::string &n,float v0,float v1,float v2,float v3) {
  float v[] ={ v0,v1,v2,v3 };
  setUniform4fv(n,1,v);
}
