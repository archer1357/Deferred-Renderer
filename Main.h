#include <cstring>
#include "Math3d.h"

#include "gl_core_3_3.h"
#include "Window.h"



#include <iostream>
#include <algorithm>

#include "TextureManager.h"
#include "ShaderManager.h"
#include "GeometryManager.h"
#include "UniformManager.h"


#include "MouseSmooth.h"
#include "Camera.h"
#include "Keyframer.h"

#include <sstream>

#include "Timer.h"


#include "ScreenFont.h"


#include <string.h>
#include <stdlib.h>


class Object {
public:
  bool visible;
  float modelMat[16];
  float modelViewMat[16];
  float modelViewProjMat[16];
  float normalMat[9];

  GLuint vao;
  GLuint shadowVao;

  GeometryDraw draw,shadowDraw;

  float color[3];
  float shininess;
  float emissive;
  bool lit;
  float shadowSmoothBias;
  Object() {
    visible=true;
    mat4::identity(modelMat);
    vao=0;
    shadowVao=0;
    color[0]=color[1]=color[2]=1.0f;
    shininess=0.1f;
    emissive=0.0f;
    lit=true;
    shadowSmoothBias=0.0f;
  }
};

class PointLight;
class SpotLight;
class DirLight;

class Light {
public:
  bool on;
  bool shadow;
  float color[3];
  float strength;
  Light() {
    shadow=false;
    color[0]=color[1]=color[2]=1.0f;
    on=true;
    strength=0.1f;
  }
  virtual ~Light() {}
};

class PointLight : public Light  {
public:
  float pos[3];
  float atten[3];
  float viewPos[4];
  PointLight() {
    pos[0]=pos[1]=pos[2]=0.0f;
    atten[0]=1.0f;
    atten[1]=atten[2]=0.0f;
    viewPos[0]=viewPos[1]=viewPos[2]=0.0f;
    viewPos[3]=1.0f;
  }
};

class SpotLight : public Light  {
public:
  float pos[3];
  float dir[3];
  float atten[3];
  float exponent;
  float cutoff;
  float viewPos[4];
  float viewDir[4];
  SpotLight() {
    pos[0]=pos[1]=pos[2]=0.0f;
    dir[0]=dir[2]=0.0f;
    dir[1]=-1.0f;
    atten[0]=1.0f;
    atten[1]=atten[2]=0.0f;
    exponent=0.0f;
    cutoff=1.0f;
    viewPos[0]=viewPos[1]=viewPos[2]=0.0f;
    viewPos[3]=1.0f;
    viewDir[0]=viewDir[2]=viewDir[3]=0.0f;
    viewDir[1]=-1.0f;
  }
};

class DirLight : public Light {
public:
  float dir[3];
  float viewDir[4];
  DirLight() {
    dir[0]=dir[2]=0.0f;
    dir[1]=-1.0f;
    viewDir[0]=viewDir[2]=viewDir[3]=0.0f;
    viewDir[1]=-1.0f;
  }
};

std::list<Object*> objects;
std::list<Light*> lights;

bool geometryShaderSupport=false;

ShaderManager shaderManager;
TextureManager textureManager;
GeometryManager geometryManager;
UniformManager uniformManager;

GLuint screenVao=0,skyVao=0;
GeometryDraw screenDraw,skyDraw;


bool mouseLook=false;
Camera camera;

GLuint deferredFbo;
GLuint deferredColorTex;
GLuint deferredNormalTex;
GLuint deferredDepthTex;


//GLuint shadowBlurDepthStencilBuf;
GLuint shadowBlurColorTex;
//GLuint shadowBlurFboTex;

GLuint lightFbo, lightDepthStencilBuf, lightColorTex;

bool useShadowSmooth=false;
bool useBackfaceShadow=true;
bool useShadowDebug=false;
bool useShadowPolygonOffset=false;
bool useShadowZPass=false;
bool useShadowDepthLessEqual=true;
bool useSsao=true;
bool attachPointLight=false;
int useAntiAliasing=1;//0=none,1=fxaa_v1,2=fxaa_v2
bool useShadowWedge=false;

float angle=0.0f;
double frame=0.0;
double deltaTime=0.0;
double curTime=0.0;
Keyframer<float,3> colKfr;
Keyframer<float,3> lightPosKfr;


// MyFont *myFont=0;


int renderWidth=0, renderHeight=0;

float zNear = 1.0f;
float zFar = 200.0f;

float projMat[16];
float invProjMat[16];
float viewMat[16];
float invViewMat[16];
float viewProjMat[16];
float proj2dMat[16];


Object *headObj=new Object();
PointLight *light1=new PointLight();
DirLight *light2=new DirLight();
Object *buildingObj=new Object();
Object *templeObj=new Object();
Object *cubeObj=new Object();

int fpsCount=0,fpsTotal=0;
double fpsTimeCount=0.0;






#define  MOUSE_HISTORY_SIZE 10

double mouseSmoothX;
double mouseSmoothY;
double mouseHistory[MOUSE_HISTORY_SIZE*2];

#define FONT_MAX_CHARS 1024
struct ScreenFont *screenFont=0;
