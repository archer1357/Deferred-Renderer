#ifndef POINTLIGHT_H
#define POINTLIGHT_H

class Pointlight {
public:
  float color[3];
  float attenuation[3];
  float strength;
  bool shadow;
  float viewPos[4];
  float pos[3];
  Pointlight();
};

#endif
