#ifndef CAMERA_H
#define CAMERA_H


class Camera {
private:
  float yaw,pitch;
  float pitching,yawing;
  bool leftward,rightward,forward,backward;

  float pos[3],viewMat[16];
  float right[3],up[3],front[3];
public:
  Camera();
  void update(float deltaTime);
  void setPitching(float pitching);
  void setYawing(float yawing);
  void setLeftward(bool leftward);
  void setRightward(bool rightward);
  void setForward(bool forward);
  void setBackward(bool backward);
  const float *getViewMat();
  void setYaw(float yaw);
  void setPitch(float pitch);
  void setPosition(float x,float y,float z);
  const float *getPos();
  float getPitch();
  float getYaw();
  const float *getRight();
  const float *getUp();
  const float *getFront();
};
#endif