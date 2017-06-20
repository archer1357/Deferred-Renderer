#include "SceneMesh.h"


SceneMesh::SceneMesh() {

  color[0]=color[1]=color[2]=1.0f;

  emissive=false;
  reflective=0.0f;
  shininess=0.1f;

  for(int i=0;i<16;i++) {
    modelMat[i]=0.0f;
  }

  bumpScale=0.045f;
  bumpBias=-0.035f;


  modelMat[0]=1.0f;
  modelMat[5]=1.0f;
  modelMat[10]=1.0f;
  modelMat[15]=1.0f;

}
