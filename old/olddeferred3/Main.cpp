#include "Main.h"

int deferredWidth=0,deferredHeight=0;
bool geometryShaderSupport=false;
ShaderManager shaderManager;
TextureManager textureManager;
GeometryManager geometryManager;
UniformManager uniformManager;
Deferred *deferred=0;

GLuint screenVao=0,planeVao=0,cubeVao,cubeShadowVao=0,skyVao=0,vaultVao,vaultShadowVao=0;
GeometryDraw screenDraw,planeDraw,cubeDraw,cubeShadowDraw,skyDraw,vaultDraw,vaultShadowDraw;

FileMon fileMon;

MouseSmooth mouseSmooth;
bool mouseLook=false;
Camera camera;

bool usePointLight=false;
bool useSmoothShadow=false;
bool useBackfaceShadow=true;
bool useShadowDebug=false;
bool useShadowPolygonOffset=false;
bool useShadowZPass=false;
bool useShadowDepthLessEqual=true;
bool test=false;
float angle=0.0f;
double frame=0.0;

float lightDir[3]={ 0.5f,-1.0f,0.8f };
float lightPos[3]={ 5.0f-lightDir[0]*10.0f+2.0f,15.0f-lightDir[1]*20.0f,5.0f-lightDir[2]*10.0f };

Keyframer<float,3> colKfr;


bool checkError(const char *n=0) {
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
          std::cout << n  <<  ": ";
        }

        std::cout << "gl error : " << errors[i] << std::endl;
        return true;
      }
    }
  }

  return false;
}

MyFont *myFont=0;



void geometryBegin() {
  //bind fbo
  glBindFramebuffer(GL_FRAMEBUFFER,deferred->getFbo());

  //states
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);


  //clear
  glClearColor(0.0f,0.0f,0.0f,0.0f);
  glClearDepth(1);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}

void geometryEnd() {
  //restore states
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  //bind fbo
  glBindFramebuffer(GL_FRAMEBUFFER,0);
}

void deferredBegin() {
  //
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,deferred->getColorTex());
  glBindSampler(0,0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D,deferred->getNormalTex());
  glBindSampler(1,0);

  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D,deferred->getDepthTex());
  glBindSampler(2,0);

  //
  deferred->renderDepth(2);

  //
  glClearColor(0.0f,0.0f,0.0f,1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

void deferredEnd() {
}

void shadowVolumeBegin(bool zpass,bool polyoffset, bool debug) {
  //states
  if(!debug) {
    glEnable(GL_STENCIL_TEST);
    glDepthMask(GL_FALSE);
    glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
  }

  if(polyoffset) {
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f,100.0f);
  }

  if(useShadowDepthLessEqual) {
    glDepthFunc(GL_LEQUAL);
  }

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_DEPTH_CLAMP);

  if(!debug) {
    if(zpass) {
      glStencilOpSeparate(GL_FRONT,GL_KEEP,GL_KEEP,GL_INCR_WRAP);
      glStencilOpSeparate(GL_BACK,GL_KEEP,GL_KEEP,GL_DECR_WRAP);
    } else {
      glStencilOpSeparate(GL_BACK,GL_KEEP,GL_INCR_WRAP,GL_KEEP);
      glStencilOpSeparate(GL_FRONT,GL_KEEP,GL_DECR_WRAP,GL_KEEP);
    }

    //glDepthRange(0.0,1.0);
    glClearStencil(0);
    glClear(GL_STENCIL_BUFFER_BIT);
  }

  if(debug) {
   // glDepthFunc(GL_LEQUAL);
  }


}

void shadowVolumeEnd(bool polyoffset,bool debug) {
  //restore states
  if(!debug) {
    glDisable(GL_STENCIL_TEST);
    glDepthMask(GL_TRUE);
    glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
    glDisable(GL_POLYGON_OFFSET_FILL);
  }

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_DEPTH_CLAMP);

  if(polyoffset) {
    glDisable(GL_POLYGON_OFFSET_FILL);
  }

  if(useShadowDepthLessEqual) {
    glDepthFunc(GL_LESS);

  }
  if(debug) {
    //glDepthFunc(GL_LESS);
  }
}

void lightBegin(bool shadowVolume) {
  //states
  glDepthMask(GL_FALSE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_ONE,GL_ONE);
  glEnable(GL_CULL_FACE);

  if(shadowVolume) {
    glEnable(GL_STENCIL_TEST);
    glStencilOpSeparate(GL_BACK,GL_KEEP,GL_KEEP,GL_KEEP);
    glStencilFunc(GL_EQUAL,0,0xff);
  }
}

void lightEnd(bool shadowVolume) {
  //restore states
  glDepthMask(GL_TRUE);
  glDisable(GL_BLEND);
  glBlendFunc(GL_ONE,GL_ZERO);
  glDisable(GL_CULL_FACE);

  if(shadowVolume) {
    glDisable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS,0,0xff);
  }
}

void skyBegin() {
  //states
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(GL_FALSE);
  glDepthRange(1.0,1.0);
  glDepthFunc(GL_LEQUAL);
}

void skyEnd() {
  //restore states
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glDepthMask(GL_TRUE);
  glDepthRange(0.0,1.0);
  glDepthFunc(GL_LESS);
}



void render(int clientWidth,int clientHeight) {

  checkError();
  glViewport(0,0,deferredWidth,deferredHeight);
  float zNear = 1.0f;
  float zFar = 100.0f;

  float projMat[16],invProjMat[16];
  mat4::perspective(projMat,(float)M_PI_4,(float)clientWidth/(float)clientHeight,1.0f,100.0f);
  mat4::inverse(invProjMat,projMat);

  const float *viewMat=camera.getViewMat();
  float invViewMat[16],viewProjMat[16];
  mat4::inverse(invViewMat,viewMat);
  mat4::multiply(viewProjMat,projMat,viewMat);

  //for(int i=0;i<16;i++) { std::cout << viewProjMat[i] << ","; }
  //std::cout << std::endl << std::endl;

  if(1){
    //float delta=-0.5f;
    //float pz=0.2f;
    //float epsilon = -2.0f * zFar * zNear * delta / ((zFar + zNear) * pz * (pz + delta));
    //projMat[3][3]*=1.0f + epsilon;
  }

  uniformManager.setUniformMatrix4fv("u_projMat",1,true,projMat);
  uniformManager.setUniformMatrix4fv("u_invProjMat",1,true,invProjMat);
  uniformManager.setUniformMatrix4fv("u_invViewMat",1,true,invViewMat);
  uniformManager.setUniformMatrix4fv("u_viewProjMat",1,true,viewProjMat);


  uniformManager.setUniform1f("u_zNear",zNear);
  uniformManager.setUniform1f("u_zFar",zFar);

  //
  geometryBegin();


  //plane
  {
    //matrices

    float modelMat[16],modelViewMat[16],normalMat[9],modelViewProjMat[16];
    mat4::scale(modelMat,30.0f,30.0f,30.0f);
    mat4::multiply(modelViewMat,viewMat,modelMat);
    mat4::normal3(normalMat,modelViewMat);
    mat4::multiply(modelViewProjMat,projMat,modelViewMat);




    uniformManager.setUniformMatrix4fv("u_modelViewProjMat",1,true,modelViewProjMat);
    uniformManager.setUniformMatrix4fv("u_modelViewMat",1,true,modelViewMat);
    uniformManager.setUniformMatrix3fv("u_normalMat",1,true,normalMat);

    //
    uniformManager.setUniform3f("u_col",0.8f,0.85f,1.0f);
    uniformManager.setUniform1f("u_shininess",0.3f);
    uniformManager.setUniform1f("u_emissive",0.0f);
    uniformManager.setUniform1i("u_unlit",0);
    uniformManager.setUniform1f("u_bumpScale",0.04f);
    uniformManager.setUniform1f("u_bumpBias",-0.02f);
    uniformManager.setUniform2f("u_texScale",8.0f,8.0f);


    GLuint prog = shaderManager.get("shader/geometry/Parallax.vs","","shader/geometry/Parallax.fs");
    glUseProgram(prog);
    uniformManager.apply(prog);

    //
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,textureManager.get2d("texture/rocks.dds"));


    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D,textureManager.get2d("texture/rocks_NM_height.dds"));

    glBindVertexArray(planeVao);
    planeDraw.draw();
  }

  //box
  float cube_modelViewMat[16],cube_normalMat[9];
  {
    //matrices



    float modelMat[16],modelViewProjMat[16];
    mat4::identity(modelMat);
    mat4::translate(modelMat,0.0f,6.0f,0.0f);
    mat4::multiplyRotateY(modelMat,angle);
    mat4::multiplyScale(modelMat,2.0f,2.0f,2.0f);
    mat4::multiply(cube_modelViewMat,viewMat,modelMat);

    mat4::normal3(cube_normalMat,cube_modelViewMat);
    mat4::multiply(modelViewProjMat,projMat,cube_modelViewMat);

    uniformManager.setUniformMatrix4fv("u_modelViewProjMat",1,true,modelViewProjMat);
    uniformManager.setUniformMatrix4fv("u_modelViewMat",1,true,cube_modelViewMat);
    uniformManager.setUniformMatrix3fv("u_normalMat",1,true,cube_normalMat);


    //
    float col[3];
    colKfr.calc((float)frame,true,col);
    uniformManager.setUniform3fv("u_col",1,col);

    //uniformManager.setUniform3f("u_col",0.93f,0.81f,0.81f);
    uniformManager.setUniform1f("u_shininess",0.3f);
    uniformManager.setUniform1f("u_emissive",0.0f);
    uniformManager.setUniform1i("u_unlit",0);


    GLuint prog = shaderManager.get("shader/geometry/Mirror.vs","","shader/geometry/Mirror.fs");
    glUseProgram(prog);
    uniformManager.apply(prog);

    //
    glBindVertexArray(cubeVao);
    cubeDraw.draw();
  }

  //vault
  float vault_modelViewMat[16],vault_normalMat[9];
  {
    //matrices



    //
    float modelMat[16],modelViewProjMat[16];
    mat4::translate(modelMat,5.0f,15.0f,5.0f);
    mat4::multiplyScale(modelMat,2.0f,2.0f,2.0f);
    mat4::multiply(vault_modelViewMat,viewMat,modelMat);
    mat4::normal3(vault_normalMat,vault_modelViewMat);
    mat4::multiply(modelViewProjMat,projMat,vault_modelViewMat);

    uniformManager.setUniformMatrix4fv("u_modelViewProjMat",1,true,modelViewProjMat);
    uniformManager.setUniformMatrix4fv("u_modelViewMat",1,true,vault_modelViewMat);
    uniformManager.setUniformMatrix3fv("u_normalMat",1,true,vault_normalMat);


    //
    uniformManager.setUniform3f("u_col",1.0f,1.0f,1.0f);
    uniformManager.setUniform1f("u_shininess",0.3f);
    uniformManager.setUniform1f("u_emissive",0.0f);
    uniformManager.setUniform1i("u_unlit",0);


    GLuint prog = shaderManager.get("shader/geometry/Simple.vs","","shader/geometry/Simple.fs");
    glUseProgram(prog);
    uniformManager.apply(prog);

    //
    glBindVertexArray(vaultVao);
    vaultDraw.draw();
  }

  //
  geometryEnd();

  glViewport(0,0,clientWidth,clientHeight);
  deferredBegin();


  //

  float light_viewDir[4],light_viewPos[4];
  {
    //


    uniformManager.setUniform1f("u_strength",0.1f);

    uniformManager.setUniform3f("u_lightCol",1.0f,1.0f,1.0f);

    uniformManager.setUniform1i("u_robust",0);


    if(usePointLight) {
      uniformManager.setUniform3f("u_lightAtten",0.5f,0.001f,0.001f);

      //vec3::multiplyScalar(pos2,pos2,15.0f);
      mat4::multiplyVector(light_viewPos,viewMat,lightPos,1.0f);

      uniformManager.setUniform4fv("u_lightPos",1,light_viewPos);
    } else {

      float lightDir2[3];
      vec3::normal(lightDir2,lightDir);
      mat4::multiplyVector(light_viewDir,viewMat,lightDir2,0.0f);
      uniformManager.setUniform4fv("u_lightDir",1,light_viewDir);
    }

  
    //
    shadowVolumeBegin(useShadowZPass,useShadowPolygonOffset,false);
    uniformManager.setUniform1i("u_backfaced",useBackfaceShadow?1:0);
    uniformManager.setUniform1i("u_zPass",useShadowZPass?1:0);

    if(geometryShaderSupport) {
      GLuint prog;

      if(usePointLight) {
        if(useSmoothShadow) {
          prog = shaderManager.get("shader/ShadowVol.vs","shader/PointShadowVolSmooth.gs","");
        } else {
          prog = shaderManager.get("shader/ShadowVol.vs","shader/PointShadowVol.gs","");
        }
      } else {
        if(useSmoothShadow) {
          prog = shaderManager.get("shader/ShadowVolSmooth.vs","shader/DirShadowVolSmooth.gs","");
        } else {
          prog = shaderManager.get("shader/ShadowVol.vs","shader/DirShadowVol.gs","");
        }
      }

      //
      glUseProgram(prog);

      //
      uniformManager.setUniformMatrix4fv("u_modelViewMat",1,true,cube_modelViewMat);
      uniformManager.setUniformMatrix3fv("u_normalMat",1,true,cube_normalMat);
      uniformManager.setUniform1f("u_smoothBias",0.001f);
      uniformManager.apply(prog);

      glBindVertexArray(cubeShadowVao);
      cubeShadowDraw.draw();

      //
      uniformManager.setUniformMatrix4fv("u_modelViewMat",1,true,vault_modelViewMat);
      uniformManager.setUniformMatrix3fv("u_normalMat",1,true,vault_normalMat);
      uniformManager.setUniform1f("u_smoothBias",0.15f);
      uniformManager.apply(prog);

      glBindVertexArray(vaultShadowVao);
      vaultShadowDraw.draw();

      //

    } else {
    }

    shadowVolumeEnd(useShadowPolygonOffset,false);

    //
    lightBegin(true);

    //
    GLuint prog;

    if(usePointLight) {
      prog = shaderManager.get("shader/deferred/Common.vs","","shader/deferred/PointLight.fs");
    } else {
      prog = shaderManager.get("shader/deferred/Common.vs","","shader/deferred/DirLight.fs");
    }

    glUseProgram(prog);
    uniformManager.apply(prog);

    //
    glBindVertexArray(screenVao);
    screenDraw.draw();

    lightEnd(true);

  }


  //
  lightBegin(false);

  //emissive
  {
    glUseProgram(shaderManager.get("shader/deferred/Common.vs","","shader/deferred/Emissive.fs"));
    glBindVertexArray(screenVao);
    screenDraw.draw();
  }

  //ambience
  {
    uniformManager.setUniform1f("u_ambience",0.1f);
    GLuint prog = shaderManager.get("shader/deferred/Common.vs","","shader/deferred/Ambient.fs");
    glUseProgram(prog);
    uniformManager.apply(prog);


    glBindVertexArray(screenVao);
    screenDraw.draw();
  }
  //
  lightEnd(false);

  //tests
  {
    glDepthMask(GL_FALSE);
    glEnable(GL_CULL_FACE);

    //colors
    if(0) {
      glUseProgram(shaderManager.get("shader/deferred/Common.vs","","shader/deferred/Colors.fs"));
      glBindVertexArray(screenVao);
      screenDraw.draw();
    }

    //normals
    if(0) {
      glUseProgram(shaderManager.get("shader/deferred/Common.vs","","shader/deferred/Normals.fs"));
      glBindVertexArray(screenVao);
      screenDraw.draw();
    }

    //mirror
    if(test) {
      GLuint prog = shaderManager.get("shader/deferred/Common.vs","","shader/deferred/Mirror.fs");
      glUseProgram(prog);
      uniformManager.apply(prog);

      //
      glActiveTexture(GL_TEXTURE3);
      glBindTexture(GL_TEXTURE_CUBE_MAP,textureManager.getCube("texture/blue_light.cube"));

      //
      glBindVertexArray(screenVao);
      screenDraw.draw();
    }

    glDepthMask(GL_TRUE);
    glDisable(GL_CULL_FACE);

    //



    //

    if(useShadowDebug) {
      shadowVolumeBegin(useShadowZPass,useShadowPolygonOffset,true);
      uniformManager.setUniform1i("u_backfaced",useBackfaceShadow?1:0);
      uniformManager.setUniform1i("u_zPass",useShadowZPass?1:0);
      
      if(geometryShaderSupport) {
        GLuint prog;

        if(usePointLight) {
          uniformManager.setUniform4fv("u_lightPos",1,light_viewPos);

          if(useSmoothShadow) {
            prog = shaderManager.get("shader/ShadowVol.vs","shader/PointShadowVolSmoothDebug.gs","shader/White.fs");
          } else {
            prog = shaderManager.get("shader/ShadowVol.vs","shader/PointShadowVolDebug.gs","shader/ShadowVolDebug.fs");
          }
        } else {
          uniformManager.setUniform4fv("u_lightDir",1,light_viewDir);

          if(useSmoothShadow) {
            prog = shaderManager.get("shader/ShadowVolSmooth.vs","shader/DirShadowVolSmoothDebug.gs","shader/White.fs");
          } else {
            prog = shaderManager.get("shader/ShadowVolSmooth.vs","shader/DirShadowVolDebug.gs","shader/ShadowVolDebug.fs");
          }
        }

        glUseProgram(prog);

        //
        uniformManager.setUniformMatrix4fv("u_modelViewMat",1,true,cube_modelViewMat);
        uniformManager.setUniformMatrix3fv("u_normalMat",1,true,cube_normalMat);
        uniformManager.setUniform1f("u_smoothBias",0.001f);

        uniformManager.apply(prog);
        glBindVertexArray(cubeShadowVao);
        cubeShadowDraw.draw();

        //
        uniformManager.setUniformMatrix4fv("u_modelViewMat",1,true,vault_modelViewMat);
        uniformManager.setUniformMatrix3fv("u_normalMat",1,true,vault_normalMat);
        uniformManager.setUniform1f("u_smoothBias",0.15f);

        uniformManager.apply(prog);
        glBindVertexArray(vaultShadowVao);
        vaultShadowDraw.draw();


      } else {
      }

      //
      shadowVolumeEnd(useShadowPolygonOffset,true);
    }
  }
  //
  deferredEnd();



  //sky
  skyBegin();
  {
    //

    //
    GLuint prog = shaderManager.get("shader/Skybox.vs","","shader/Skybox.fs");
    glUseProgram(prog);
    uniformManager.apply(prog);

    //
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_CUBE_MAP,textureManager.getCube("texture/blue_light.cube"));

    //
    glBindVertexArray(skyVao);
    skyDraw.draw();
  }
  skyEnd();


  //
  {

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_ALWAYS);
    glDepthMask(GL_FALSE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE,GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_CULL_FACE);

    //
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D,myFont->texture);

    //
    float projMat2[16];
    mat4::ortho2d(projMat2,0.0f,(float)clientWidth,(float)clientHeight,0.0f);
    uniformManager.setUniformMatrix4fv("u_projMat",1,true,projMat2);

    //
    GLuint prog=shaderManager.get("shader/FontTest.vs","","shader/FontTest.fs");   
    glUseProgram(prog);
    uniformManager.apply(prog);

    //
    std::stringstream ss;
    ss << "[1] " << (usePointLight?"Point light":"Directional light") << " : on\n";

    if(geometryShaderSupport) {
      ss << "[2] Debug shadows : " << (useShadowDebug?"on":"off") << "\n";
      ss << "[3] Backfaced shadows : " << (useBackfaceShadow?"on":"off") << "\n";
      ss << "[4] Smooth shadows : " << (useSmoothShadow?"on":"off") << "\n";
      ss << "[5] Polygon offset shadows : " << (useShadowPolygonOffset?"on":"off") << "\n";
      ss << "[6] ZPass shadows : " << (useShadowZPass?"on":"off") << "\n";
      ss << "[7] Depth func less equal shadows : " << (useShadowDepthLessEqual?"on":"off") << "\n";      
    } else {
      ss << "\nNo shadows (geometry shaders unsupported)";
    }
    
    //
    myFont->generate(ss.str(),0.0f,0.0f,true);
    glBindVertexArray(myFont->getVao());
    myFont->draw();

    //
    //glUseProgram(shaderManager.get("shader/TextureTest.vs","","shader/TextureTest.fs"));
   // glBindVertexArray(screenVao);
    //screenDraw.draw();

    //
    glDisable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);

    glDisable(GL_BLEND);
    glBlendFunc(GL_ONE,GL_ZERO);

    glDisable(GL_CULL_FACE);
  }
}

void initResources() {
  //screen geometry
  screenVao = geometryManager.getVao("geometry/Screen.geom","geometry/Default.layout");
  screenDraw = geometryManager.getDraw("geometry/Screen.geom","default");

  //plane geometry
  planeVao = geometryManager.getVao("geometry/Plane.geom","geometry/Default.layout");
  planeDraw = geometryManager.getDraw("geometry/Plane.geom","default");

  //cube geometry
  cubeVao = geometryManager.getVao("geometry/head.geom","geometry/Default.layout");
  cubeDraw = geometryManager.getDraw("geometry/head.geom","default");

  //cube shadow geometry
  cubeShadowVao = geometryManager.getVao("geometry/head_shadow.geom","geometry/Default.layout");
  cubeShadowDraw = geometryManager.getDraw("geometry/head_shadow.geom","default");

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


  myFont=new MyFont("font/DroidSerif-Bold.ttf",14,1024);

}

void mouseMoveCallback(int x,int y) {
  mouseSmooth.onMouseMove((float)x,(float)y);
}

void inputCallback(int code,int action) {

  if(code==VK_LBUTTON) {
    mouseLook=action!=0;
  } else if(code==VK_W) {
    camera.setForward(action!=0);
  } else if(code==VK_S) {
    camera.setBackward(action!=0);
  } else if(code==VK_A) {
    camera.setLeftward(action!=0);
  } else if(code==VK_D) {
    camera.setRightward(action!=0);
  } else if(code==VK_1) {
    if(action==1) {
      usePointLight=!usePointLight;
    }
  } else if(code==VK_2) {
    if(action==1) {
      useShadowDebug=!useShadowDebug;
    }
  } else if(code==VK_4) {
    if(action==1) {
      useSmoothShadow=!useSmoothShadow;
    }
  } else if(code==VK_3) {
    if(action==1) {
      useBackfaceShadow=!useBackfaceShadow;
    }
  } else if(code==VK_5) {
    if(action==1) {
      useShadowPolygonOffset=!useShadowPolygonOffset;
    }
  } else if(code==VK_6) {
    if(action==1) {
      useShadowZPass=!useShadowZPass;
    }
  } else if(code==VK_7) {
    if(action==1) {
      useShadowDepthLessEqual=!useShadowDepthLessEqual;
    }    
  } else if(code==VK_P) {
    if(action==1) {
      const float *pos=camera.getPos();
      float yaw=camera.getYaw();
      float pitch=camera.getPitch();
      std::cout << "camera " << pos[0] << "," << pos[1] << "," << pos[2] << "; " << yaw << "," << pitch << std::endl;
    }
  } else if(action==1) {
    std::cout << "input=" << code << std::endl;
  }
}

void printGLExts() {
  int extNum;
  glGetIntegerv(GL_NUM_EXTENSIONS,&extNum);

  for(int i=0;i<extNum;i++) {
    std::cout << glGetStringi(GL_EXTENSIONS,i) << std::endl;
  }
}

void printVers() {

  //std::cout << "GLEW " << glewGetString(GLEW_VERSION) << std::endl;
  std::cout << "GL " << glGetString(GL_VERSION) << std::endl;
  std::cout << "Shader version " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
  std::cout << std::endl;
}

int main(int argc,char* argv[]) {
  MyWindow w("Demo",1024,600);
  w.setMouseMoveCallback(mouseMoveCallback);
  w.setInputCallback(inputCallback);
  if(w.isInitError()) {
    return 1;
  }

  //
  if(ogl_LoadFunctions() == ogl_LOAD_FAILED) {
    std::cout << "ogl_LOAD_FAILED\n";
    return 5;
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
  deferred=new Deferred();

  //
  initResources();

  //
  //camera.setYaw(4.8f);
  //camera.setPitch(-0.8f);
  //camera.setPosition(-5.2f,10.8f,2.6f);


  camera.setYaw(5.9f);
  camera.setPitch(-0.42f);
  camera.setPosition(-4.4f,22.4f,16.5f);


  //

  fileMon.monitor("shader/geometry");
  fileMon.monitor("shader/deferred");
  fileMon.monitor("shader");
  fileMon.monitor("texture");
  fileMon.monitor("geometry");

  deferred->setup(128,128);

  Timer timer;
  timer.start();
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

  //main loop
  while(w.run()) {

    if(w.isSized() && w.getClientWidth()>0 && w.getClientHeight()>0) {
      deferredWidth=w.getClientWidth();//*2;
      deferredHeight=w.getClientHeight();//*2;

      std::cout << "deferred setup "<< deferredWidth << ", " <<deferredHeight << std::endl;
      deferred->setup(deferredWidth,deferredHeight);
    }
    double time=timer.stop();
    frame=time;
    //
    fileMon.update();

    for(auto i : fileMon.getChanges()) {
      std::cout << i << " : updated.\n";

      textureManager.reload2d(i) ||
        textureManager.reloadCube(i) ||
        geometryManager.reloadGeometry(i) ||
        geometryManager.reloadLayout(i) ||
        shaderManager.reloadSource(i);

      uniformManager.clearPrograms();
    }

    //
    static double lastTime=time;


    //
    double deltaTime=time-lastTime;

    if(mouseLook  && w.isFocused() && !w.isIconified()) {
      w.setLockCursor(true);
      camera.setPitching((float)mouseSmooth.getY());
      camera.setYawing((float)mouseSmooth.getX());
    } else {

      w.setLockCursor(false);
    }

    angle=(float)time;
    camera.update((float)deltaTime);

    render(w.getClientWidth(),w.getClientHeight());

    //
    mouseSmooth.beforeInput();

    w.swapBuffers();

    lastTime=time;
  }

  //cleanup
  delete deferred;

  return 0;
}
