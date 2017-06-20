#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H


class SceneDirectionalLight {
public:
  float color[3];
  float strength;
  bool shadow;
  float dir[3];
  SceneDirectionalLight();
};

#endif
