#include "Shadow.h"

Shadow::Shadow() : cpuShadow(0), gpuShadow(0){
}

Shadow::~Shadow() {
  delete cpuShadow;
  delete gpuShadow;
}
