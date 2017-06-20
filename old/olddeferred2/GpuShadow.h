#ifndef GPUSHADOW_H
#define GPUSHADOW_H

#include "Geometry.h"
#include "Draw.h"
#include <GL/glew.h>

class GpuShadow {
public:
  float modelViewMat[16];
  float modelMat[16];
  GLuint vao;
  GeometryDraw *draw;
  GpuShadow();
};

#endif
