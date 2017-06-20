#ifndef SHADOW_H
#define SHADOW_H

#include <GL/glew.h>
#include <string>
#include "Mat4.h"

class SceneShadow {
public:
  SceneShadow();
  bool robust,zpass;
  std::string geometry;
  float modelMat[16];

};


#endif
