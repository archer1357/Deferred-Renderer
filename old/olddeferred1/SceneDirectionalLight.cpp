#include "SceneDirectionalLight.h"

SceneDirectionalLight::SceneDirectionalLight() {

  color[0]=color[1]=color[2]=1.0f;
  strength=0.1f;

  shadow=false;
  dir[0]=dir[1]=0.0f;
  dir[2]=-1.0f;

}
