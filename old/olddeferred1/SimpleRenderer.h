#ifndef SIMPLE_RENDERER_H
#define SIMPLE_RENDERER_H

#include "Renderer.h"
#include "Mat4.h"

class SimpleRenderer {
private:
  ProgramManager *programManager;
  TextureManager *textureManager;
  GeometryVaoManager *geometryVaoManager;
  Renderer *renderer;
  float viewMat[16];
  float projMat[16];

  float invProjMat[16];
  float invViewMat[16];
  float viewProjMat[16];

  int width,height,lastWidth,lastHeight;
  float fov,aspect,zNear, zFar;
  float ambience;
  std::string envMapFn;
public:
  SimpleRenderer();
  ~SimpleRenderer();
  void update();

  void setWidth(int width);
  void setHeight(int height);
  void setZNear(float zNear);
  void setZFar(float zFar);
  void setFov(float fov);

  void setViewMat(const float *viewMat);

  void setAmbience(float ambience);
  void setEnvMap(const std::string &envMapFn);

  void onFileAdded(const std::string &fn);
  void onFileModified(const std::string &fn);
  void onFileDeleted(const std::string &fn);

  void geometryBegin();
  void geometryEnd();
  void geometryRender(const float *modelMat, const std::string &vao,
                      const std::string &geometry, const std::string &draw,
                      RendererGeometryMaterial *material);
  void geometryRender(const float *modelMat, const std::string &vao,
                      const std::string &geometry, const std::string &draw,
                      const std::string &programFn);


  void deferredBegin();
  void deferredEnd();

  void shadowVolumeBegin(bool zpass, bool backFace,bool debug);
  void shadowVolumeEnd(bool backFace,bool debug);

  void pointLightShadowVolumeRender(bool geometryShader, const float *lightPos,
                                    const float *modelMat, bool zpass, bool robust,
                                    bool backFace, const std::string &geometry, bool debug);
  void spotLightShadowVolumeRender(bool geometryShader, const float *lightPos,
                                   const float *lightDir, const float *modelMat,
                                   bool zpass, bool robust, bool backFace,
                                   const std::string &geometry, bool debug);
  void dirLightShadowVolumeRender(bool geometryShader, const float *lightDir,
                                  const float *modelMat,bool zpass, bool robust,
                                  bool backFace, const std::string &geometry, bool debug);

  void lightBegin(bool shadowVolume);
  void lightEnd(bool shadowVolume);
  void pointLightRender(const float *pos, const float *attenuation, float strength,
                        const float *color);
  void spotLightRender(const float *pos,const float *dir, const float *attenuation,
                       float strength, float spotExponent, float spotCutoff,
                       const float *color);
  void dirLightRender(const float *dir, float strength, const float *color);

  void normalsRender();
  void colorsRender();
  void linearDepthRender();
  void skyboxRender();
};

#endif
