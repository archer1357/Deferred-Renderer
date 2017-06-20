#ifndef POINTLIGHT_H
#define POINTLIGHT_H

class ScenePointLight {
public:
  float color[3];
  float attenuation[3];
  float strength;
  bool shadow;
  float pos[3];
  ScenePointLight();
};

#endif
