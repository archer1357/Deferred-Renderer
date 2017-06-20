#include "Spotlight.h"

Spotlight::Spotlight() {

  color[0]=color[1]=color[2]=1.0f;
  attenuation[0]=1.0f;
  attenuation[1]=attenuation[2]=0.0f;
  strength=0.1f;

  spotExponent=0.1f;
  spotCutoff=1.1f;

  shadow=false;
  viewPos[0]=viewPos[1]=viewPos[2]=viewPos[3]=0.0f;
  viewDir[0]=viewDir[1]=0.0f;
  viewDir[2]=-1.0f;
}
