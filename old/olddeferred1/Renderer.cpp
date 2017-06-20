#include "Renderer.h"


int shadowCpuThreads=8;






Renderer::Renderer(ProgramManager *programManager,
               TextureManager *textureManager,
               GeometryVaoManager *geometryVaoManager)
               : programManager(programManager),
               textureManager(textureManager),
               geometryVaoManager(geometryVaoManager),
               error(false) //,
              // clientWidth(1),clientHeight(1),lastClientWidth(0),lastClientHeight(0)
{
  glGenFramebuffers(1,&deferredFbo);
  glGenTextures(1,&deferredColorTex);
  glGenTextures(1,&deferredNormalTex);
  glGenTextures(1,&deferredDepthTex);

  //cubeEnvTexFn=;
}
Renderer::~Renderer() {
  glDeleteFramebuffers(1,&deferredFbo);
  glDeleteTextures(1,&deferredColorTex);
  glDeleteTextures(1,&deferredNormalTex);
  glDeleteTextures(1,&deferredDepthTex);
}
bool Renderer::checkError() {
  static const char *errors[]={
    "invalid enum","invalid value","invalid operation",
    "invalid frame buffer operation","out of memory"};

  static const GLenum codes[]={
    GL_INVALID_ENUM,GL_INVALID_VALUE,GL_INVALID_OPERATION,
    GL_INVALID_FRAMEBUFFER_OPERATION,GL_OUT_OF_MEMORY};

  int errCode = glGetError();

  if(errCode != GL_NO_ERROR) {


    for(int i=0;i<5;i++) {
      if(codes[i]==errCode) {
        error=true;
        std::cout << "gl error : " << errors[i] << std::endl;
        return true;
      }
    }
  }

  return false;
}
bool Renderer::setupDeferred(int width, int height) {
  int clientWidth=width;
  int clientHeight=height;
  //
  if(clientWidth<=0) {
    clientWidth=1;
  }

  if(clientHeight<=0) {
    clientHeight=1;
  }

  //
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, deferredFbo);

  //color
  glBindTexture(GL_TEXTURE_2D, deferredColorTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,clientWidth,clientHeight,0,GL_RGBA,GL_FLOAT,0);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,
    GL_TEXTURE_2D,deferredColorTex,0);

  //normal
  GLenum normalInnerFormat;
  GLenum normalType;

  normalInnerFormat=GL_RGBA32F;
  normalType=GL_FLOAT;

  // normalInnerFormat=GL_RGBA16F;
  // normalType=GL_HALF_FLOAT;

  glBindTexture(GL_TEXTURE_2D, deferredNormalTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D,0,normalInnerFormat,clientWidth,clientHeight,0,GL_RGBA,normalType,0);

  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT1,
    GL_TEXTURE_2D,deferredNormalTex,0);

  //depth
  glBindTexture(GL_TEXTURE_2D, deferredDepthTex);
  glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT32F,
    clientWidth,clientHeight,0,GL_DEPTH_COMPONENT,
    GL_FLOAT,0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);

  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,
    GL_TEXTURE_2D,deferredDepthTex,0);

  //
  GLenum drawBufs[]={GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1};
  glDrawBuffers(2,drawBufs);

  //check fbo status
  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

  if(status == GL_FRAMEBUFFER_COMPLETE) {
    //restore default FBO
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
  } else if(status==GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
    std::cout << "fbo error: incomplete attachment\n";
  } else if(status==GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) {
    std::cout << "fbo error: incomplete missing attachment\n";
  } else if(status==GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER) {
    std::cout << "fbo error: incomplete draw buffer\n";
  } else if(status==GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER) {
    std::cout << "fbo error: incomplete read buffer\n";
  } else if(status==GL_FRAMEBUFFER_UNSUPPORTED) {
    std::cout << "fbo error: unsupported\n";
  }

  return false;
}

void Renderer::geometryBegin() {
  //states
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  //default states
  // glCullFace(GL_BACK);
  // glFrontFace(GL_CCW);
  // glDisable(GL_BLEND);
  // glDisable(GL_STENCIL_TEST);
  // glDepthMask(GL_TRUE);
  // glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

  //bind fbo
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, deferredFbo);

  //clear
  glClearColor(0.0f,0.0f,0.0f,0.0f);
  glClearDepth(1);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}
void Renderer::geometryEnd() {
  //restore states
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  //bind fbo
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void Renderer::geometryRender(const float *modelViewMat, const float *normalMat,
                              const float *modelViewProjMat, float zNear, float zFar,
                              const std::string &vao, const std::string &geometry,
                              const std::string &draw, RendererGeometryMaterial *material) {
  GLuint prog=0;
  if(material->colorTex.size() && material->normalTex.size() && material->heightTex.size()) {
    prog=programManager->get("data/shader/geometry/Parallax2.json");
  } else if(material->colorTex.size() && material->reliefTex.size()) {
    prog=programManager->get("data/shader/geometry/Parallax.json");
  } else {
    prog=programManager->get("data/shader/geometry/Colored.json");
  }


  if(GLuint tex=textureManager->get2d(material->colorTex)) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,tex);
  }

  if(GLuint tex=textureManager->get2d(material->normalTex)) {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,tex);
  }

  if(GLuint tex=textureManager->get2d(material->heightTex)) {
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D,tex);
  }

  if(GLuint tex=textureManager->get2d(material->reliefTex)) {
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D,tex);
  }

  //
  glUseProgram(prog);

  int modelViewProjMatLoc=glGetUniformLocation(prog,"u_modelViewProjMat");
  int modelViewMatLoc=glGetUniformLocation(prog,"u_modelViewMat");
  int normalMatLoc=glGetUniformLocation(prog,"u_normalMat");

  int colLoc=glGetUniformLocation(prog,"u_col");
  int shininessLoc=glGetUniformLocation(prog,"u_shininess");
  int emissiveLoc=glGetUniformLocation(prog,"u_emissive");
  int reflectiveLoc=glGetUniformLocation(prog,"u_reflective");

  int parallaxScaleLoc=glGetUniformLocation(prog,"u_parallaxScale");
  int parallaxBiasLoc=glGetUniformLocation(prog,"u_parallaxBias");

  int zNearFarLoc=glGetUniformLocation(prog,"u_zNearFar");

  //
  if(colLoc!=-1) {
    glUniform3fv(colLoc,1,material->color);
  }

  if(modelViewProjMatLoc!=-1) {
    glUniformMatrix4fv(modelViewProjMatLoc,1,GL_TRUE,modelViewProjMat);
  }

  if(modelViewMatLoc!=-1) {
    glUniformMatrix4fv(modelViewMatLoc,1,GL_TRUE,modelViewMat);
  }

  if(normalMatLoc!=-1) {
    glUniformMatrix3fv(normalMatLoc,1,GL_TRUE,normalMat);
  }

  if(shininessLoc!=-1) {
    glUniform1fv(shininessLoc,1,&material->shininess);
  }

  if(emissiveLoc!=-1) {
    glUniform1i(emissiveLoc,material->emissive?1:0);
  }

  if(reflectiveLoc!=-1) {
    glUniform1fv(reflectiveLoc,1,&material->reflective);
  }


  //
  if(parallaxScaleLoc!=-1) {
    glUniform1fv(parallaxScaleLoc,1,&material->bumpScale);
  }

  if(parallaxBiasLoc!=-1) {
    glUniform1fv(parallaxBiasLoc,1,&material->bumpBias);
  }

  if(zNearFarLoc!=-1) {
    float zNearFar[2]={zNear,zFar};
    glUniform2fv(zNearFarLoc,1,zNearFar);
  }

  //
  if(GeometryVao *gv=geometryVaoManager->get(geometry,vao)) {
    glBindVertexArray(gv->getVao());

    if(GeometryDraw *d=gv->getDraw(draw)) {
      d->draw();
    }
  }
}
void Renderer::geometryRender(const float *modelViewMat, const float *normalMat,
                              const float *modelViewProjMat, float zNear, float zFar,
                              const std::string &vao, const std::string &geometry,
                              const std::string &draw, const std::string &programFn) {
  GLuint prog=programManager->get(programFn);


  int modelViewProjMatLoc=glGetUniformLocation(prog,"u_modelViewProjMat");
  int modelViewMatLoc=glGetUniformLocation(prog,"u_modelViewMat");
  int normalMatLoc=glGetUniformLocation(prog,"u_normalMat");
  int zNearFarLoc=glGetUniformLocation(prog,"u_zNearFar");


  if(modelViewProjMatLoc!=-1) {
    glUniformMatrix4fv(modelViewProjMatLoc,1,GL_TRUE,modelViewProjMat);
  }

  if(modelViewMatLoc!=-1) {
    glUniformMatrix4fv(modelViewMatLoc,1,GL_TRUE,modelViewMat);
  }

  if(normalMatLoc!=-1) {
    glUniformMatrix3fv(normalMatLoc,1,GL_TRUE,normalMat);
  }


  if(zNearFarLoc!=-1) {
    float zNearFar[2]={zNear,zFar};
    glUniform2fv(zNearFarLoc,1,zNearFar);
  }

  //
  if(GeometryVao *gv=geometryVaoManager->get(geometry,vao)) {
    glBindVertexArray(gv->getVao());

    if(GeometryDraw *d=gv->getDraw(draw)) {
      d->draw();
    }
  }
}
void Renderer::deferredBegin() {
  glClearColor(0.0f,0.0f,0.0f,1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}
void Renderer::deferredEnd() {

}
void Renderer::bindGeometryTextures() {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, deferredColorTex);
  glBindSampler(0,0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, deferredNormalTex);
  glBindSampler(1,0);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, deferredDepthTex);
  glBindSampler(2,0);
}
void Renderer::bindEnvironmentMapTexture(const std::string &fn) {
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_CUBE_MAP,textureManager->getCube(fn));
}
void Renderer::copyDepth() {
  //temp
  GLuint screenVao=geometryVaoManager->get("data/geometry/Screen.json","data/geometry/Position.json")->getVao();
  GeometryDraw *screenDraw=geometryVaoManager->get("data/geometry/Screen.json","data/geometry/Position.json")->getDraw();

  //states
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glEnable(GL_DEPTH_TEST);

  //default states
  // glDepthFunc(GL_LESS);

  //clear
  glClearDepth(1.0f);
  glClear(GL_DEPTH_BUFFER_BIT);

  //render deferred depth texture
  glUseProgram(programManager->get("data/shader/deferred/DepthCopy.json"));

  glBindVertexArray(screenVao);
  screenDraw->draw();

  //restore states
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glDisable(GL_DEPTH_TEST);
}
void Renderer::shadowVolumeBegin(bool zpass,bool backFace,bool debug) {
  //states
  if(!debug) {

     glEnable(GL_STENCIL_TEST);
  glDepthMask(GL_FALSE);
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);


  }

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_DEPTH_CLAMP);

  if(!debug && !backFace) {
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(0.1f,1.0f);
  }

    if(!debug && zpass) {
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_INCR_WRAP);
    glStencilOpSeparate(GL_BACK , GL_KEEP, GL_KEEP, GL_DECR_WRAP);
  } else {
    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
    glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
  }

  //default states
  // glDisable(GL_CULL_FACE);
  // glDisable(GL_BLEND);
  // glStencilFunc(GL_ALWAYS, 0, 0xff);

  //
  if(!debug) {
  glClearStencil(0);
  glClear(GL_STENCIL_BUFFER_BIT);
  }

}
void Renderer::shadowVolumeEnd(bool backFace,bool debug) {
  //restore states
    if(!debug) {
  //glDisable(GL_STENCIL_TEST);


  glDepthMask(GL_TRUE);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_DEPTH_CLAMP);

  if(!debug && !backFace) {
  glDisable(GL_POLYGON_OFFSET_FILL);
  }
  //

}


void Renderer::pointLightShadowVolumeCpuRender(const float *projMat, const float *lightPos, const float *lightViewPos, const float *modelViewMat, const float *invModelMat, bool zpass, bool robust, bool backFace, const std::string &geometry, bool debug) {


  GLuint shadowProg=programManager->get("data/shader/PointLightShadowCpu.json");


  GLint projMatLoc2=glGetUniformLocation(shadowProg,"u_projMat");
  GLint modelViewMatLoc2=glGetUniformLocation(shadowProg,"u_modelViewMat");
  GLint lightPosLoc2=glGetUniformLocation(shadowProg,"u_lightPos");

  glUseProgram(shadowProg);
  glUniformMatrix4fv(projMatLoc2,1,GL_TRUE,projMat);

  glUniform4fv(lightPosLoc2,1,lightViewPos);


  glUniformMatrix4fv(modelViewMatLoc2,1,GL_TRUE,modelViewMat);

  ShadowVolumeCpuGeometry *geom=geometryVaoManager->getShadowVolumeCpuGeometry(geometry);
  geom->pointLightRender(lightPos,invModelMat,8,zpass,robust,backFace,debug);

}
void Renderer::pointLightShadowVolumeGpuRender(const float *projMat, const float *lightViewPos,const float *modelViewMat,bool zpass, bool robust, bool backFace, const std::string &geometry, bool debug) {

  GLuint shadowProg=0;
  if(debug) {
    shadowProg=programManager->get("data/shader/PointLightShadowGpuDebug.json");
  } else {
  shadowProg=programManager->get("data/shader/PointLightShadowGpu.json");
  }
  GLint zpassLoc2=glGetUniformLocation(shadowProg,"u_zpass");
  GLint robustLoc2=glGetUniformLocation(shadowProg,"u_robust");


  GLint projMatLoc2=glGetUniformLocation(shadowProg,"u_projMat");
  GLint modelViewMatLoc2=glGetUniformLocation(shadowProg,"u_modelViewMat");
  GLint lightPosLoc2=glGetUniformLocation(shadowProg,"u_lightPos");

  glUseProgram(shadowProg);
  glUniformMatrix4fv(projMatLoc2,1,GL_TRUE,projMat);

  glUniform4fv(lightPosLoc2,1,lightViewPos);

  glUniform1i(zpassLoc2,zpass?1:0);
  glUniform1i(robustLoc2,robust?1:0);


  glUniformMatrix4fv(modelViewMatLoc2,1,GL_TRUE,modelViewMat);

  if(GeometryVao *gv=geometryVaoManager->get(geometry,"data/geometry/Position.json")) {
    glBindVertexArray(gv->getVao());
    gv->getDraw("default")->draw();
  }

}


void Renderer::spotLightShadowVolumeCpuRender(const float *projMat, const float *lightPos, const float *lightViewPos, const float *lightDir, const float *lightViewDir, const float *modelViewMat, const float *invModelMat, bool zpass, bool robust, bool backFace, const std::string &geometry, bool debug) {


  GLuint shadowProg=programManager->get("data/shader/PointLightShadowCpu.json");


  GLint projMatLoc2=glGetUniformLocation(shadowProg,"u_projMat");
  GLint modelViewMatLoc2=glGetUniformLocation(shadowProg,"u_modelViewMat");
  GLint lightPosLoc2=glGetUniformLocation(shadowProg,"u_lightPos");

  glUseProgram(shadowProg);
  glUniformMatrix4fv(projMatLoc2,1,GL_TRUE,projMat);

  glUniform4fv(lightPosLoc2,1,lightViewPos);


  glUniformMatrix4fv(modelViewMatLoc2,1,GL_TRUE,modelViewMat);

  ShadowVolumeCpuGeometry *geom=geometryVaoManager->getShadowVolumeCpuGeometry(geometry);
  geom->spotLightRender(lightPos,lightDir,invModelMat,8,zpass,robust,backFace,debug);
}
void Renderer::spotLightShadowVolumeGpuRender(const float *projMat, const float *lightViewPos,const float *lightViewDir,const float *modelViewMat,bool zpass, bool robust, bool backFace, const std::string &geometry, bool debug) {
  pointLightShadowVolumeGpuRender(projMat,lightViewPos,modelViewMat,zpass, robust,backFace,geometry,debug);
}

void Renderer::dirLightShadowVolumeCpuRender(const float *projMat, const float *lightDir, const float *lightViewDir, const float *modelViewMat, const float *invModelMat, bool zpass, bool robust, bool backFace, const std::string &geometry, bool debug) {
  GLuint shadowProg=0;

  shadowProg=programManager->get("data/shader/DirectionalLightShadowCpu.json");


  GLint projMatLoc2=glGetUniformLocation(shadowProg,"u_projMat");
  GLint modelViewMatLoc2=glGetUniformLocation(shadowProg,"u_modelViewMat");
  GLint lightDirLoc2=glGetUniformLocation(shadowProg,"u_lightDir");

  glUseProgram(shadowProg);
  glUniformMatrix4fv(projMatLoc2,1,GL_TRUE,projMat);

  glUniform4fv(lightDirLoc2,1,lightViewDir);


  glUniformMatrix4fv(modelViewMatLoc2,1,GL_TRUE,modelViewMat);

  ShadowVolumeCpuGeometry *geom=geometryVaoManager->getShadowVolumeCpuGeometry(geometry);
  geom->directionalLightRender(lightDir, invModelMat,  8,  zpass,   robust,   backFace, debug);


}
void Renderer::directionalLightShadowVolumeGpuRender(const float *projMat, const float *lightViewDir,const float *modelViewMat,bool zpass, bool robust, bool backFace, const std::string &geometry, bool debug) {
  GLuint shadowProg=0;
  GLint zpassLoc2=-1;
  GLint robustLoc2=-1;

  if(debug) {
  shadowProg=programManager->get("data/shader/DirectionalLightShadowGpuDebug.json");
  } else {
  shadowProg=programManager->get("data/shader/DirectionalLightShadowGpu.json");
  }

  zpassLoc2=glGetUniformLocation(shadowProg,"u_zpass");
  robustLoc2=glGetUniformLocation(shadowProg,"u_robust");


  GLint projMatLoc2=glGetUniformLocation(shadowProg,"u_projMat");
  GLint modelViewMatLoc2=glGetUniformLocation(shadowProg,"u_modelViewMat");
  GLint lightDirLoc2=glGetUniformLocation(shadowProg,"u_lightDir");

  glUseProgram(shadowProg);
  glUniformMatrix4fv(projMatLoc2,1,GL_TRUE,projMat);

  glUniform4fv(lightDirLoc2,1,lightViewDir);

  glUniform1i(zpassLoc2,zpass?1:0);
  glUniform1i(robustLoc2, robust?1:0);


  glUniformMatrix4fv(modelViewMatLoc2,1,GL_TRUE,modelViewMat);

  if(GeometryVao *gv=geometryVaoManager->get(geometry,"data/geometry/Position.json")) {
    glBindVertexArray(gv->getVao());
    gv->getDraw("default")->draw();
  }

}

void Renderer::lightBegin(bool shadowVolume) {
  //states
  glDepthMask(GL_FALSE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  glEnable(GL_CULL_FACE);

  if(shadowVolume) {
    //glEnable(GL_STENCIL_TEST);
    glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
    glStencilFunc(GL_EQUAL, 0, 0xff);
  }

  //default states
  // glDisable(GL_DEPTH_TEST);
  // glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
  // glCullFace(GL_BACK);
  // glFrontFace(GL_CCW);
}
void Renderer::lightEnd(bool shadowVolume) {

  //restore states
  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ZERO);
  glDisable(GL_CULL_FACE);

  if(shadowVolume) {
    glDisable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 0, 0xff);
  }

}
void Renderer::pointLightRender(const float *invProjMat, const float *viewPos, const float *attenuation, float strength, const float *color) {
  //temp
  GLuint screenVao=geometryVaoManager->get("data/geometry/Screen.json","data/geometry/Position.json")->getVao();
  GeometryDraw *screenDraw=geometryVaoManager->get("data/geometry/Screen.json","data/geometry/Position.json")->getDraw();

  //
  GLuint lightProg=programManager->get("data/shader/deferred/PointLight.json");

  GLuint invProjLoc=glGetUniformLocation(lightProg,"u_invProjMat");
  GLint lightPosLoc=glGetUniformLocation(lightProg,"u_lightPos");
  GLint lightAttenLoc=glGetUniformLocation(lightProg,"u_lightAtten");
  GLint lightColLoc=glGetUniformLocation(lightProg,"u_lightCol");
  GLint strengthLoc=glGetUniformLocation(lightProg,"u_strength");

  glUseProgram(lightProg);
  glUniformMatrix4fv(invProjLoc,1,GL_TRUE,invProjMat);

  glUniform4fv(lightPosLoc,1,viewPos);
  glUniform3fv(lightAttenLoc,1,attenuation);
  glUniform3fv(lightColLoc,1,color);
  glUniform1fv(strengthLoc,1,&strength);

  glBindVertexArray(screenVao);
  screenDraw->draw();

}

void Renderer::spotLightRender(const float *invProjMat, const float *viewPos,const float *viewDir, const float *attenuation, float strength, float spotExponent, float spotCutoff, const float *color) {
  //temp
  GLuint screenVao=geometryVaoManager->get("data/geometry/Screen.json","data/geometry/Position.json")->getVao();
  GeometryDraw *screenDraw=geometryVaoManager->get("data/geometry/Screen.json","data/geometry/Position.json")->getDraw();


  GLuint p=programManager->get("data/shader/deferred/SpotLight.json");


  GLuint invProjLoc=glGetUniformLocation(p,"u_invProjMat");
  GLint lightPosLoc=glGetUniformLocation(p,"u_lightPos");
  GLint lightAttenLoc=glGetUniformLocation(p,"u_lightAtten");
  GLint lightColLoc=glGetUniformLocation(p,"u_lightCol");
  GLint strengthLoc=glGetUniformLocation(p,"u_strength");
  GLint lightDirLoc=glGetUniformLocation(p,"u_lightDir");
  GLint spotExponentLoc=glGetUniformLocation(p,"u_spotExponent");
  GLint spotCutoffLoc=glGetUniformLocation(p,"u_spotCutoff");

  glUseProgram(p);

  glUniformMatrix4fv(invProjLoc,1,GL_TRUE,invProjMat);
  glUniform4fv(lightPosLoc,1,viewPos);
  glUniform4fv(lightDirLoc,1,viewDir);
  glUniform3fv(lightAttenLoc,1,attenuation);
  glUniform3fv(lightColLoc,1,color);
  glUniform1fv(strengthLoc,1,&strength);
  glUniform1fv(spotExponentLoc,1,&spotExponent);
  glUniform1fv(spotCutoffLoc,1,&spotCutoff);

  glBindVertexArray(screenVao);
  screenDraw->draw();

}
void Renderer::dirLightRender(const float *invProjMat, const float *viewDir, float strength, const float *color) {
  //temp
  GLuint screenVao=geometryVaoManager->get("data/geometry/Screen.json","data/geometry/Position.json")->getVao();
  GeometryDraw *screenDraw=geometryVaoManager->get("data/geometry/Screen.json","data/geometry/Position.json")->getDraw();

  //
  GLuint p=programManager->get("data/shader/deferred/DirectionalLight.json");
  glUseProgram(p);

  GLint invProjLoc=glGetUniformLocation(p,"u_invProjMat");
  glUniformMatrix4fv(invProjLoc,1,GL_TRUE,invProjMat);


  GLint lightDirLoc=glGetUniformLocation(p,"u_lightDir");
  GLint lightColLoc=glGetUniformLocation(p,"u_lightCol");
  GLint strengthLoc=glGetUniformLocation(p,"u_strength");

  glUniform4fv(lightDirLoc,1,viewDir);
  glUniform3fv(lightColLoc,1,color);
  glUniform1fv(strengthLoc,1,&strength);
  glBindVertexArray(screenVao);
  screenDraw->draw();
}
void Renderer::ambientLightRender(float ambience) {
  //temp
  GLuint screenVao=geometryVaoManager->get("data/geometry/Screen.json","data/geometry/Position.json")->getVao();
  GeometryDraw *screenDraw=geometryVaoManager->get("data/geometry/Screen.json","data/geometry/Position.json")->getDraw();

  //states
  glDepthMask(GL_FALSE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  glEnable(GL_CULL_FACE);

  //
  GLuint prog=programManager->get("data/shader/deferred/Ambient.json");
  GLuint ambienceLoc=glGetUniformLocation(prog,"u_ambience");
  glUseProgram(prog);
  glUniform1f(ambienceLoc,ambience);

  glBindVertexArray(screenVao);
  screenDraw->draw();

  //restore states
  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ZERO);
  glDisable(GL_CULL_FACE);
}
void Renderer::emissiveRender() {
  //temp
  GLuint screenVao=geometryVaoManager->get("data/geometry/Screen.json","data/geometry/Position.json")->getVao();
  GeometryDraw *screenDraw=geometryVaoManager->get("data/geometry/Screen.json","data/geometry/Position.json")->getDraw();

  //states
  glDepthMask(GL_FALSE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ONE);
  glEnable(GL_CULL_FACE);

  //
  glUseProgram(programManager->get("data/shader/deferred/Emissive.json"));

  //
  glBindVertexArray(screenVao);
  screenDraw->draw();

  //restore states
  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ZERO);
  glDisable(GL_CULL_FACE);
}
void Renderer::reflectionsRender(const float *invProjMat, const float *invViewMat) {
  //temp
  GLuint screenVao=geometryVaoManager->get("data/geometry/Screen.json","data/geometry/Position.json")->getVao();
  GeometryDraw *screenDraw=geometryVaoManager->get("data/geometry/Screen.json","data/geometry/Position.json")->getDraw();

  //states
  glDepthMask(GL_FALSE);
  glEnable(GL_BLEND);
  glEnable(GL_CULL_FACE);
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);

  //
  GLuint prog=programManager->get("data/shader/deferred/Reflection.json");
  glUseProgram(prog);

  GLint invProjLoc=glGetUniformLocation(prog,"u_invProjMat");
  glUniformMatrix4fv(invProjLoc,1,GL_TRUE,invProjMat);

  GLint viewLoc=glGetUniformLocation(prog,"u_invViewMat");
  glUniformMatrix4fv(viewLoc,1,GL_TRUE,invViewMat);

  //
  glBindVertexArray(screenVao);
  screenDraw->draw();

  //restore states
  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);
  glBlendFunc(GL_ONE, GL_ZERO);
  glDisable(GL_CULL_FACE);
}
void Renderer::normalsRender() {
  //temp
  GLuint screenVao=geometryVaoManager->get("data/geometry/Screen.json","data/geometry/Position.json")->getVao();
  GeometryDraw *screenDraw=geometryVaoManager->get("data/geometry/Screen.json","data/geometry/Position.json")->getDraw();
  glBindVertexArray(screenVao);

  glUseProgram(programManager->get("data/shader/deferred/Normals.json"));
  screenDraw->draw();
}
void Renderer::colorsRender() {
  //temp
  GLuint screenVao=geometryVaoManager->get("data/geometry/Screen.json","data/geometry/Position.json")->getVao();
  GeometryDraw *screenDraw=geometryVaoManager->get("data/geometry/Screen.json","data/geometry/Position.json")->getDraw();
  glBindVertexArray(screenVao);

  glUseProgram(programManager->get("data/shader/deferred/Colors.json"));
  screenDraw->draw();
}
void Renderer::linearDepthRender(float zNear, float zFar) {
  //temp
  GLuint screenVao=geometryVaoManager->get("data/geometry/Screen.json","data/geometry/Position.json")->getVao();
  GeometryDraw *screenDraw=geometryVaoManager->get("data/geometry/Screen.json","data/geometry/Position.json")->getDraw();
  glBindVertexArray(screenVao);

  //
  GLuint prog=programManager->get("data/shader/deferred/LinearDepth.json");
  glUseProgram(prog);
  int zNearFarLoc=glGetUniformLocation(prog,"u_zNearFar");

  float zNearFar[2]={zNear,zFar};
  glUniform2fv(zNearFarLoc,1,zNearFar);

  screenDraw->draw();
}
void Renderer::skyboxRender(const float *viewProjMat) {
  //states
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);
  glDepthRange(1.0,1.0);
  glDepthFunc(GL_EQUAL);

  //
  GLuint skyProg=programManager->get("data/shader/SkyBox.json");
  GeometryVao *gv=geometryVaoManager->get("data/geometry/SkyBox.json","data/geometry/Position.json");

  glUseProgram(skyProg);
  GLuint loc=glGetUniformLocation(skyProg,"u_viewProjMat");
  glUniformMatrix4fv(loc,1,GL_TRUE,viewProjMat);

  glBindVertexArray(gv->getVao());
  gv->getDraw()->draw();

  //restore states
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthRange(0.0,1.0);
  glDepthFunc(GL_LESS);

}

void Renderer::normalVectorsRender(const float *projMat) {
  //  glDepthMask(GL_FALSE);
  GLuint prog=programManager->get("data/shader/Normals.json");
  glUseProgram(prog);
  int modelViewProjMatLoc=glGetUniformLocation(prog,"u_modelViewProjMat");
  int modelViewMatLoc=glGetUniformLocation(prog,"u_modelViewMat");
  int normalMatLoc=glGetUniformLocation(prog,"u_normalMat");
  int projLoc=glGetUniformLocation(prog,"u_projMat");

  if(projLoc!=-1) glUniformMatrix4fv(projLoc,1,GL_TRUE,projMat  );

  // if(modelViewProjMatLoc!=-1) glUniformMatrix4fv(modelViewProjMatLoc,1,GL_TRUE,m->modelViewProjMat);
  // if(normalMatLoc!=-1) glUniformMatrix4fv(normalMatLoc,1,GL_TRUE,m->normalMat);
  // if(modelViewMatLoc!=-1) glUniformMatrix4fv(modelViewMatLoc,1,GL_TRUE,m->modelViewMat);

  //if(GeometryVao *gv=geometryVaoManager->get(m->geometry,m->vao)) {
  //  glBindVertexArray(gv->getVao());
  //
  //  if(GeometryDraw *d=gv->getDraw(m->draw)) {
  //    d->draw();
  //  }

  //glDepthMask(GL_TRUE);
}
