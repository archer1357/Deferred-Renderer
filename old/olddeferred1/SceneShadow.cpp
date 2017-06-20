#include "SceneShadow.h"

SceneShadow::SceneShadow(){

  robust=false;
  zpass=false;
  mat4::identity(modelMat);

}
