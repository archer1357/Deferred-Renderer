#include "Main.h"

GLFWwindow *window=0;
bool iconified=false;
bool focused=true;

ProgramManager *programManager;
TextureManager *textureManager;
GeometryVaoManager *geometryVaoManager;
Scene *scene=0;

GLuint deferredFbo=0;
GLuint deferredColorTex=0;
GLuint deferredNormalTex=0;
GLuint deferredDepthTex=0;

bool errorFromGl=false;
bool geometryShaderSupport=false;

int shadowCpuThreads=8;


std::string mainScript="data/Main.lua";

const char *geometryColoredProgFn="data/shader/geometry/Colored.json";
const char *geometryParallaxProgFn="data/shader/geometry/Bump.json";
const char *deferredDepthCopyProgFn="data/shader/deferred/DepthCopy.json";
const char *deferredPointlightProgFn="data/shader/deferred/PointLight.json";
const char *deferredSpotlightProgFn="data/shader/deferred/SpotLight.json";
const char *deferredDirectionallightProgFn="data/shader/deferred/DirectionalLight.json";
const char *deferredAmbientProgFn="data/shader/deferred/Ambient.json";
const char *deferredEmissiveProgFn="data/shader/deferred/Emissive.json";
const char *deferredReflectionProgFn="data/shader/deferred/Reflection.json";
const char *deferredDepthStencilProg="data/shader/deferred/DepthStencil.json";

const char *pointLightShadowCpuProgFn="data/shader/PointLightShadowCpu.json";
const char *pointLightShadowGpuProgFn="data/shader/PointLightShadowGpu.json";
const char *pointLightShadowGpuDebugProgFn="data/shader/PointLightShadowGpuDebug.json";

const char *directionalLightShadowCpuProgFn="data/shader/DirectionalLightShadowCpu.json";
const char *directionalLightShadowGpuProgFn="data/shader/DirectionalLightShadowGpu.json";
const char *directionalLightShadowGpuDebugProgFn="data/shader/DirectionalLightShadowGpuDebug.json";

const char *normalsDebugProgFn="data/shader/Normals.json";
const char *skyboxProgFn="data/shader/SkyBox.json";

const char *screenGeomFn="data/geometry/Screen.json";
const char *positionsVaoFn="data/geometry/Position.json";
const char *skyboxGeomFn="data/geometry/SkyBox.json";

const char *cubeEnvTexFn="data/texture/blue_light.cube";

bool checkGlError();

void render(int clientWidth, int clientHeight) {
  GLuint screenVao=geometryVaoManager->get(screenGeomFn,positionsVaoFn)->getVao();
  GeometryDraw *screenDraw=geometryVaoManager->get(screenGeomFn,positionsVaoFn)->getDraw();

  glViewport(0,0,clientWidth,clientHeight);

  //states
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  glDisable(GL_BLEND);
  glDisable(GL_STENCIL_TEST);

  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LEQUAL);

  //bind fbo
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, deferredFbo);

  //clear
  glClearColor(0.0f,0.0f,0.0f,0.0f);
  glClearDepth(1);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  //states
  glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

  //draw entity geom depths

  //states
  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

  //draw entity geoms
  {
    for(Mesh *m : scene->getMeshes()) {
      GLuint prog=0;

      if(m->material==Mesh::Color) {
        prog=programManager->get(geometryColoredProgFn);

      } else if(m->material==Mesh::Parallax) {
        prog=programManager->get(geometryParallaxProgFn);

        if(m->reliefTex.empty()) {
          prog=programManager->get("data/shader/geometry/Parallax2.json");
        } else {
          prog=programManager->get("data/shader/geometry/Parallax.json");
        }
      }

      if(GLuint tex=textureManager->get2d(m->colorTex)) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,tex);
      }

      if(GLuint tex=textureManager->get2d(m->normalTex)) {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D,tex);
        // std::cout << "n " << tex << std::endl;
      }

      if(GLuint tex=textureManager->get2d(m->heightTex)) {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D,tex);
      }

      if(GLuint tex=textureManager->get2d(m->reliefTex)) {
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

      int bumpScaleLoc=glGetUniformLocation(prog,"u_bumpScale");
      int bumpBiasLoc=glGetUniformLocation(prog,"u_bumpBias");

      int reliefScaleLoc=glGetUniformLocation(prog,"u_reliefScale");

      int parallaxScaleLoc=glGetUniformLocation(prog,"u_parallaxScale");
      int parallaxBiasLoc=glGetUniformLocation(prog,"u_parallaxBias");
      int parallaxInvertHeightLoc=glGetUniformLocation(prog,"u_parallaxInvertHeight");

      int zNearFarLoc=glGetUniformLocation(prog,"u_zNearFar");

      //
      if(colLoc!=-1) {
        glUniform3fv(colLoc,1,m->color);
      }

      glUniformMatrix4fv(modelViewProjMatLoc,1,GL_TRUE,m->modelViewProjMat);

      if(modelViewMatLoc!=-1) {
        glUniformMatrix4fv(modelViewMatLoc,1,GL_TRUE,m->modelViewMat);
      }

      if(normalMatLoc!=-1) {
        glUniformMatrix3fv(normalMatLoc,1,GL_TRUE,m->normalMat);
      }

      if(shininessLoc!=-1) {
        glUniform1fv(shininessLoc,1,&m->shininess);
      }

      if(emissiveLoc!=-1) {
        glUniform1i(emissiveLoc,m->emissive?1:0);
      }

      if(reflectiveLoc!=-1) {
        glUniform1fv(reflectiveLoc,1,&m->reflective);
      }

      //
      if(bumpScaleLoc!=-1) {
        glUniform1fv(bumpScaleLoc,1,&m->bumpScale);
      }

      if(bumpBiasLoc!=-1) {
        glUniform1fv(bumpBiasLoc,1,&m->bumpBias);
      }

      //
      if(reliefScaleLoc!=-1) {
        glUniform1fv(reliefScaleLoc,1,&m->reliefScale);
      }

      //
      if(parallaxScaleLoc!=-1) {
        glUniform1fv(parallaxScaleLoc,1,&m->parallaxScale);
      }

      if(parallaxBiasLoc!=-1) {
        glUniform1fv(parallaxBiasLoc,1,&m->parallaxBias);
      }

      if(parallaxInvertHeightLoc!=-1) {
        glUniform1i(parallaxInvertHeightLoc,m->parallaxInvertHeight?1:0);
      }

      if(zNearFarLoc!=-1) {
        float zNearFar[2]={m->zNear,m->zFar};
        glUniform2fv(zNearFarLoc,1,zNearFar);
      }

      //
      if(GeometryVao *gv=geometryVaoManager->get(m->geometry,m->vao)) {
        glBindVertexArray(gv->getVao());

        if(GeometryDraw *d=gv->getDraw(m->draw)) {
          d->draw();
        }
      }
    }
  }

  //bind fbo
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  //bind textures
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, deferredColorTex);
  glBindSampler(0,0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, deferredNormalTex);
  glBindSampler(1,0);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, deferredDepthTex);
  glBindSampler(2,0);

  //states
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glDepthFunc(GL_LESS);

  //clear
  glClearDepth(1.0f);
  glClear(GL_DEPTH_BUFFER_BIT);

  //render deferred depth texture
  glUseProgram(programManager->get(deferredDepthCopyProgFn));

  glBindVertexArray(screenVao);
  screenDraw->draw();

  //states
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glDepthMask(GL_FALSE);

  //clear
  glClearColor(0.0f,0.0f,0.0f,1.0f);
  glClearStencil(0);
  glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);


  //bind vao
  glBindVertexArray(screenVao);

  //states
  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_ONE, GL_ONE);

  //draw point lights
  {
    GLuint p=programManager->get(deferredPointlightProgFn);
    glUseProgram(p);
    GLint invProjLoc=glGetUniformLocation(p,"u_invProjMat");
    glUniformMatrix4fv(invProjLoc,1,GL_TRUE,scene->getInvProjMat());

    GLint lightPosLoc=glGetUniformLocation(p,"u_lightPos");
    GLint lightAttenLoc=glGetUniformLocation(p,"u_lightAtten");
    GLint lightColLoc=glGetUniformLocation(p,"u_lightCol");
    GLint strengthLoc=glGetUniformLocation(p,"u_strength");

    for(Pointlight *pl : scene->getPointlights()) {
      if(!pl->shadow) {
        glUniform4fv(lightPosLoc,1,pl->viewPos);
        glUniform3fv(lightAttenLoc,1,pl->attenuation);
        glUniform3fv(lightColLoc,1,pl->color);
        glUniform1fv(strengthLoc,1,&pl->strength);
        screenDraw->draw();
      }
    }
  }

  //draw spot lights
  {
    GLuint p=programManager->get(deferredSpotlightProgFn);
    glUseProgram(p);
    GLint invProjLoc=glGetUniformLocation(p,"u_invProjMat");
    glUniformMatrix4fv(invProjLoc,1,GL_TRUE,scene->getInvProjMat());

    for(Spotlight *sl : scene->getSpotlights()) {
      if(!sl->shadow) {
      }
    }
  }

  //draw directional lights
  {
    GLuint p=programManager->get(deferredDirectionallightProgFn);
    glUseProgram(p);
    GLint invProjLoc=glGetUniformLocation(p,"u_invProjMat");
    glUniformMatrix4fv(invProjLoc,1,GL_TRUE,scene->getInvProjMat());


    GLint lightDirLoc=glGetUniformLocation(p,"u_lightDir");
    GLint lightColLoc=glGetUniformLocation(p,"u_lightCol");
    GLint strengthLoc=glGetUniformLocation(p,"u_strength");

    for(Directionallight *dl : scene->getDirectionallights()) {
      if(!dl->shadow) {
        glUniform4fv(lightDirLoc,1,dl->viewDir);
        glUniform3fv(lightColLoc,1,dl->color);
        glUniform1fv(strengthLoc,1,&dl->strength);
        screenDraw->draw();
      }
    }
  }

  //states
  glEnable(GL_STENCIL_TEST);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_ONE, GL_ONE);

  //draw point lights with shadows
  {
    //
    GLuint lightProg=programManager->get(deferredPointlightProgFn);

    GLuint invProjLoc=glGetUniformLocation(lightProg,"u_invProjMat");
    GLint lightPosLoc=glGetUniformLocation(lightProg,"u_lightPos");
    GLint lightAttenLoc=glGetUniformLocation(lightProg,"u_lightAtten");
    GLint lightColLoc=glGetUniformLocation(lightProg,"u_lightCol");
    GLint strengthLoc=glGetUniformLocation(lightProg,"u_strength");

    glUseProgram(lightProg);
    glUniformMatrix4fv(invProjLoc,1,GL_TRUE,scene->getInvProjMat());

    //
    GLuint shadowProg=0;
    GLint zpassLoc2=-1;
    GLint robustLoc2=-1;

    if(geometryShaderSupport && scene->isGeometryShadows()) {
      shadowProg=programManager->get(pointLightShadowGpuProgFn);
      zpassLoc2=glGetUniformLocation(shadowProg,"u_zpass");
      robustLoc2=glGetUniformLocation(shadowProg,"u_robust");
    } else {
      shadowProg=programManager->get(pointLightShadowCpuProgFn);
    }

    GLint projMatLoc2=glGetUniformLocation(shadowProg,"u_projMat");
    GLint modelViewMatLoc2=glGetUniformLocation(shadowProg,"u_modelViewMat");
    GLint lightPosLoc2=glGetUniformLocation(shadowProg,"u_lightPos");

    glUseProgram(shadowProg);
    glUniformMatrix4fv(projMatLoc2,1,GL_TRUE,scene->getProjMat());

    //
    for(Pointlight *pl : scene->getPointlights()) {
      if(pl->shadow) {
        glDisable(GL_BLEND);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glDisable(GL_CULL_FACE);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_DEPTH_CLAMP);

        glStencilFunc(GL_ALWAYS, 0, 0xff);
        //...
        bool lastZpass=false;
        glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
        glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(0.1f,1.0f);

        glClearStencil(0);
        glClear(GL_STENCIL_BUFFER_BIT);

        //use shadow program
        glUseProgram(shadowProg);
        glUniform4fv(lightPosLoc2,1,pl->viewPos);

        //
        if(geometryShaderSupport && scene->isGeometryShadows()) {
          //    glEnable(GL_PRIMITIVE_RESTART);

          for(Shadow *shadow : scene->getShadows()) {
            if(lastZpass !=shadow->zpass) {
              lastZpass=shadow->zpass;

              if(!shadow->zpass) {
                glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
                glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
              } else {
                glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_INCR_WRAP);
                glStencilOpSeparate(GL_BACK , GL_KEEP, GL_KEEP, GL_DECR_WRAP);
              }
            }
            glUniform1i(zpassLoc2,shadow->zpass?1:0);
            glUniform1i(robustLoc2,shadow->robust?1:0);

            glUniformMatrix4fv(modelViewMatLoc2,1,GL_TRUE,shadow->modelViewMat);
            if(GeometryVao *gv=geometryVaoManager->get(shadow->geometry,positionsVaoFn)) {
              glBindVertexArray(gv->getVao());
              gv->getDraw("default")->draw();
            }
          }
        } else {
          for(Shadow *shadow : scene->getShadows()) {
            if(!shadow->zpass) {
              glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
              glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
            } else {
              glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_INCR_WRAP);
              glStencilOpSeparate(GL_BACK , GL_KEEP, GL_KEEP, GL_DECR_WRAP);
            }
            if(shadow->cpuShadow) {
              glUniformMatrix4fv(modelViewMatLoc2,1,GL_TRUE,
                                 shadow->modelViewMat);

              shadow->cpuShadow->draw(geometryVaoManager,pl,shadowCpuThreads,false);
            }
          }
        }
      }

      //draw light

      glDisable(GL_POLYGON_OFFSET_FILL);
      glEnable(GL_BLEND);
      glEnable(GL_CULL_FACE);
      glDisable(GL_DEPTH_TEST);
      glDisable(GL_DEPTH_CLAMP);
      glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

      glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
      glStencilFunc(GL_EQUAL, 0, 0xff);

      glUseProgram(lightProg);
      glUniform4fv(lightPosLoc,1,pl->viewPos);
      glUniform3fv(lightAttenLoc,1,pl->attenuation);
      glUniform3fv(lightColLoc,1,pl->color);
      glUniform1fv(strengthLoc,1,&pl->strength);
      glBindVertexArray(screenVao);

      screenDraw->draw();
    }
  }
  //draw spot lights with shadows
  {

  }

  //draw directional lights with shadows
  //if(geometryShaderSupport && scene->isGeometryShadows()) //cpu directional shadows not implemented
  {
    GLuint p=programManager->get(deferredDirectionallightProgFn);
    glUseProgram(p);

    GLint invProjLoc=glGetUniformLocation(p,"u_invProjMat");
    glUniformMatrix4fv(invProjLoc,1,GL_TRUE,scene->getInvProjMat());


    GLint lightDirLoc=glGetUniformLocation(p,"u_lightDir");
    GLint lightColLoc=glGetUniformLocation(p,"u_lightCol");
    GLint strengthLoc=glGetUniformLocation(p,"u_strength");

    //
    GLuint shadowProg=0;
    GLint zpassLoc2=-1;
    GLint robustLoc2=-1;

    if(geometryShaderSupport && scene->isGeometryShadows()) {
      shadowProg=programManager->get(directionalLightShadowGpuProgFn);
      zpassLoc2=glGetUniformLocation(shadowProg,"u_zpass");
      robustLoc2=glGetUniformLocation(shadowProg,"u_robust");
    } else {
      shadowProg=programManager->get(directionalLightShadowCpuProgFn);
    }

    GLint projMatLoc2=glGetUniformLocation(shadowProg,"u_projMat");
    GLint modelViewMatLoc2=glGetUniformLocation(shadowProg,"u_modelViewMat");
    GLint lightDirLoc2=glGetUniformLocation(shadowProg,"u_lightDir");

    glUseProgram(shadowProg);
    glUniformMatrix4fv(projMatLoc2,1,GL_TRUE,scene->getProjMat());

    for(Directionallight *dl : scene->getDirectionallights()) {
      if(dl->shadow) {
        glDisable(GL_BLEND);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        glDisable(GL_CULL_FACE);

        glEnable(GL_DEPTH_TEST);
        glEnable(GL_DEPTH_CLAMP);

        glStencilFunc(GL_ALWAYS, 0, 0xff);

        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(0.3f,1.0f);

        glClearStencil(0);
        glClear(GL_STENCIL_BUFFER_BIT);

        //use shadow program
        glUseProgram(shadowProg);
        glUniform4fv(lightDirLoc2,1,dl->viewDir);

        //
        if(geometryShaderSupport && scene->isGeometryShadows()) {
          for(Shadow *shadow : scene->getShadows()) {


            if(!shadow->zpass) {
              glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
              glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
            } else {
              glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_INCR_WRAP);
              glStencilOpSeparate(GL_BACK , GL_KEEP, GL_KEEP, GL_DECR_WRAP);
            }


            glUniform1i(zpassLoc2,shadow->zpass?1:0);
            glUniform1i(robustLoc2,shadow->robust?1:0);
            glUniformMatrix4fv(modelViewMatLoc2,1,GL_TRUE,shadow->modelViewMat);

            if(GeometryVao *gv=geometryVaoManager->get(shadow->geometry,positionsVaoFn)) {
              glBindVertexArray(gv->getVao());
              gv->getDraw("default")->draw();
            }
          }
        } else {

          for(Shadow *shadow : scene->getShadows()) {
            if(!shadow->zpass) {
              glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
              glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
            } else {
              glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_INCR_WRAP);
              glStencilOpSeparate(GL_BACK , GL_KEEP, GL_KEEP, GL_DECR_WRAP);
            }
            if(shadow->cpuShadow) {
              glUniformMatrix4fv(modelViewMatLoc2,1,GL_TRUE,
                                 shadow->modelViewMat);

              shadow->cpuShadow->draw(geometryVaoManager,dl,shadowCpuThreads,false);

            }
          }
        }

        //draw light
        glDisable(GL_POLYGON_OFFSET_FILL);
        glEnable(GL_BLEND);
        glEnable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_DEPTH_CLAMP);
        glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

        glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
        glStencilFunc(GL_EQUAL, 0, 0xff);

        glUseProgram(p);
        glUniform4fv(lightDirLoc,1,dl->viewDir);
        glUniform3fv(lightColLoc,1,dl->color);
        glUniform1fv(strengthLoc,1,&dl->strength);
        glBindVertexArray(screenVao);
        screenDraw->draw();
      }
    }
  }

  //
  glDisable(GL_STENCIL_TEST);

  //states
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CCW);

  glDepthMask(GL_FALSE);
  glDisable(GL_DEPTH_TEST);
  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

  //bind vao
  glBindVertexArray(screenVao);

  //draw ambients
  {
    GLuint prog=programManager->get(deferredAmbientProgFn);
    GLuint ambienceLoc=glGetUniformLocation(prog,"u_ambience");
    glUseProgram(prog);
    glUniform1f(ambienceLoc,0.05f);
    screenDraw->draw();
  }

  //draw emissives
  glUseProgram(programManager->get(deferredEmissiveProgFn));
  screenDraw->draw();

  //bind sky texture to 3
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_CUBE_MAP,textureManager->getCube(cubeEnvTexFn));

  //states
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);

  //draw reflections
  {
    GLuint prog=programManager->get(deferredReflectionProgFn);
    glUseProgram(prog);

    GLint invProjLoc=glGetUniformLocation(prog,"u_invProjMat");
    glUniformMatrix4fv(invProjLoc,1,GL_TRUE,scene->getInvProjMat());

    GLint viewLoc=glGetUniformLocation(prog,"u_invViewMat");
    glUniformMatrix4fv(viewLoc,1,GL_TRUE,scene->getInvViewMat());
    screenDraw->draw();
  }

  //states
  glBlendFunc(GL_ONE, GL_ONE);

  //draw color test
  // glUseProgram(programManager->get(""));
  // screenDraw->draw();

  //states
  glDisable(GL_BLEND);
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_ALWAYS, 0, 0xff);
  glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);

  //clear stencil
  glClearStencil(0);
  glClear(GL_STENCIL_BUFFER_BIT);

  //stencil out sky area
  glUseProgram(programManager->get(deferredDepthStencilProg));
  screenDraw->draw();

  //states
  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
  glStencilFunc(GL_EQUAL, 0x0, 0xff);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

  //draw sky
  {

    GLuint skyProg=programManager->get(skyboxProgFn);
    GeometryVao *gv=geometryVaoManager->get(skyboxGeomFn,positionsVaoFn);

    glUseProgram(skyProg);
    GLuint loc=glGetUniformLocation(skyProg,"u_viewRotProjMat");
    glUniformMatrix4fv(loc,1,GL_TRUE,scene->getViewRotProjMat());

    glBindVertexArray(gv->getVao());
    gv->getDraw()->draw();
  }

  //states
  glDisable(GL_STENCIL_TEST);
  glDisable(GL_CULL_FACE);
  glDisable(GL_BLEND);
  //glBlendEquation(GL_FUNC_ADD);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_DEPTH_CLAMP);

  //point light shadow debug
  if(scene->isShadowDebug()) {
    GLuint shadowProg=0;
    GLint zpassLoc2=-1;
    GLint robustLoc2=-1;

    if(geometryShaderSupport && scene->isGeometryShadows()) {
      shadowProg=programManager->get(pointLightShadowGpuDebugProgFn);
      zpassLoc2=glGetUniformLocation(shadowProg,"u_zpass");
      robustLoc2=glGetUniformLocation(shadowProg,"u_robust");
    } else {
      shadowProg=programManager->get(pointLightShadowCpuProgFn);
    }

    GLint projMatLoc2=glGetUniformLocation(shadowProg,"u_projMat");
    GLint modelViewMatLoc2=glGetUniformLocation(shadowProg,"u_modelViewMat");
    GLint lightPosLoc2=glGetUniformLocation(shadowProg,"u_lightPos");

    glUseProgram(shadowProg);
    glUniformMatrix4fv(projMatLoc2,1,GL_TRUE,scene->getProjMat());

    for(Pointlight *pl : scene->getPointlights()) {
      if(pl->shadow) {
        glUniform4fv(lightPosLoc2,1,pl->viewPos);

        if(geometryShaderSupport && scene->isGeometryShadows()) {
          for(Shadow *shadow : scene->getShadows()) {
            glUniform1i(zpassLoc2,shadow->zpass?1:0);
            glUniform1i(robustLoc2,shadow->robust?1:0);
            glUniformMatrix4fv(modelViewMatLoc2,1,GL_TRUE,shadow->modelViewMat);

            if(GeometryVao *gv=geometryVaoManager->get(shadow->geometry,positionsVaoFn)) {
              glBindVertexArray(gv->getVao());
              gv->getDraw("default")->draw();
            }
          }
        } else {

          for(Shadow *shadow : scene->getShadows()) {
            if(shadow->cpuShadow ) {
              glUniformMatrix4fv(modelViewMatLoc2,1,GL_TRUE,
                                 shadow->modelViewMat);

              shadow->cpuShadow->draw(geometryVaoManager,pl,shadowCpuThreads,true);

            }
          }
        }
      }
    }
  }

  //directional light shadow debug
    if(scene->isShadowDebug()) {
    //
    GLuint shadowProg=0;
    GLint zpassLoc2=-1;
    GLint robustLoc2=-1;

    if(geometryShaderSupport && scene->isGeometryShadows()) {
      shadowProg=programManager->get(directionalLightShadowGpuDebugProgFn);
      zpassLoc2=glGetUniformLocation(shadowProg,"u_zpass");
      robustLoc2=glGetUniformLocation(shadowProg,"u_robust");
    } else {
      shadowProg=programManager->get(directionalLightShadowCpuProgFn);
    }

    GLint projMatLoc2=glGetUniformLocation(shadowProg,"u_projMat");
    GLint modelViewMatLoc2=glGetUniformLocation(shadowProg,"u_modelViewMat");
    GLint lightDirLoc2=glGetUniformLocation(shadowProg,"u_lightDir");

    glUseProgram(shadowProg);
    glUniformMatrix4fv(projMatLoc2,1,GL_TRUE,scene->getProjMat());

    for(Directionallight *dl : scene->getDirectionallights()) {
      if(dl->shadow) {
        //use shadow program
        glUniform4fv(lightDirLoc2,1,dl->viewDir);

        //
        if(geometryShaderSupport && scene->isGeometryShadows()) {
          for(Shadow *shadow : scene->getShadows()) {
            glUniform1i(zpassLoc2,shadow->zpass?1:0);
            glUniform1i(robustLoc2,shadow->robust?1:0);
            glUniformMatrix4fv(modelViewMatLoc2,1,GL_TRUE,shadow->modelViewMat);

            if(GeometryVao *gv=geometryVaoManager->get(shadow->geometry,positionsVaoFn)) {
              glBindVertexArray(gv->getVao());
              gv->getDraw("default")->draw();
            }
          }
        } else {

          for(Shadow *shadow : scene->getShadows()) {
            if(shadow->cpuShadow) {
              glUniformMatrix4fv(modelViewMatLoc2,1,GL_TRUE,
                                 shadow->modelViewMat);

              shadow->cpuShadow->draw(geometryVaoManager,dl,shadowCpuThreads,true);

            }
          }
        }
      }
    }
  }

  //states
  glEnable(GL_CULL_FACE);
  glDisable(GL_DEPTH_CLAMP);
  glDepthMask(GL_FALSE);
  //glDisable(GL_DEPTH_TEST);
  //glDepthFunc(GL_LESS);

  //draw normals, triangle normals, tangents, bitangents
  if(false && geometryShaderSupport) {
    GLuint prog=programManager->get(normalsDebugProgFn);
    glUseProgram(prog);
    int modelViewProjMatLoc=glGetUniformLocation(prog,"u_modelViewProjMat");
    int modelViewMatLoc=glGetUniformLocation(prog,"u_modelViewMat");
    int normalMatLoc=glGetUniformLocation(prog,"u_normalMat");
    int projLoc=glGetUniformLocation(prog,"u_projMat");

    if(projLoc!=-1) glUniformMatrix4fv(projLoc,1,GL_TRUE,scene->getProjMat()  );
    for(Mesh *m : scene->getMeshes()) {
      if(modelViewProjMatLoc!=-1) glUniformMatrix4fv(modelViewProjMatLoc,1,GL_TRUE,m->modelViewProjMat);
      if(normalMatLoc!=-1) glUniformMatrix4fv(normalMatLoc,1,GL_TRUE,m->normalMat);
      if(modelViewMatLoc!=-1) glUniformMatrix4fv(modelViewMatLoc,1,GL_TRUE,m->modelViewMat);

      if( m->material==Mesh::Normal ||
          m->material==Mesh::Bump ||
          m->material==Mesh::Parallax) {
        if(GeometryVao *gv=geometryVaoManager->get(m->geometry,m->vao)) {
          glBindVertexArray(gv->getVao());

          if(GeometryDraw *d=gv->getDraw(m->draw)) {
            d->draw();
          }
        }
      }
    }
  }
}

void initDeferred() {
  glGenFramebuffers(1,&deferredFbo);
  glGenTextures(1,&deferredColorTex);
  glGenTextures(1,&deferredNormalTex);
  glGenTextures(1,&deferredDepthTex);
}

void uninitDeferred() {
  glDeleteFramebuffers(1,&deferredFbo);
  glDeleteTextures(1,&deferredColorTex);
  glDeleteTextures(1,&deferredNormalTex);
  glDeleteTextures(1,&deferredDepthTex);
}

bool setupDeferred(int w, int h) {
  std::cout << "setupDeferred " << w << " " << h << std::endl;
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, deferredFbo);

  //color
  glBindTexture(GL_TEXTURE_2D, deferredColorTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,w,h,0,GL_RGBA,GL_FLOAT,0);
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
  glTexImage2D(GL_TEXTURE_2D,0,normalInnerFormat,w,h,0,GL_RGBA,normalType,0);

  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT1,
                         GL_TEXTURE_2D,deferredNormalTex,0);

  //depth
  glBindTexture(GL_TEXTURE_2D, deferredDepthTex);
  glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT32F,w,h,0,GL_DEPTH_COMPONENT,GL_FLOAT,0);
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

void onKey(GLFWwindow *window,int key,int scancode,int action, int mods) {
  if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GL_TRUE);
  } else {
    scene->onKey(key,action,mods);
  }
}

void onMouseButton(GLFWwindow *window,int button,int action,int mods) {
  scene->onMouseButton(button,action,mods);
}

void onCursor(GLFWwindow *window,double x,double y) {
  scene->onCursor(x,y);
}

void onScroll(GLFWwindow *window,double x,double y) {
  scene->onScroll(x,y);
}

void onCursorEnter(GLFWwindow *window,int entered) {
}

void onChar(GLFWwindow *window,unsigned int c) {
  scene->onChar(c);
}

void onIconify(GLFWwindow *window, int iconify) {
  iconified=iconify==GL_TRUE;
}

void onFocus(GLFWwindow *window, int focus) {
  focused=focus==GL_TRUE;
}

void printGLExts() {
  int extNum;
  glGetIntegerv(GL_NUM_EXTENSIONS,&extNum);

  for(int i=0;i<extNum;i++) {
    std::cout << glGetStringi(GL_EXTENSIONS,i) << std::endl;
  }
}

void printVers() {
  std::cout << "GLFW " << glfwGetVersionString() << std::endl;
  std::cout << "GLEW " << glewGetString(GLEW_VERSION) << std::endl;
  std::cout << "GL " << glGetString(GL_VERSION) << std::endl;
  std::cout << "Shader version " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  std::cout << std::endl;
}

class UpdateListener : public FW::FileWatchListener {
public:
  void handleFileAction(FW::WatchID watchid,const FW::String& dir,
                        const FW::String& filename,FW::Action action) {

    //
    if(action == FW::Action::Delete || action == FW::Action::Modified) {
      programManager->onFileModified(filename);
      textureManager->onFileModified(filename);
      geometryVaoManager->onFileModified(filename);
    }

    //
    if(action == FW::Action::Modified) {
      scene->onFileModified(dir + "/" +filename);

    }

    //
    std::cout << "\"" << dir <<  "/" <<filename << "\" ";

    if(action == FW::Action::Add) {
      std::cout << "added";
    } else if(action == FW::Action::Delete) {
      std::cout << "deleted";
    } else if(action == FW::Action::Modified) {
      std::cout << "modified";
    }

    std::cout << "." << std::endl;
  }
};



int main(int argc, char* argv[]) {
  if(argc>1) {
    mainScript=argv[1];
  }

  //glfw
  if(!glfwInit()) {
    std::cout << "GLFW init error.\n";
    return 1;
  }

  window = glfwCreateWindow(1280, 600, "Demo", 0, 0);

  if(!window) {
    glfwTerminate();
    std::cout << "GLFW window creation error.\n";
    return 2;
  }

  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  //glfw callbacks
  glfwSetKeyCallback(window, onKey);
  glfwSetMouseButtonCallback(window, onMouseButton);
  glfwSetCursorPosCallback(window, onCursor);
  glfwSetScrollCallback(window, onScroll);
  glfwSetWindowIconifyCallback(window,onIconify);
  glfwSetWindowFocusCallback(window,onFocus);
  glfwSetCursorEnterCallback(window, onCursorEnter);
  glfwSetCharCallback(window, onChar);

  //glew
  GLenum glewErr = glewInit();

  if(GLEW_OK != glewErr) {
    std::cout << "GLEW Error " << glewGetErrorString(glewErr) << std::endl;
    glfwTerminate();
    return 3;
  }

  //
  printVers();

  //


  int majorVer,minorVer;
  glGetIntegerv(GL_MAJOR_VERSION,&majorVer);
  glGetIntegerv(GL_MINOR_VERSION,&minorVer);
  //std::cout << majorVer<<" "<<minorVer << std::endl;

  //
  programManager=new ProgramManager();

  //
  if(majorVer>3 || minorVer>=3) {
    geometryShaderSupport=true;
    programManager->setGeometryShaderSupport(true);
    glPrimitiveRestartIndex(-1);
  }

  //
  initDeferred();

  //inits
  textureManager=new TextureManager();
  geometryVaoManager=new GeometryVaoManager();
  scene=new Scene(mainScript);

  //
  UpdateListener *fileListener=new UpdateListener();
  FW::FileWatcher fileWatcher;
 
  fileWatcher.addWatch("data/shader/geometry", fileListener);
  fileWatcher.addWatch("data/shader/deferred", fileListener);
  fileWatcher.addWatch("data/shader", fileListener);
  fileWatcher.addWatch("data/texture", fileListener);
  fileWatcher.addWatch("data/geometry", fileListener);
  fileWatcher.addWatch("data", fileListener);

  //
  int lastClientWidth=0,lastClientHeight=0;

  //
  while(!glfwWindowShouldClose(window)) {
    fileWatcher.update();
    double time = glfwGetTime();
    int clientWidth,clientHeight;
    glfwGetWindowSize(window, &clientWidth, &clientHeight);

    scene->run(time,clientWidth,clientHeight);

    glfwSetInputMode(window,GLFW_CURSOR, scene->isLockCursor()?GLFW_CURSOR_DISABLED:GLFW_CURSOR_NORMAL);


    if(lastClientWidth != clientWidth || lastClientHeight != clientHeight) {
      setupDeferred(clientWidth?clientWidth:128, clientHeight?clientHeight:128);
    }

    render(clientWidth,clientHeight);
    glfwSwapBuffers(window);
    glfwPollEvents();

    //
    lastClientWidth=clientWidth;
    lastClientHeight=clientHeight;
  }

  //uninits

  delete programManager;
  delete textureManager;
  delete geometryVaoManager;
  delete scene;


  uninitDeferred();

  //
  glfwTerminate();
  return 0;
}

bool checkGlError() {
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
        std::cout << "gl error : " << errors[i] << std::endl;
        return true;
      }
    }
  }

  return false;
}
