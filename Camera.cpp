#include "Camera.h"
#include "Math3d.h"


Camera::Camera()
: yaw(0.0f),pitch(0.0f),yawing(0.0f),pitching(0.0f),
leftward(false),rightward(false),forward(false),backward(false)
//,viewMat(1.0f)
{
  mat4::identity(viewMat);
  pos[0]=pos[1]=pos[2]=0.0f;
}
void Camera::update(float deltaTime) {
  float lookSpeed=(float)deltaTime;
  float maxPitch=1.5f;


  yaw-=yawing*lookSpeed;
  pitch-=pitching*lookSpeed;

  if(pitch>maxPitch) {
    pitch=maxPitch;
  }

  if(pitch<-maxPitch) {
    pitch=-maxPitch;
  }

  float rotMat[16];
  mat4::rotateY(rotMat,yaw);
  mat4::rotateX(rotMat,pitch,true);

  float xAxis2[3]={ rotMat[0],rotMat[4],rotMat[8] };
  float yAxis2[3]={ rotMat[1],rotMat[5],rotMat[9] };
  float zAxis2[3]={ rotMat[2],rotMat[6],rotMat[10] };

  for(int i=0;i<3;i++) {
    right[i]=xAxis2[i];
    up[i]=yAxis2[i];
    front[i]=-zAxis2[i];
  }
  float dir2[3]={ 0.0f,0.0f,0.0f };


  //glm::mat4 rotMat(1.0);
  //rotMat=glm::rotate(rotMat,(float)yaw,glm::vec3(0.0f,1.0f,0.0f));
  //rotMat=glm::rotate(rotMat,(float)pitch,glm::vec3(1.0f,0.0f,0.0f));

  //glm::vec3 xAxis(glm::column(rotMat,0));
  //glm::vec3 zAxis(glm::column(rotMat,2));
  //glm::vec3 dir(0.0f);

  if(forward) {
    //dir-=zAxis;

    vec3::sub(dir2,dir2,zAxis2);
  }

  if(backward) {
    //dir+=zAxis;

    vec3::add(dir2,dir2,zAxis2);
  }

  if(leftward) {
    //dir-=xAxis;

    vec3::sub(dir2,dir2,xAxis2);
  }

  if(rightward) {
    //dir+=xAxis;

    vec3::add(dir2,dir2,xAxis2);
  }

  //pos+=dir*(float)deltaTime*10.0f;
  //viewMat=glm::mat4(1.0f);
  //viewMat=glm::translate(viewMat,pos);
  //viewMat*=rotMat;
  //viewMat=glm::inverse(viewMat);


  //
  vec3::multiplyScalar(dir2,dir2,(float)deltaTime*10.0f);
  vec3::add(pos,pos,dir2);

  mat4::translate(viewMat,pos);
  mat4::multiply(viewMat,viewMat,rotMat);
  mat4::inverse(viewMat,viewMat);


  //

  yawing=pitching=0.0f;
  //leftward=rightward=forward=backward=false;
}
void Camera::setPitching(float pitching) {
  this->pitching=pitching;
}
void Camera::setYawing(float yawing) {
  this->yawing=yawing;
}

void Camera::setLeftward(bool leftward) {
  this->leftward=leftward;
}
void Camera::setRightward(bool rightward) {
  this->rightward=rightward;
}
void Camera::setForward(bool forward) {
  this->forward=forward;
}
void Camera::setBackward(bool backward) {
  this->backward=backward;
}
//const glm::mat4 &Camera::getViewMat() {
//  return viewMat;
//}
const float *Camera::getViewMat() {
  return viewMat;
}

void Camera::setYaw(float yaw) {
  this->yaw=yaw;
}
void Camera::setPitch(float pitch) {
  this->pitch=pitch;
}
void Camera::setPosition(float x,float y,float z) {
  pos[0]=x;
  pos[1]=y;
  pos[2]=z;
}
const float *Camera::getPos() {
  return pos;
}
float Camera::getPitch() {
  return pitch;
}
float Camera::getYaw() {
  return yaw;
}
const float *Camera::getRight() {
  return right;
}
const float *Camera::getUp() {
  return up;
}
const float *Camera::getFront() {
  return front;
}