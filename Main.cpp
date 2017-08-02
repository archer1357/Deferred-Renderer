#include "Main.h"


bool checkGlError(const char *n=0) {
  static const char *errors[]={
    "invalid enum","invalid value","invalid operation",
    "invalid frame buffer operation","out of memory" };

  static const GLenum codes[]={
    GL_INVALID_ENUM,GL_INVALID_VALUE,GL_INVALID_OPERATION,
    GL_INVALID_FRAMEBUFFER_OPERATION,GL_OUT_OF_MEMORY };

  int errCode = glGetError();

  if(errCode != GL_NO_ERROR) {
    for(int i=0;i<5;i++) {
      if(codes[i]==errCode) {

        if(n) {
          std::cerr << n  <<  ": ";
        }

        std::cerr << "gl error : " << errors[i] << std::endl;
        return true;
      }
    }
  }

  return false;
}

bool checkFboError(const char *n=0) {
  GLenum status = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);

  if(status == GL_FRAMEBUFFER_COMPLETE) {
    return true;
  }

  if(n) {
    std::cerr << n  <<  ": ";
  }

  if(status==GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT) {
    std::cerr << "deferredFbo error: incomplete attachment\n";
  } else if(status==GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT) {
    std::cerr << "deferredFbo error: incomplete missing attachment\n";
  } else if(status==GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER) {
    std::cerr << "deferredFbo error: incomplete draw buffer\n";
  } else if(status==GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER) {
    std::cerr << "deferredFbo error: incomplete read buffer\n";
  } else if(status==GL_FRAMEBUFFER_UNSUPPORTED) {
    std::cerr << "deferredFbo error: unsupported\n";
  }

  return false;
}


void update() {
  fpsTimeCount+=deltaTime;

  if(fpsTimeCount>1.0) {
    fpsTimeCount-=1.0;
    fpsTotal=fpsCount;
    fpsCount=0;
  }
  fpsCount++;
  angle=(float)curTime;

  //
  mat4::perspective(projMat,(float)M_PI_4,(float)renderWidth/(float)renderHeight,zNear,zFar); //projMat
  mat4::inverse(invProjMat,projMat); //invProjMat
  memcpy(viewMat,camera.getViewMat(),sizeof(viewMat)); //viewMat
  mat4::inverse(invViewMat,viewMat); //invViewMat
  mat4::multiply(viewProjMat,projMat,viewMat); //viewProjMat
  mat4::ortho2d(proj2dMat,0.0f,(float)renderWidth,(float)renderHeight,0.0f); //proj2dMat

  //
  uniformManager.setUniformMatrix4fv("u_projMat",1,true,projMat);
  uniformManager.setUniformMatrix4fv("u_invProjMat",1,true,invProjMat);
  uniformManager.setUniformMatrix4fv("u_invViewMat",1,true,invViewMat);
  uniformManager.setUniformMatrix4fv("u_viewProjMat",1,true,viewProjMat);
  uniformManager.setUniform1f("u_zNear",zNear);
  uniformManager.setUniform1f("u_zFar",zFar);
  uniformManager.setUniform2f("u_screenSize",(float)renderWidth,(float)renderHeight);

  //
  mat4::identity(headObj->modelMat);
  mat4::translate(headObj->modelMat,0.0f,6.0f,0.0f, true);
  mat4::rotateY(headObj->modelMat,angle,true);
  mat4::scale(headObj->modelMat,2.0f,2.0f,2.0f, true);
  colKfr.calc((float)frame,true,headObj->color);

  //light2->dir[0]=cos(angle/15.0f);
  //light2->dir[2]=sin(angle/15.0f);

  light2->dir[0]=0.5f;
  light2->dir[1]=-1.0f;
  light2->dir[2]=0.8f;
  vec3::normal(light2->dir,light2->dir);


  mat4::translate(cubeObj->modelMat,0.0f,80.0f,0.0f);
  //mat4::multiplyRotateAxis(cubeObj->modelMat,0.0f,1.0f,0.0f,(float)curTime);

  //
  //lightPosKfr.calc((float)frame,true,light1->pos);

  //for(int i=0;i<3;i++) { light1->pos[i]=camera.getPos()[i]+camera.getRight()[i]; }


  //
  for(auto object : objects) {
    mat4::multiply(object->modelViewMat,viewMat,object->modelMat); //modelViewMat
    mat4::multiply(object->modelViewProjMat,projMat,object->modelViewMat); //modelViewProjMat
    mat4::normal3(object->normalMat,object->modelViewMat); //normalMat
  }

  //
  for(auto light : lights) {
    if(PointLight *pointLight=dynamic_cast<PointLight*>(light)) {
      mat4::multiplyVector(pointLight->viewPos,viewMat,pointLight->pos,1.0f); //viewPos
    } else if(SpotLight *spotLight=dynamic_cast<SpotLight*>(light)) {
      mat4::multiplyVector(spotLight->viewPos,viewMat,spotLight->pos,1.0f); //viewPos
      mat4::multiplyVector(spotLight->viewDir,viewMat,spotLight->dir,0.0f); //viewDir
    } else if(DirLight *dirLight=dynamic_cast<DirLight*>(light)) {
      mat4::multiplyVector(dirLight->viewDir,viewMat,dirLight->dir,0.0f); //viewDir
    }
  }

}

void renderTextMenu() {
  //bind font texture to 4
  // glActiveTexture(GL_TEXTURE4);
  // glBindTexture(GL_TEXTURE_2D,myFont->texture);

  // //text draw states
  // glEnable(GL_DEPTH_TEST);
  // glDepthFunc(GL_ALWAYS);
  // glDepthMask(GL_FALSE);
  // glEnable(GL_BLEND);
  // // glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_ALPHA);
  // glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  // glEnable(GL_CULL_FACE);

  //
  // uniformManager.setUniformMatrix4fv("u_proj2dMat",1,true,proj2dMat);

  //
  // GLuint fontProg=shaderManager.get("shader/Font.vs","","shader/Font.fs", "","","");
  // glUseProgram(fontProg);
  // uniformManager.apply(fontProg);

  //text menu
  std::stringstream ssText;
  ssText << fpsTotal << " fps\n\n";
  ssText << "[1] Point light : " << (light1->on?"on":"off") << "\n";

  if(geometryShaderSupport) {
    ssText << "[2] Debug shadows : " << (useShadowDebug?"on":"off") << "\n";
    ssText << "[3] Backfaced shadows : " << (useBackfaceShadow?"on":"off") << "\n";
    //ssText << "[4] Smooth shadows : " << (useShadowSmooth?"on":"off") << "\n";
    ssText << "[5] Polygon offset shadows : " << (useShadowPolygonOffset?"on":"off") << "\n";
    ssText << "[6] ZPass shadows : " << (useShadowZPass?"on":"off") << "\n";
    ssText << "[7] Depth func less equal shadows : " << (useShadowDepthLessEqual?"on":"off") << "\n";
  } else {
    ssText << "\nNo shadows (geometry shaders unsupported)\n";
  }
  ssText << "[8] Directional light : " << (light2->on?"on":"off") << "\n";
  ssText << "[9] SSAO : " << (useSsao?"on":"off") << "\n";

  ssText << "[U] Anti-aliasing : ";

  if(useAntiAliasing==0) {
    ssText << "off";
  } else if(useAntiAliasing==1) {
    ssText << "fxaa v1";
  } else if(useAntiAliasing==2) {
    ssText << "fxaa v2";
  }
  ssText << "\n";

  ssText << "[T] Set pointlight to camera\n";
  ssText << "[M] Other Model\n";

  ssText << "[F5] Refresh resources\n";
  ssText << '\r';
  ssText << '\r';
  ssText << '\r';
  ssText << '\r';

  ssText << "bla#\n";
  ssText << "`~!@#$%^&*()-_=+[{]}\\|;:'\",<.>/?\n";
  //draw text
  // myFont->generate(ssText.str(),0.0f,0.0f,true);
  // glBindVertexArray(myFont->getVao());
  // myFont->draw();

  //font bitmap test
  // glUseProgram(shaderManager.get("shader/deferred/Common.vs","","shader/Font.fs","","",""));
  // glBindVertexArray(screenVao);
  // screenDraw.draw();

  //text restore draw states
  // glDisable(GL_DEPTH_TEST);
  // glDepthFunc(GL_LESS);
  // glDepthMask(GL_TRUE);
  // glDisable(GL_BLEND);
  // glBlendFunc(GL_ONE,GL_ZERO);
  // glDisable(GL_CULL_FACE);


  drawScreenFont(screenFont,ssText.str().c_str(),window_client_width(),
                 window_client_height());

}

void renderSky() {
  //sky draw states
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);
  glDepthRange(1.0,1.0);
  glDepthFunc(GL_LEQUAL);

  //sky shader program
  GLuint skyProg = shaderManager.get("shader/Skybox.vs","","shader/Skybox.fs","","","");
  glUseProgram(skyProg);
  uniformManager.apply(skyProg);

  //sky cube map
  glActiveTexture(GL_TEXTURE3);
  glBindTexture(GL_TEXTURE_CUBE_MAP,textureManager.getCube("texture/blue_light.cube"));

  //sky draw
  glBindVertexArray(skyVao);
  skyDraw.draw();

  //sky restore draw states
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthRange(0.0,1.0);
  glDepthFunc(GL_LESS);
}

void renderDeferredColorTest() {
  glUseProgram(shaderManager.get("shader/deferred/Common.vs","","shader/deferred/Colors.fs","","",""));
  glBindVertexArray(screenVao);
  screenDraw.draw();
}

void renderDeferredNormalTest() {
  glUseProgram(shaderManager.get("shader/deferred/Common.vs","","shader/deferred/Normals.fs","","",""));
  glBindVertexArray(screenVao);
  screenDraw.draw();
}

void renderGeometry() {
  //geometry draw states
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);

  //draw geometry
  GLuint geomProg = shaderManager.get("shader/geometry/Simple.vs","","shader/geometry/Simple.fs","","","");
  glUseProgram(geomProg);

  for(auto object : objects) {
    if(object->vao == 0 || !object->visible) {
      continue;
    }

    uniformManager.setUniformMatrix4fv("u_modelViewProjMat",1,true,object->modelViewProjMat);
    uniformManager.setUniformMatrix4fv("u_modelViewMat",1,true,object->modelViewMat);
    uniformManager.setUniformMatrix3fv("u_normalMat",1,true,object->normalMat);
    uniformManager.setUniform1f("u_shininess",object->shininess);
    uniformManager.setUniform1f("u_emissive",object->emissive);
    uniformManager.setUniform3fv("u_col",1,object->color);
    uniformManager.setUniform1i("u_unlit",object->lit?0:1);
    uniformManager.apply(geomProg);

    glBindVertexArray(object->vao);
    object->draw.draw();
  }

  //geometry restore draw states
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
}

void renderDeferredEmissive() {
  GLuint emissiveProg=shaderManager.get("shader/deferred/Common.vs","","shader/deferred/Emissive.fs", "","","");
  glUseProgram(emissiveProg);
  glBindVertexArray(screenVao);
  screenDraw.draw();
}

void renderDeferredAmbient() {
  GLuint ambienceProg = 0;
  uniformManager.setUniform1f("u_ambience",0.15f);
  ambienceProg=shaderManager.get("shader/deferred/Common.vs","","shader/deferred/Ambient.fs", "","","");

  glUseProgram(ambienceProg);
  uniformManager.apply(ambienceProg);

  glBindVertexArray(screenVao);
  screenDraw.draw();
}


void renderShadows(Light *light,bool debugShadowsOnly,bool backFace,bool zPass,bool shadowWedge) {
  if(!geometryShaderSupport) {
    return;
  }

  GLuint shadowProg=0;
  std::string geomDefs;
  std::string debugFs;

  if(backFace) {
    geomDefs+="BACKFACE;";
  }

  if(zPass) {
    geomDefs+="ZPASS;";
  }

  if(debugShadowsOnly) {
    geomDefs+="DEBUG;";
  }

  if(PointLight *pointLight=dynamic_cast<PointLight*>(light)) {
    geomDefs+="POINT;";
    uniformManager.setUniform4fv("u_lightPos",1,pointLight->viewPos);
  } else if(SpotLight *spotLight=dynamic_cast<SpotLight*>(light)) {
    geomDefs+="SPOT;";
    uniformManager.setUniform4fv("u_lightPos",1,spotLight->viewPos);
    uniformManager.setUniform4fv("u_lightDir",1,spotLight->viewDir);
  } else if(DirLight *dirLight=dynamic_cast<DirLight*>(light)) {
    geomDefs+="DIRECTIONAL;";
    uniformManager.setUniform4fv("u_lightDir",1,dirLight->viewDir);
  } else {
    return;
  }

  if(debugShadowsOnly) {
    debugFs="shader/ShadowVolDebug.fs";
  }

  shadowProg = shaderManager.get("shader/ShadowVol.vs","shader/ShadowVol.gs",debugFs, "",geomDefs,"");
  //shadowProg = shaderManager.get("shader/ShadowVol.vs","shader/ShadowVol2.gs","","","","");

  if(shadowWedge) {

   // shadowProg = shaderManager.get("shader/ShadowVol.vs","shader/ShadowVol3.gs","shader/ShadowVolDebug.fs","","","");
  }
  //
  glUseProgram(shadowProg);

  //

  //begin draw states
  if(useShadowPolygonOffset) {
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f,3.0f);
  }

  glEnable(GL_DEPTH_CLAMP);

  if(useShadowDepthLessEqual) {
    glDepthFunc(GL_LEQUAL);
  }

  //
  for(auto object : objects) {
    if(object->shadowVao==0 || !object->visible) {
      continue;
    }

    uniformManager.setUniformMatrix4fv("u_modelViewMat",1,true,object->modelViewMat);

    if(useShadowSmooth) {
      uniformManager.setUniformMatrix3fv("u_normalMat",1,true,object->normalMat);
    }

    if(geometryShaderSupport) {
      uniformManager.setUniform1f("u_smoothBias",object->shadowSmoothBias);
    }

    uniformManager.apply(shadowProg);

    if(geometryShaderSupport) {
      glBindVertexArray(object->shadowVao);
      object->shadowDraw.draw();
    } else {

    }
  }

  //end draw states
  if(useShadowPolygonOffset) {
    glDisable(GL_POLYGON_OFFSET_FILL);
  }

  glDisable(GL_DEPTH_CLAMP);

  if(useShadowDepthLessEqual) {
    glDepthFunc(GL_LESS);
  }
}

void deferredLightBegin() {
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE,GL_ONE);
  glEnable(GL_CULL_FACE);
}

void deferredLightEnd() {
  glDisable(GL_BLEND);
  glBlendFunc(GL_ONE,GL_ZERO);
  glDisable(GL_CULL_FACE);
}

void renderDeferredLight(Light *light) {
  GLuint lightProg=0;

  //
  if(PointLight *pointLight=dynamic_cast<PointLight*>(light)) {
    uniformManager.setUniform1f("u_strength",pointLight->strength);
    uniformManager.setUniform3fv("u_lightCol",1,pointLight->color);
    uniformManager.setUniform3fv("u_lightAtten",1,pointLight->atten);
    uniformManager.setUniform4fv("u_lightPos",1,pointLight->viewPos);

    lightProg = shaderManager.get("shader/deferred/Common.vs","","shader/deferred/Light.fs","","","POINT");
  } else if(SpotLight *spotLight=dynamic_cast<SpotLight*>(light)) {
    uniformManager.setUniform1f("u_strength",spotLight->strength);
    uniformManager.setUniform3fv("u_lightCol",1,spotLight->color);
    uniformManager.setUniform3fv("u_lightAtten",1,spotLight->atten);
    uniformManager.setUniform1f("u_spotExponent",spotLight->exponent);
    uniformManager.setUniform1f("u_spotCutoff",spotLight->cutoff);
    uniformManager.setUniform4fv("u_lightPos",1,spotLight->viewPos);
    uniformManager.setUniform4fv("u_lightDir",1,spotLight->viewDir);

    lightProg = shaderManager.get("shader/deferred/Common.vs","","shader/deferred/Light.fs","","","SPOT");
  } else if(DirLight *dirLight=dynamic_cast<DirLight*>(light)) {
    uniformManager.setUniform3fv("u_lightCol",1,dirLight->color);
    uniformManager.setUniform4fv("u_lightDir",1,dirLight->viewDir);
    uniformManager.setUniform1f("u_strength",dirLight->strength);

    lightProg = shaderManager.get("shader/deferred/Common.vs","","shader/deferred/Light.fs","","","DIRECTIONAL");
  } else {
    return;
  }

  //
  glUseProgram(lightProg);
  uniformManager.apply(lightProg);

  //
  glBindVertexArray(screenVao);
  screenDraw.draw();
}

void copyDeferredDepth() {
  //
  glUseProgram(shaderManager.get("shader/deferred/Common.vs","","shader/deferred/DepthCopy.fs","","",""));

  //states
  glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
  glEnable(GL_DEPTH_TEST);

  //clear
  glClearDepth(1.0f);
  glClear(GL_DEPTH_BUFFER_BIT);

  //render deferred depth texture
  glBindVertexArray(screenVao);
  screenDraw.draw();

  //restore states
  glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
  glDisable(GL_DEPTH_TEST);
}

void renderSsao() {

  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D,textureManager.get2d("texture/noise.dds"));
  glBindSampler(0,0);

  GLuint ambienceProg=shaderManager.get("shader/deferred/Common.vs","",
                                        "shader/deferred/SSAOa.fs",
                                        // "shader/deferred/SSAOL_fp.fs",
                                        "","","");
  glUseProgram(ambienceProg);
  uniformManager.apply(ambienceProg);

  glEnable(GL_BLEND);
 // glBlendFunc(GL_DST_COLOR,GL_ZERO);//(final = texture * fb)
 glBlendFunc(GL_ZERO,GL_SRC_COLOR);//(final = fb * texture)

 //  glBlendFunc(GL_ONE,GL_ONE);
   // glBlendFunc(GL_ONE,GL_ZERO);
     // glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
   //glBlendEquation(GL_FUNC_SUBTRACT);

  //
  glBindVertexArray(screenVao);
  screenDraw.draw();

  glBlendFunc(GL_ONE,GL_ZERO);
  glBlendEquation(GL_FUNC_ADD);
  glDisable(GL_BLEND);
}

void renderScene() {

  //
  checkGlError();
  glViewport(0,0,renderWidth,renderHeight);

  //bind deferred fbo
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER,deferredFbo);


  //clear deferred fbo
  glClearColor(0.0f,0.0f,0.0f,0.0f);
  glClearDepth(1);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  //
  renderGeometry();

  if(useSsao) {
    //renderSsao();
  }
  //unbind fbo
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);

  //bind deferred color tex
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,deferredColorTex);
  glBindSampler(0,0);

  //bind deferred normal tex
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D,deferredNormalTex);
  glBindSampler(1,0);

  //bind deferred depth tex
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D,deferredDepthTex);
  glBindSampler(2,0);

  //copy deferred depth to main frame buffer
  //copyDeferredDepth();

  //bind light fbo
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER,lightFbo);


  //copy deferred depth to light frame buffer
  //copyDeferredDepth();

  //clear
  glClearColor(0.0f,0.0f,0.0f,1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  //begin draw states
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);

  //
  for(auto light : lights) {
    if(!light->on) {
      continue;
    }

    //render shadow volumes
    if(light->shadow) {
      glClearStencil(0);
      glClear(GL_STENCIL_BUFFER_BIT);

      //
      glEnable(GL_STENCIL_TEST);
      glStencilFunc(GL_ALWAYS,0,0xff);

      if(useShadowZPass) {
        glStencilOpSeparate(GL_FRONT,GL_KEEP,GL_KEEP,GL_INCR_WRAP);
        glStencilOpSeparate(GL_BACK,GL_KEEP,GL_KEEP,GL_DECR_WRAP);
      } else {
        glStencilOpSeparate(GL_BACK,GL_KEEP,GL_INCR_WRAP,GL_KEEP);
        glStencilOpSeparate(GL_FRONT,GL_KEEP,GL_DECR_WRAP,GL_KEEP);
      }

      glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);

      //
      renderShadows(light,false,useBackfaceShadow,useShadowZPass,false);

      //restore draw states
      glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);

      //
      glStencilOpSeparate(GL_BACK,GL_KEEP,GL_KEEP,GL_KEEP);
      glStencilFunc(GL_EQUAL,0,0xff);
    }

    //render light
    deferredLightBegin();
    renderDeferredLight(light);
    deferredLightEnd();

    //restore states
    if(light->shadow) {
      glDisable(GL_STENCIL_TEST);
    }
  }


  //
  deferredLightBegin();
  renderDeferredEmissive();

  //if(!useSsao) {
    renderDeferredAmbient();
  //}

  deferredLightEnd();


  //end draw states
  glDepthMask(GL_TRUE);
  glDisable(GL_DEPTH_TEST);

  //
  if(useSsao) {
  //  renderSsao();
  }

  //
  renderSky();

  //unbind fbo
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);

  //
  //bind light color tex
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,lightColorTex);
  glBindSampler(0,0);

  //

  /*
  glBindFramebuffer(GL_FRAMEBUFFER,aaFbo);
  glUseProgram(shaderManager.get("shader/deferred/Common.vs","","shader/deferred/ColorCopy.fs"));

  //
  glBindVertexArray(screenVao);
  screenDraw.draw();


  glBindFramebuffer(GL_FRAMEBUFFER,0);

  //bind aa color tex
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,aaColorTex);
  glBindSampler(0,0);
  */

  //
  {
    //begin draw states
    glDepthMask(GL_FALSE);
    glEnable(GL_CULL_FACE);

    if(useAntiAliasing==1) {
      GLuint ambienceProg=shaderManager.get("shader/deferred/Common.vs","","shader/deferred/FXAA.fs","","","");
      glUseProgram(ambienceProg);
      uniformManager.apply(ambienceProg);

      //
      glBindVertexArray(screenVao);
      screenDraw.draw();
    } else if(useAntiAliasing==2) {
      GLuint ambienceProg=shaderManager.get("shader/deferred/Common.vs","","shader/deferred/FXAAspades.fs","","","");
      glUseProgram(ambienceProg);
      uniformManager.apply(ambienceProg);

      //
      glBindVertexArray(screenVao);
      screenDraw.draw();
    } else { //todo 2
      glUseProgram(shaderManager.get("shader/deferred/Common.vs","","shader/deferred/ColorCopy.fs","","",""));

      //
      glBindVertexArray(screenVao);
      screenDraw.draw();
    }


    //end draw states
    glDepthMask(GL_TRUE);
    glDisable(GL_CULL_FACE);
  }

  if(useSsao) {
    renderSsao();
  }
  //
  //deferred test draw states
  //glDepthMask(GL_FALSE);
  //glEnable(GL_CULL_FACE);

  //renderDeferredColorTest();
  //renderDeferredNormalTest();

  //deferred test restore draw states
  //glDepthMask(GL_TRUE);
  //glDisable(GL_CULL_FACE);

  //shadow volume test
  glEnable(GL_DEPTH_TEST);
 // glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_DST_COLOR,GL_SRC_COLOR);
  if(useShadowDebug) {
    for(auto light : lights) {
      if(!light->on || !light->shadow) {
        continue;
      }

      renderShadows(light,true,useBackfaceShadow,useShadowZPass,false);
    }
  }

  //end drawstates
  glBlendFunc(GL_ONE,GL_ZERO);
  glDisable(GL_BLEND);
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);

  //
  renderTextMenu();
}


void initResources() {
  GLuint vaultVao,vaultShadowVao=0;
  GeometryDraw vaultDraw,vaultShadowDraw;


  //screen geometry
  screenVao = geometryManager.getVao("geometry/Screen.geom","geometry/Default.layout");
  screenDraw = geometryManager.getDraw("geometry/Screen.geom","default");


  //sky geometry
  skyVao = geometryManager.getVao("geometry/Skybox.geom","geometry/Default.layout");
  skyDraw = geometryManager.getDraw("geometry/Skybox.geom","default");

  //vault geometry
  //vaultVao = geometryManager.getVao("geometry/sibenik2.geom","geometry/Default.layout");
  //vaultDraw = geometryManager.getDraw("geometry/sibenik2.geom","default");

  vaultVao = geometryManager.getVao("geometry/vault.geom","geometry/Default.layout");
  vaultDraw = geometryManager.getDraw("geometry/vault.geom","default");

  //vault shadow geometry
  //vaultShadowVao = geometryManager.getVao("geometry/sibenik2_shadow.geom","geometry/Default.layout");
  //vaultShadowDraw = geometryManager.getDraw("geometry/sibenik2_shadow.geom","default");

  vaultShadowVao = geometryManager.getVao("geometry/vault_shadow.geom","geometry/Default.layout");
  vaultShadowDraw = geometryManager.getDraw("geometry/vault_shadow.geom","default");


  // myFont=new MyFont("font/DroidSerif-Bold.ttf",16,1024);


  Object *planeObj=new Object();
  planeObj->vao=geometryManager.getVao("geometry/Plane.geom","geometry/Default.layout");
  planeObj->draw=geometryManager.getDraw("geometry/Plane.geom","default");
  planeObj->color[0]=0.6f;
  planeObj->color[1]=0.9f;
  planeObj->color[2]=0.75f;
  mat4::scale(planeObj->modelMat,30.0f,30.0f,30.0f, true);
 // objects.push_back(planeObj);


  Object *vaultObj=new Object();
  vaultObj->vao=vaultVao;
  vaultObj->draw=vaultDraw;
  vaultObj->shadowVao=vaultShadowVao;
  vaultObj->shadowDraw=vaultShadowDraw;
  vaultObj->shadowSmoothBias=0.15f;
  mat4::translate(vaultObj->modelMat,5.0f,15.0f,5.0f, true);
  mat4::scale(vaultObj->modelMat,2.0f,2.0f,2.0f, true);
  //objects.push_back(vaultObj);


  buildingObj->vao=geometryManager.getVao("geometry/sibenik2.geom","geometry/Default.layout");
  buildingObj->draw=geometryManager.getDraw("geometry/sibenik2.geom","default");
  buildingObj->shadowVao=geometryManager.getVao("geometry/sibenik2_shadow.geom","geometry/Default.layout");;
  buildingObj->shadowDraw=geometryManager.getDraw("geometry/sibenik2_shadow.geom","default");

  mat4::translate(buildingObj->modelMat,0.0f,25.0f,0.0f, true);
  mat4::scale(buildingObj->modelMat,3.0f,3.0f,3.0f, true);
  objects.push_back(buildingObj);

  //headObj->vao=geometryManager.getVao("geometry/head.geom","geometry/Default.layout");
  //headObj->draw=geometryManager.getDraw("geometry/head.geom","default");
  //headObj->shadowVao=geometryManager.getVao("geometry/head_shadow.geom","geometry/Default.layout");
  //headObj->shadowDraw=geometryManager.getDraw("geometry/head_shadow.geom","default");
  headObj->shadowSmoothBias=0.001f;
  mat4::translate(headObj->modelMat,0.0f,0.0f,-10.0f, true);
  //objects.push_back(headObj);

  templeObj->visible=false;
  templeObj->vao=geometryManager.getVao("geometry/sponza.geom","geometry/Default.layout");
  templeObj->draw=geometryManager.getDraw("geometry/sponza.geom","default");
  templeObj->shadowVao=geometryManager.getVao("geometry/sponza_shadow.geom","geometry/Default.layout");;
  templeObj->shadowDraw=geometryManager.getDraw("geometry/sponza_shadow.geom","default");
  objects.push_back(templeObj);


  cubeObj->vao=geometryManager.getVao("geometry/Cube.geom","geometry/Default.layout");;
  cubeObj->draw=geometryManager.getDraw("geometry/Cube.geom","default");
  cubeObj->shadowVao=geometryManager.getVao("geometry/Cube_shadow.geom","geometry/Default.layout");;;
  cubeObj->shadowDraw=geometryManager.getDraw("geometry/Cube_shadow.geom","default");
  cubeObj->shadowSmoothBias=0.15f;
  objects.push_back(cubeObj);


  light1->pos[0]= 1.3f;
  light1->pos[1]=0.0f;
  light1->pos[2]=26.0f;
  light1->atten[0]=0.8f;
  light1->atten[1]=0.02f;
  light1->atten[2]=0.002f;
  light1->shadow=true;
  lights.push_back(light1);

  light2->dir[0]=0.7f;
  light2->dir[1]=-0.5f;
  light2->dir[2]=0.3f;
  light2->strength=0.0f;
  light2->shadow=true;
  light2->on=false;
  lights.push_back(light2);

  {
    float space=2.0f;
    int pos=0;
    //0
    {
      float col[3]={ 0.93f,0.81f,0.81f };
      colKfr.add((float)(pos)*space,col);
      pos+=15;
    }

    //1
    {
      float col[3]={ 187.0f/255.0f,135.0f/255.0f,111.0f/255.0f };
      colKfr.add((float)(pos++)*space,col);
    }

    //2
    {
      float col[3]={ 116.0f/255.0f,69.0f/255.0f,61.0f/255.0f };
      colKfr.add((float)(pos++)*space,col);
    }

    //3
    {
      float col[3]={ 92.0f/255.0f,57.0f/255.0f,55.0f/255.0f };
      colKfr.add((float)(pos++)*space,col);
    }

    //4
    {
      float col[3]={ 221.0f/255.0f,183.0f/255.0f,160.0f/255.0f };
      colKfr.add((float)(pos)*space,col);
      pos+=10;
    }

    //5
    {
      float col[3]={ 166.0f/255.0f,115.0f/255.0f,88.0f/255.0f };
      colKfr.add((float)(pos++)*space,col);
    }

    //6
    {
      float col[3]={ 173.0f/255.0f,100.0f/255.0f,83.0f/255.0f };
      colKfr.add((float)(pos++)*space,col);
    }

    //7
    {
      float col[3]={ 242.0f/255.0f,214.0f/255.0f,203.0f/255.0f };
      colKfr.add((float)(pos)*space,col);
      pos+=10;
    }

    //8
    {
      float col[3]={ 206.0f/255.0f,150.0f/255.0f,125.0f/255.0f };
      colKfr.add((float)(pos++)*space,col);
    }

    //9
    {
      float col[3]={ 170.0f/255.0f,129.0f/255.0f,111.0f/255.0f };
      colKfr.add((float)(pos++)*space,col);
    }

    //0
    {

      float col[3]={ 0.93f,0.81f,0.81f };
      colKfr.add((float)(pos++)*space,col);
    }

    {
      float aa[3]={ 0.0f,0.0f,-15.0f };
      float bb[3]={ 0.0f,0.0f,10.0f };
      lightPosKfr.add(0.0f,aa);
      lightPosKfr.add(10.0f,bb);
      lightPosKfr.add(20.0f,aa);

    }
  }
}


void printGLExts() {
  int extNum;
  glGetIntegerv(GL_NUM_EXTENSIONS,&extNum);

  for(int i=0;i<extNum;i++) {
    std::cerr << glGetStringi(GL_EXTENSIONS,i) << std::endl;
  }
}

void printVers() {
  std::cerr << "GL " << glGetString(GL_VERSION) << std::endl;
  std::cerr << "Shader version " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  std::cerr << std::endl;
}

void init() {
  glGenFramebuffers(1,&deferredFbo);
  glGenTextures(1,&deferredColorTex);
  glGenTextures(1,&deferredNormalTex);
  glGenTextures(1,&deferredDepthTex);

  glGenFramebuffers(1,&lightFbo);
  glGenRenderbuffers(1,&lightDepthStencilBuf);
  glGenTextures(1,&lightColorTex);


  screenFont=createScreenFont("font/DroidSerif-Regular.ttf",13,FONT_MAX_CHARS);

}
void uninit() {
  glDeleteFramebuffers(1,&deferredFbo);
  glDeleteTextures(1,&deferredColorTex);
  glDeleteTextures(1,&deferredNormalTex);
  glDeleteTextures(1,&deferredDepthTex);

  glDeleteFramebuffers(1,&lightFbo);
  glDeleteRenderbuffers(1,&lightDepthStencilBuf);
  glDeleteTextures(1,&lightColorTex);


}

bool setup() {
  checkGlError("Setup begin");
  int width=renderWidth;
  int height=renderHeight;
  int colorIn=0;
  int normalIn=1;

  if(width<=0) {
    width=128;
  }

  if(height<=0) {
    height=128;
  }

  //
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER,deferredFbo);

  //deferred color
  glBindTexture(GL_TEXTURE_2D,deferredColorTex);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_FLOAT,0);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0+colorIn,GL_TEXTURE_2D,deferredColorTex,0);

  //deferred normal
  GLenum normalInnerFormat;
  GLenum normalType;

  normalInnerFormat=GL_RGBA32F;
  normalType=GL_FLOAT;

  // normalInnerFormat=GL_RGBA16F;
  // normalType=GL_HALF_FLOAT;

  glBindTexture(GL_TEXTURE_2D,deferredNormalTex);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D,0,normalInnerFormat,width,height,0,GL_RGBA,normalType,0);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0+normalIn,GL_TEXTURE_2D,deferredNormalTex,0);

  //deferred depth
  glBindTexture(GL_TEXTURE_2D,deferredDepthTex);
  glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH24_STENCIL8,
    width,height,0,GL_DEPTH_STENCIL,
    GL_UNSIGNED_INT_24_8,0);

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_FUNC,GL_LEQUAL);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,GL_TEXTURE_2D,deferredDepthTex,0);

  //
  GLenum drawBufs[]={ GL_COLOR_ATTACHMENT0+(GLenum)colorIn,GL_COLOR_ATTACHMENT0+(GLenum)normalIn };
  glDrawBuffers(2,drawBufs);

  //check deferredFbo status
  if(!checkFboError("deferred fbo")) {
    return false;
  }

  //
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER,lightFbo);

  //light color
  glBindTexture(GL_TEXTURE_2D,lightColorTex);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_FLOAT,0);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,lightColorTex,0);

  //light depth stencil buffer
  glBindRenderbuffer(GL_RENDERBUFFER,lightDepthStencilBuf);
  glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH24_STENCIL8,width,height);
  checkGlError("Bb3");
  //glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,GL_RENDERBUFFER,lightDepthStencilBuf);
  //glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,deferredDepthTex,0);
  glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,GL_TEXTURE_2D,deferredDepthTex,0);

  //check lightFbo status
  if(!checkFboError("light fbo")) {
    return false;
  }


  checkGlError("Setup end");
  return true;
}

void handleInput() {
  updateMouseSmooth((double)window_mouse_x(),(double)window_mouse_y(),
                    mouseHistory,MOUSE_HISTORY_SIZE, 0.2,
                    &mouseSmoothX,&mouseSmoothY);

  mouseLook=window_input_down(WINDOW_MOUSE_LEFT);
  camera.setForward(window_input_down(WINDOW_KEY_W));
  camera.setBackward(window_input_down(WINDOW_KEY_S));
  camera.setLeftward(window_input_down(WINDOW_KEY_A));
  camera.setRightward(window_input_down(WINDOW_KEY_D));

  if(window_input_press(WINDOW_KEY_1)) {
    light1->on=!light1->on;
  }

  if(window_input_press(WINDOW_KEY_8)) {
    light2->on=!light2->on;
  }

  if(window_input_press(WINDOW_KEY_2)) {
    useShadowDebug=!useShadowDebug;
  }

  if(window_input_press(WINDOW_KEY_4)) {
    useShadowSmooth=!useShadowSmooth;
  }

  if(window_input_press(WINDOW_KEY_3)) {
    useBackfaceShadow=!useBackfaceShadow;
  }

  if(window_input_press(WINDOW_KEY_5)) {
    useShadowPolygonOffset=!useShadowPolygonOffset;
  }

  if(window_input_press(WINDOW_KEY_6)) {
    useShadowZPass=!useShadowZPass;
  }

  if(window_input_press(WINDOW_KEY_7)) {
    useShadowDepthLessEqual=!useShadowDepthLessEqual;
  }

  if(window_input_press(WINDOW_KEY_9)) {
    useSsao=!useSsao;
  }

  if(window_input_press(WINDOW_KEY_P)) {
    const float *pos=camera.getPos();
    float yaw=camera.getYaw();
    float pitch=camera.getPitch();
    std::cerr << "camera " << pos[0] << "," << pos[1] << ","
              << pos[2] << "; " << yaw << "," << pitch << std::endl;
  }

  if(window_input_press(WINDOW_KEY_M)) {
    buildingObj->visible=!buildingObj->visible;
    templeObj->visible=!templeObj->visible;
  }

  if(window_input_press(WINDOW_KEY_U)) {
    useAntiAliasing++;

    if(useAntiAliasing>2) {
      useAntiAliasing=0;
    }
  }

  if(window_input_down(WINDOW_KEY_T)) {
    for(int i=0;i<3;i++) {
      light1->pos[i]=camera.getPos()[i];
    }
  }

  if(window_input_press(WINDOW_KEY_F5) || window_input_press(WINDOW_KEY_G)) {
    uniformManager.clearPrograms();
    shaderManager.refresh();
    textureManager.refresh();
    geometryManager.refresh();
  }


  if(window_input_press(WINDOW_KEY_J)) {
    std::string geomDefs;
    std::string debugFs;

     if(useBackfaceShadow) {
      geomDefs+="BACKFACE;";
    }

     if(useShadowZPass) {
      geomDefs+="ZPASS;";
    }

    if(useShadowDebug) {
      geomDefs+="DEBUG;";
    }

    geomDefs+="POINT;";

    std::cerr << std::endl << shaderManager.getShaderSource(GL_GEOMETRY_SHADER,"shader/ShadowVol.gs",geomDefs) << std::endl;;
  }
}


int main(int argc,char* argv[]) {
  if(!window_create("Demo",1024,600)) {
    return 1;
  }

  // if(!window_context(3,3) &&
  //    !window_context(3,2) &&
  //    !window_context(3,1) &&
  //    !window_context(3,0)) {
  //   return 2;
  // }

  //
  if(ogl_LoadFunctions() == ogl_LOAD_FAILED) {
    std::cerr << "ogl_LOAD_FAILED\n";
    return 15;
  }

  //
  printVers();
  // printGLExts();


  {
    std::set<std::string> extensions;
    int extNum;
    glGetIntegerv(GL_NUM_EXTENSIONS,&extNum);

    for(int i=0;i<extNum;i++) {
      extensions.insert((const char*)glGetStringi(GL_EXTENSIONS,i));
    }

    geometryShaderSupport=extensions.find("GL_ARB_geometry_shader4")!=extensions.end();
  }

  //
  mouseSmoothX=0.0;
  mouseSmoothY=0.0;
  memset(mouseHistory,0,sizeof(mouseHistory));

  //
  init();
  initResources();

  //
  camera.setYaw(5.9f);
  camera.setPitch(-0.42f);
  camera.setPosition(-4.4f,22.4f,16.5f);


  setup();

  Timer timer;
  timer.start();

  //main loop
  while(window_update() && !window_input_press(WINDOW_KEY_ESCAPE)) {
    // curTime=timer2.elapsed();
    curTime=timer.stop();
    static double lastTime=curTime;
    frame=curTime;
    //
    if(window_iconified()) {
      window_swap_buffers();
      lastTime=curTime;
      continue;
    }
    renderWidth=window_client_width();
    renderHeight=window_client_height();

    if(window_sized() && window_client_width()>0 && window_client_height()>0) {
      std::cerr << "deferred setup "<< renderWidth << ", " <<renderHeight << std::endl;
      setup();
    }

    handleInput();


    //



    //
    deltaTime=curTime-lastTime;

    if(mouseLook  && window_focused() && !window_iconified()) {
      window_lock_cursor(true);
      camera.setPitching((float)mouseSmoothY);
      camera.setYawing((float)mouseSmoothX);
    } else {
      window_lock_cursor(false);
    }


    camera.update((float)deltaTime);

    update();
    renderScene();

    //

    window_swap_buffers();

    lastTime=curTime;
  }

  //cleanup

  uninit();

  window_destroy();
  return 0;
}
