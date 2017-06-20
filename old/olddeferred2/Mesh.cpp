#include "Mesh.h"


Mesh::Mesh() {

  color[0]=color[1]=color[2]=1.0f;

  emissive=0.0f;
  reflective=0.0f;
  shininess=0.1f;
  colorTex=0;
  normalTex=0;
  heightTex=0;
  specularTex=0;
  vao=0;
  draw=0;
  material=Mesh::Color;

  for(int i=0;i<16;i++) {
    modelViewProjMat[i]=0.0f;
    modelViewMat[i]=0.0f;
  }
  modelViewProjMat[0]=1.0f;
  modelViewProjMat[5]=1.0f;
  modelViewProjMat[10]=-1.0f;
  modelViewProjMat[15]=1.0f;

  bumpScale=0.045f;
  bumpBias=-0.035f;
  unlit=false;

  normalMat[0]=normalMat[4]=normalMat[8]=1.0f;
  normalMat[1]=normalMat[2]=normalMat[3]=0.0f;
  normalMat[5]=normalMat[6]=normalMat[7]=0.0f;

  modelViewMat[0]=1.0f;
  modelViewMat[5]=1.0f;
  modelViewMat[10]=1.0f;
  modelViewMat[15]=1.0f;
}
