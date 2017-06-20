#ifndef CPUSHADOW_H
#define CPUSHADOW_H

#include "Geometry.h"
#include "Draw.h"
#include <GL/glew.h>
#include "ShadowGeometry.h"
#include "Pointlight.h"
#include "Spotlight.h"
#include <list>

class CpuShadow {
public:
  float modelViewMat[16];
  float modelMat[16];
  GLuint vao;

  GLuint vertBuf;
  int vertsNum;
  ShadowGeometry *shadowGeometry;

  CpuShadow();
  ~CpuShadow();
  void calc(Pointlight *pl, bool debug);
  void calc(Spotlight *sl, bool debug);
  void draw();
};
#endif
