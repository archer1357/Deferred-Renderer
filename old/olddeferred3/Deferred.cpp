#include "Deferred.h"
#include <iostream>


Deferred::Deferred()
: screenVao(0),screenBuffer(0),depthCopyProg(0),depthCopyTexLoc(-1)
{
  glGenFramebuffers(1,&fbo);
  glGenTextures(1,&colorTex);
  glGenTextures(1,&normalTex);
  glGenTextures(1,&depthTex);

  //
  initScreenVao();
  initDepthCopyProg();
}

Deferred::~Deferred() {
  glDeleteFramebuffers(1,&fbo);
  glDeleteTextures(1,&colorTex);
  glDeleteTextures(1,&normalTex);
  glDeleteTextures(1,&depthTex);

  //
  glDeleteBuffers(1,&screenBuffer);
  glDeleteVertexArrays(1,&screenVao);
  glDeleteProgram(depthCopyProg);
}

void Deferred::initScreenVao() {
  glGenVertexArrays(1,&screenVao);
  glBindVertexArray(screenVao);

  glGenBuffers(1,&screenBuffer);
  glBindBuffer(GL_ARRAY_BUFFER,screenBuffer);

  const float vertices[]={ -1,-1,1,-1,-1,1,1,1 };
  glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

  glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,0,0);
  glEnableVertexAttribArray(0);
}

bool Deferred::initDepthCopyProg() {
  const char *vsSrc="#version 130\n"
    "attribute vec2 a_pos;\n"
    "varying vec2 v_tex;\n"
    "void main() {\n"
    "  gl_Position = vec4(a_pos,0.0,1.0);\n"
    "  v_tex = a_pos/2.0 + 0.5;\n"
    "}\n";

  const char *fsSrc="#version 130\n"
    "uniform sampler2D u_depthMap;\n"
    "varying vec2 v_tex;\n"
    "void main() {\n"
    "  gl_FragDepth = texture2D(u_depthMap, v_tex).x;\n"
    "}\n";

  //
  GLuint vs=glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vs,1,&vsSrc,0);
  glCompileShader(vs);

  GLint vsStatus;
  glGetShaderiv(vs,GL_COMPILE_STATUS,&vsStatus);

  if(!vsStatus) {
    char log[256];
    glGetShaderInfoLog(vs,256,0,log);
    std::cout << "Deferred::depthCopyProg::vs : shader compile error, " << log << std::endl;
    glDeleteShader(vs);
    return false;
  }

  //
  GLuint fs=glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fs,1,&fsSrc,0);
  glCompileShader(fs);

  GLint fsStatus;
  glGetShaderiv(fs,GL_COMPILE_STATUS,&fsStatus);

  if(!fsStatus) {
    char log[256];
    glGetShaderInfoLog(fs,256,0,log);
    std::cout << "Deferred::depthCopyProg::fs : shader compile error, " << log << std::endl;
    glDeleteShader(vs);
    glDeleteShader(fs);
    return false;
  }

  //
  depthCopyProg=glCreateProgram();
  glBindAttribLocation(depthCopyProg,0,"a_pos");
  glAttachShader(depthCopyProg,vs);
  glAttachShader(depthCopyProg,fs);
  glLinkProgram(depthCopyProg);

  glDeleteShader(vs);
  glDeleteShader(fs);

  GLint progStatus;
  glGetProgramiv(depthCopyProg,GL_LINK_STATUS,&progStatus);

  if(!progStatus) {
    char log[256];
    glGetProgramInfoLog(depthCopyProg,256,0,log);
    glDeleteProgram(depthCopyProg);
    std::cout << "Deferred::depthCopyProg : link error, " << log << std::endl;
    return false;
  }

  depthCopyTexLoc=glGetUniformLocation(depthCopyProg,"u_depthMap");

  return true;
}

bool Deferred::setup(int width,int height,int colorIn,int normalIn) {
  if(width<=0) {
    width=128;
  }

  if(height<=0) {
    height=128;
  }

  //
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER,fbo);

  //color
  glBindTexture(GL_TEXTURE_2D,colorTex);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_FLOAT,0);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0+colorIn,GL_TEXTURE_2D,colorTex,0);
  
  //normal
  GLenum normalInnerFormat;
  GLenum normalType;

  normalInnerFormat=GL_RGBA32F;
  normalType=GL_FLOAT;

  // normalInnerFormat=GL_RGBA16F;
  // normalType=GL_HALF_FLOAT;

  glBindTexture(GL_TEXTURE_2D,normalTex);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D,0,normalInnerFormat,width,height,0,GL_RGBA,normalType,0);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0+normalIn,GL_TEXTURE_2D,normalTex,0);

  //depth
  glBindTexture(GL_TEXTURE_2D,depthTex);
  glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT32F,
    width,height,0,GL_DEPTH_COMPONENT,
    GL_FLOAT,0);

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_FUNC,GL_LEQUAL);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,depthTex,0);

  //
  GLenum drawBufs[]={GL_COLOR_ATTACHMENT0+(GLenum)colorIn,GL_COLOR_ATTACHMENT0+(GLenum)normalIn};
  glDrawBuffers(2,drawBufs);

  //check fbo status
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

  if(status == GL_FRAMEBUFFER_COMPLETE) {
    //restore default FBO
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    return true;
  } else if(status==GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
    std::cout << "fbo error: incomplete attachment\n";
  } else if(status==GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) {
    std::cout << "fbo error: incomplete missing attachment\n";
  } else if(status==GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER) {
    std::cout << "fbo error: incomplete draw buffer\n";
  } else if(status==GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER) {
    std::cout << "fbo error: incomplete read buffer\n";
  } else if(status==GL_FRAMEBUFFER_UNSUPPORTED) {
    std::cout << "fbo error: unsupported\n";
  }

  return false;
}

void Deferred::renderDepth(int depthTexBinding) {
  //states
  glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
  glEnable(GL_DEPTH_TEST);

  //clear
  glClearDepth(1.0f);
  glClear(GL_DEPTH_BUFFER_BIT);
  
  //prog
  glUseProgram(depthCopyProg);
  glUniform1i(depthCopyTexLoc,2);

  //render deferred depth texture
  glBindVertexArray(screenVao);
  glDrawArrays(GL_TRIANGLE_STRIP,0,4);

  //restore states
  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
  glDisable(GL_DEPTH_TEST);
}

GLuint Deferred::getFbo() {
  return fbo;
}
GLuint Deferred::getColorTex() {
  return colorTex;
}
GLuint Deferred::getNormalTex() {
  return normalTex;
}
GLuint Deferred::getDepthTex() {
  return depthTex;
}

