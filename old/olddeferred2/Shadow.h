#ifndef SHADOW_H
#define SHADOW_H

#include "CpuShadow.h"
#include "GpuShadow.h"

class Shadow {
public:
  CpuShadow *cpuShadow;
  GpuShadow *gpuShadow;
  Shadow();
  ~Shadow();
};


#endif
