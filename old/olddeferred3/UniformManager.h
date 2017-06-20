#ifndef UNIFORMMANAGER_HPP
#define UNIFORMMANAGER_HPP

#include <map>
#include <string>
#include "gl_core_3_3.h"

class UniformManager {
private:
  static void uniform1i(GLint location,GLsizei count,GLboolean transpose,const void*value);
  static void uniform1f(GLint location,GLsizei count,GLboolean transpose,const void*value);
  static void uniform2f(GLint location,GLsizei count,GLboolean transpose,const void*value);
  static void uniform3f(GLint location,GLsizei count,GLboolean transpose,const void*value);
  static void uniform4f(GLint location,GLsizei count,GLboolean transpose,const void*value);
  static void uniformMatrix3f(GLint location,GLsizei count,GLboolean transpose,const void*value);
  static void uniformMatrix4f(GLint location,GLsizei count,GLboolean transpose,const void*value);
  static bool uniform1iType(GLenum type);
  static bool uniform1fType(GLenum type);
  static bool uniform2fType(GLenum type);
  static bool uniform3fType(GLenum type);
  static bool uniform4fType(GLenum type);
  static bool uniformMatrix3fType(GLenum type);
  static bool uniformMatrix4fType(GLenum type);

  class ValBase {
  public:
    inline virtual ~ValBase() {}
    virtual void run(GLint location) = 0;
    virtual bool checkType(GLenum type)=0;
  };

  template<typename Q,int n,
            void U(GLint,GLsizei,GLboolean,const void*),
            bool T(GLenum)>
  class Val : public ValBase {
  private:
    Q *value;
    GLsizei count;
    GLboolean transpose;
  public:
    Val(GLsizei count,GLboolean transpose,const Q*value) :count(count),transpose(transpose) {
      this->value = new Q[n*count];

      for(int i = 0; i < n*count; i++) {
        this->value[i] = value[i];
      }
    }
    Val(GLsizei count,const Q*value) :Val(count,GL_FALSE,value) {}
    ~Val() { delete[] value; }
    void run(GLint location) { U(location,count,transpose,value); }
    bool checkType(GLenum type) { return T(type); }
  };

  struct ProgUniform {
    GLint location;
    bool set;
    GLenum type;
  };
public:
  UniformManager();
  ~UniformManager();
  void apply(GLuint p);
  void clearProgram(GLuint p);
  void clearPrograms();
  void clear();
  void setUniform1iv(const std::string &n,int c,const int *v);
  void setUniform1fv(const std::string &n,int c,const float *v);
  void setUniform2fv(const std::string &n,int c,const float *v);
  void setUniform3fv(const std::string &n,int c,const float *v);
  void setUniform4fv(const std::string &n,int c,const float *v);
  void setUniformMatrix3fv(const std::string &n,int c,bool tranpose,const float *v);
  void setUniformMatrix4fv(const std::string &n,int c,bool tranpose,const float *v);
  void setUniform1i(const std::string &n,int v0);
  void setUniform1f(const std::string &n,float v0);
  void setUniform2f(const std::string &n,float v0,float v1);
  void setUniform3f(const std::string &n,float v0,float v1,float v2);
  void setUniform4f(const std::string &n,float v0,float v1,float v2,float v3);
private:
  std::map<GLuint,std::map<std::string,ProgUniform*>> locs;
  std::map<std::string,ValBase*> vals;
};



#endif
