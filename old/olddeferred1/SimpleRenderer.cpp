#include "Mat4.h"
#include "SimpleRenderer.h"


SimpleRenderer::SimpleRenderer()
  : width(128),height(128), lastWidth(0), lastHeight(0),
  fov((float)M_PI_4),aspect(1.0f),zNear(1.0f), zFar(100.0f),
  ambience(0.1f)
{
  programManager=new ProgramManager();
  textureManager=new TextureManager();
  geometryVaoManager=new GeometryVaoManager();
  renderer=new Renderer(programManager,textureManager,geometryVaoManager);
}

SimpleRenderer::~SimpleRenderer() {
  delete renderer;
  delete programManager;
  delete textureManager;
  delete geometryVaoManager;
}
void SimpleRenderer::update() {
  aspect=(float)width/(float)height;

  //projMat
  mat4::perspective(projMat,fov,aspect,zNear,zFar);

  //invProjMat
  mat4::inverse(invProjMat,projMat);

  //invViewMat
  mat4::inverse(invViewMat,viewMat);

  //viewProjMat
  mat4::multiply(viewProjMat,projMat,viewMat);

  //
  if(lastWidth != width || lastHeight !=height) {
    renderer->setupDeferred(width,height);

    lastWidth=width;
    lastHeight=height;
  }
}


void SimpleRenderer::setWidth(int width) {
  this->width=width;
}
void SimpleRenderer::setHeight(int height) {
  this->height=height;
}
void SimpleRenderer::setZNear(float zNear) {
  this->zNear=zNear;
}
void SimpleRenderer::setZFar(float zFar) {
  this->zFar=zFar;
}
void SimpleRenderer::setFov(float fov) {
  this->fov=fov;
}

void SimpleRenderer::setViewMat(const float *viewMat) {
  for(int i=0;i<16;i++) {
    this->viewMat[i]=viewMat[i];
  }
}

void SimpleRenderer::setAmbience(float ambience) {
  this->ambience=ambience;
}

void SimpleRenderer::setEnvMap(const std::string &envMapFn) {
  this->envMapFn=envMapFn;
}

void SimpleRenderer::onFileAdded(const std::string &fn) {
  programManager->onFileAdded(fn);
  textureManager->onFileAdded(fn);
  geometryVaoManager->onFileAdded(fn);
}
void SimpleRenderer::onFileModified(const std::string &fn) {
  programManager->onFileModified(fn);
  textureManager->onFileModified(fn);
  geometryVaoManager->onFileModified(fn);
}
void SimpleRenderer::onFileDeleted(const std::string &fn) {
  programManager->onFileDeleted(fn);
  textureManager->onFileDeleted(fn);
  geometryVaoManager->onFileDeleted(fn);
}

void SimpleRenderer::geometryBegin() {
  glViewport(0,0,width,height);
  renderer->geometryBegin();
}
void SimpleRenderer::geometryEnd() {
  renderer->geometryEnd();
}
void SimpleRenderer::geometryRender(const float *modelMat, const std::string &vao,
                                    const std::string &geometry, const std::string &draw,
                                    RendererGeometryMaterial *material) {
  //modelViewMat
  float modelViewMat[16];
  mat4::multiply(modelViewMat, viewMat,modelMat);

  //normalMat
  float normalMat[16];
  mat4::inverse(normalMat, modelViewMat);
  mat4::transpose(normalMat,normalMat);

  //normalMat3
  float normalMat3[9];
  mat4::toMat3(normalMat3,normalMat);

  //modelViewProjMat
  float modelViewProjMat[16];
  mat4::multiply(modelViewProjMat, projMat,modelViewMat);

  //
  renderer->geometryRender(modelViewMat,normalMat3,modelViewProjMat,zNear,zFar,
                           vao,geometry,draw,material);
}
void SimpleRenderer::geometryRender(const float *modelMat, const std::string &vao,
                                    const std::string &geometry, const std::string &draw,
                                    const std::string &programFn) {
  //modelViewMat
  float modelViewMat[16];
  mat4::multiply(modelViewMat, viewMat,modelMat);

  //normalMat
  float normalMat[16];
  mat4::inverse(normalMat, modelViewMat);
  mat4::transpose(normalMat,normalMat);

  //normalMat3
  float normalMat3[9];
  mat4::toMat3(normalMat3,normalMat);

  //modelViewProjMat
  float modelViewProjMat[16];
  mat4::multiply(modelViewProjMat, projMat,modelViewMat);

  //
  renderer->geometryRender(modelViewMat,normalMat3,modelViewProjMat,zNear,zFar,
                           vao,geometry,draw,programFn);
}
void SimpleRenderer::deferredBegin() {
  renderer->deferredBegin();
  renderer->bindGeometryTextures();

  if(envMapFn.size()) {
    renderer->bindEnvironmentMapTexture(envMapFn);
  }

  renderer->copyDepth();
}
void SimpleRenderer::deferredEnd() {
  renderer->ambientLightRender(ambience);
  renderer->emissiveRender();
  renderer->reflectionsRender(invProjMat,invViewMat);

  renderer->deferredEnd();
}

void SimpleRenderer::shadowVolumeBegin(bool zpass, bool backFace,bool debug) {
  renderer->shadowVolumeBegin(zpass,backFace,debug);
}
void SimpleRenderer::shadowVolumeEnd(bool backFace,bool debug) {
  renderer->shadowVolumeEnd(backFace,debug);
}

void SimpleRenderer::pointLightShadowVolumeRender(bool geometryShader, const float *lightPos, const float *modelMat, bool zpass, bool robust, bool backFace, const std::string &geometry, bool debug) {
  //lightViewPos
  float lightViewPos[4];
  mat4::multiplyVector(lightViewPos,viewMat,lightPos,1.0f);

  //modelViewMat
  float modelViewMat[16];
  mat4::multiply(modelViewMat,viewMat,modelMat);

  //
  if(geometryShader) {
    renderer->pointLightShadowVolumeGpuRender(projMat,lightViewPos,modelViewMat,zpass,robust,backFace,geometry,debug);
  } else {
    //invModelMat
    float invModelMat[16];
    mat4::inverse(invModelMat,modelMat);

    //
    renderer->pointLightShadowVolumeCpuRender(projMat,lightPos,lightViewPos,modelViewMat,invModelMat,zpass,robust,backFace,geometry,debug);
  }
}

void SimpleRenderer::spotLightShadowVolumeRender(bool geometryShader, const float *lightPos, const float *lightDir, const float *modelMat, bool zpass, bool robust, bool backFace, const std::string &geometry, bool debug) {
  //lightViewPos
  float lightViewPos[4];
  mat4::multiplyVector(lightViewPos,viewMat,lightPos,1.0f);

  //lightViewDir
  float lightViewDir[4];
  mat4::multiplyVector(lightViewDir,viewMat,lightDir,0.0f);

  //modelViewMat
  float modelViewMat[16];
  mat4::multiply(modelViewMat,viewMat,modelMat);

  //
  if(geometryShader) {
    renderer->spotLightShadowVolumeGpuRender(projMat,lightViewPos,lightViewDir,modelViewMat,zpass,robust,backFace,geometry,debug);
  } else {
    //invModelMat
    float invModelMat[16];
    mat4::inverse(invModelMat,modelMat);

    //
    renderer->spotLightShadowVolumeCpuRender(projMat,lightPos,lightViewPos,lightDir,lightViewDir,modelViewMat,invModelMat,zpass,robust,backFace,geometry,debug);
  }
}

void SimpleRenderer::dirLightShadowVolumeRender(bool geometryShader, const float *lightDir, const float *modelMat, bool zpass, bool robust, bool backFace, const std::string &geometry, bool debug) {
  //lightViewDir
  float lightViewDir[4];
  mat4::multiplyVector(lightViewDir,viewMat,lightDir,0.0f);

  //modelViewMat
  float modelViewMat[16];
  mat4::multiply(modelViewMat,viewMat,modelMat);

  if(geometryShader) {
    renderer->directionalLightShadowVolumeGpuRender(projMat,lightViewDir,modelViewMat,zpass,robust,backFace,geometry,debug);
  } else {
    //invModelMat
    float invModelMat[16];
    mat4::inverse(invModelMat,modelMat);

    //
    renderer->dirLightShadowVolumeCpuRender(projMat,lightDir,lightViewDir,modelViewMat,invModelMat,zpass,robust,backFace,geometry,debug);
  }
}

void SimpleRenderer::lightBegin(bool shadowVolume) {
  renderer->lightBegin(shadowVolume);
}
void SimpleRenderer::lightEnd(bool shadowVolume) {
  renderer->lightEnd(shadowVolume);
}
void SimpleRenderer::pointLightRender(const float *pos, const float *attenuation, float strength, const float *color) {
  //viewPos
  float viewPos[4];
  mat4::multiplyVector(viewPos,viewMat,pos,1.0f);

  //
  renderer->pointLightRender(invProjMat,viewPos,attenuation,strength,color);
}
void SimpleRenderer::spotLightRender(const float *pos,const float *dir, const float *attenuation, float strength, float spotExponent, float spotCutoff, const float *color) {
  //viewPos
  float viewPos[4];
  mat4::multiplyVector(viewPos,viewMat,pos,1.0f);

  //viewDir
  float viewDir[4];
  mat4::multiplyVector(viewDir,viewMat,dir,0.0f);

  //
  renderer->spotLightRender(invProjMat,viewPos,viewDir,attenuation,strength,spotExponent,spotCutoff,color);
}
void SimpleRenderer::dirLightRender(const float *dir, float strength, const float *color) {
  //viewDir
  float viewDir[4];
  mat4::multiplyVector(viewDir,viewMat,dir,0.0f);

  //
  renderer->dirLightRender(invProjMat,viewDir,strength,color);
}

void SimpleRenderer::normalsRender() {
  renderer->bindGeometryTextures();
  renderer->normalsRender();
}
void SimpleRenderer::colorsRender() {
  renderer->bindGeometryTextures();
  renderer->colorsRender();
}
void SimpleRenderer::linearDepthRender() {
  renderer->bindGeometryTextures();
  renderer->linearDepthRender(zNear,zFar);
}
void SimpleRenderer::skyboxRender() {
  if(envMapFn.size()) {
    renderer->bindEnvironmentMapTexture(envMapFn);
    renderer->skyboxRender(viewProjMat);
  }
}
