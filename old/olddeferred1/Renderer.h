#ifndef RENDER_H
#define RENDER_H

#include <GL/glew.h>
#include <iostream>
#include "ProgramManager.h"
#include "GeometryVaoManager.h"
#include "TextureManager.h"

#include "RendererGeometryMaterial.h"



class Renderer {
private:
  GLuint deferredFbo;
  GLuint deferredColorTex;
  GLuint deferredNormalTex;
  GLuint deferredDepthTex;
  bool error;
  ProgramManager *programManager;
  TextureManager *textureManager;
  GeometryVaoManager *geometryVaoManager;
  //int clientWidth,clientHeight,lastClientWidth,lastClientHeight;
  //float projMat[16],invProjMat[16];
 // float viewProjMat[16],invViewMat[16];
  //float zNear,zFar;
 // std::string cubeEnvTexFn;
public:
  Renderer(ProgramManager *programManager,
         TextureManager *textureManager,
         GeometryVaoManager *geometryVaoManager);
  ~Renderer();
  bool checkError();
  bool setupDeferred(int width, int height);

  void geometryBegin();
  void geometryEnd();
  void geometryRender(const float *modelViewMat, const float *normalMat,
                      const float *modelViewProjMat, float zNear, float zFar,
                      const std::string &vao, const std::string &geometry,
                      const std::string &draw, RendererGeometryMaterial *material);
  void geometryRender(const float *modelViewMat, const float *normalMat,
                      const float *modelViewProjMat, float zNear, float zFar,
                      const std::string &vao, const std::string &geometry,
                      const std::string &draw, const std::string &programFn);
  void deferredBegin();
  void deferredEnd();
  void bindGeometryTextures();
  void bindEnvironmentMapTexture(const std::string &fn);
  void copyDepth();
  void shadowVolumeBegin(bool zpass, bool backFace,bool debug);
  void shadowVolumeEnd(bool backFace,bool debug);

  void pointLightShadowVolumeCpuRender(const float *projMat, const float *lightPos, const float *lightViewPos, const float *modelViewMat, const float *invModelMat, bool zpass, bool robust, bool backFace, const std::string &geometry, bool debug);
  void pointLightShadowVolumeGpuRender(const float *projMat, const float *lightViewPos,const float *modelViewMat,bool zpass, bool robust, bool backFace, const std::string &geometry, bool debug);

  void spotLightShadowVolumeCpuRender(const float *projMat, const float *lightPos, const float *lightViewPos, const float *lightDir, const float *lightViewDir, const float *modelViewMat, const float *invModelMat, bool zpass, bool robust, bool backFace, const std::string &geometry, bool debug);
  void spotLightShadowVolumeGpuRender(const float *projMat, const float *lightViewPos,const float *lightViewDir,const float *modelViewMat,bool zpass, bool robust, bool backFace, const std::string &geometry, bool debug);

  void dirLightShadowVolumeCpuRender(const float *projMat, const float *lightDir, const float *lightViewDir, const float *modelViewMat, const float *invModelMat, bool zpass, bool robust, bool backFace, const std::string &geometry, bool debug);
  void directionalLightShadowVolumeGpuRender(const float *projMat, const float *lightViewDir,const float *modelViewMat,bool zpass, bool robust, bool backFace, const std::string &geometry, bool debug);



  void lightBegin(bool shadowVolume);
  void lightEnd(bool shadowVolume);
  void pointLightRender(const float *invProjMat, const float *viewPos, const float *attenuation, float strength, const float *color);
  void spotLightRender(const float *invProjMat, const float *viewPos,const float *viewDir, const float *attenuation, float strength, float spotExponent, float spotCutoff, const float *color);
  void dirLightRender(const float *invProjMat, const float *viewDir, float strength, const float *color);
  void ambientLightRender(float ambience);
  void emissiveRender();
  void reflectionsRender(const float *invProjMat, const float *invViewMat);
  void normalsRender();
  void colorsRender();
  void linearDepthRender(float zNear, float zFar);
  void skyboxRender(const float *viewProjMat);
  void normalVectorsRender(const float *projMat);

};

#endif


/*
some render states are restored after use, but others may not be, like stencil func, op etc. So that some states aren't needlessly restored.
*/
