#ifndef RENDERER_H
#define RENDERER_H

#include "gl_core_3_3.h"

class Deferred {
private:
  GLuint fbo;
  GLuint colorTex;
  GLuint normalTex;
  GLuint depthTex;
  GLuint screenVao,screenBuffer;
  GLuint depthCopyProg;
  int depthCopyTexLoc;
public:
  Deferred();
  ~Deferred();
private:
  void initScreenVao();
  bool initDepthCopyProg();
public:
  bool setup(int width,int height,int colorIn=0,int normalIn=1);
  void renderDepth(int depthTexBinding);
  GLuint getFbo();
  GLuint getColorTex();
  GLuint getNormalTex();
  GLuint getDepthTex();
};
#endif