#ifndef MESH_H
#define MESH_H

#include "Geometry.h"
#include "Draw.h"
#include <GL/glew.h>
#include <string>

class SceneMesh {
public:
  float color[3];
  bool emissive;
  float reflective;
  float shininess;
  std::string colorTex;
  std::string normalTex;
  std::string heightTex;
  std::string specularTex;
  std::string reliefTex;
  std::string geometry;
  std::string draw;
  std::string vao;
  float modelMat[16];
  float bumpBias,bumpScale;
  SceneMesh();
};
#endif