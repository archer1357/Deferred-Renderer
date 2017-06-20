#include "RendererGeometryMaterial.h"

RendererGeometryMaterial::RendererGeometryMaterial() {
  
  color[0]=color[1]=color[2]=1.0f;

  emissive=false;
  reflective=0.0f;
  shininess=0.1f;


  bumpScale=0.045f;
  bumpBias=-0.035f;

}