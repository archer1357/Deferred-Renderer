#ifndef SPOTLIGHT_H
#define SPOTLIGHT_H


class Spotlight {
public:
  float color[3];
  float attenuation[3];
  float strength;
  float spotExponent;
  float spotCutoff;
  bool shadow;
  float viewPos[4];
  float viewDir[3];
  Spotlight();
};

#endif
