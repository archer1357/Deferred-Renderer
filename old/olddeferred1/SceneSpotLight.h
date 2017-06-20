#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H


class SceneSpotLight {
public:
  float color[3];
  float attenuation[3];
  float strength;
  float spotExponent;
  float spotCutoff;
  bool shadow;
  float pos[3],dir[3];
  SceneSpotLight();
};

#endif
