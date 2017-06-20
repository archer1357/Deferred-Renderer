#ifndef RENDERGEOMETRYMATERIAL_H
#define RENDERGEOMETRYMATERIAL_H
#include <string>

class RendererGeometryMaterial {
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
  float bumpBias,bumpScale;
  RendererGeometryMaterial();
};

#endif