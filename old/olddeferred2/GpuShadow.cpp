#include "GpuShadow.h"

GpuShadow::GpuShadow() {
  vao=0;

  for(int i=0;i<16;i++) {
    modelViewMat[i]=0.0f;
    modelMat[i]=0.0f;
  }

  modelViewMat[0]=1.0f;
  modelViewMat[5]=1.0f;
  modelViewMat[10]=1.0f;
  modelViewMat[15]=1.0f;

  modelMat[0]=1.0f;
  modelMat[5]=1.0f;
  modelMat[10]=1.0f;
  modelMat[15]=1.0f;


  draw=0;
}
