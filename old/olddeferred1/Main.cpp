#include "Main.h"

Scene *scene=0;
SimpleRenderer *simpleRenderer=0;

FileMonitor fileMonitor;
Window *window;

bool geometryShaderSupport=false;

std::string mainScript="data/Main.lua";

void renderGeometry() {
  for(SceneMesh *m : scene->getMeshes()) {
    RendererGeometryMaterial material;
    material.bumpBias=m->bumpBias;
    material.bumpScale=m->bumpScale;
    material.shininess=m->shininess;
    material.reflective=m->reflective;
    material.emissive=m->emissive;
    for(int i=0;i<3;i++) material.color[i]=m->color[i];
    material.colorTex=m->colorTex;
    material.normalTex=m->normalTex;
    material.heightTex=m->heightTex;
    material.reliefTex=m->reliefTex;
    material.specularTex=m->specularTex;
    simpleRenderer->geometryRender(m->modelMat,m->vao,m->geometry,m->draw,&material);
  }
}

void renderLights() {

  simpleRenderer->lightBegin(false);

  for(ScenePointLight *pl : scene->getPointlights()) {
    if(!pl->shadow) {
      simpleRenderer->pointLightRender(pl->pos, pl->attenuation, pl->strength, pl->color);
    }
  }

  for(SceneSpotLight *sl : scene->getSpotlights()) {
    if(!sl->shadow) {
      simpleRenderer->spotLightRender(sl->pos,sl->dir,sl->attenuation,sl->strength,sl->spotExponent,sl->spotCutoff,sl->color);
    }
  }

  for(SceneDirectionalLight *dl : scene->getDirectionallights()) {
    if(!dl->shadow) {
      simpleRenderer->dirLightRender(dl->dir,dl->strength,dl->color);
    }
  }

  simpleRenderer->lightEnd(false);
}

void renderShadowedLights(bool useGeometryShader,bool zPass,bool robust, bool backFaced) {

  for(ScenePointLight *pl : scene->getPointlights()) {
    if(pl->shadow) {
      simpleRenderer->shadowVolumeBegin(zPass,backFaced,false);

      for(SceneShadow *shadow : scene->getShadows()) {
        simpleRenderer->pointLightShadowVolumeRender(useGeometryShader,pl->pos,shadow->modelMat,zPass,robust,backFaced,shadow->geometry,false);
      }

      simpleRenderer->shadowVolumeEnd(backFaced,false);

      simpleRenderer->lightBegin(true);
      simpleRenderer->pointLightRender(pl->pos, pl->attenuation, pl->strength, pl->color);
      simpleRenderer->lightEnd(true);
    }
  }

  for(SceneSpotLight *sl : scene->getSpotlights()) {
    if(sl->shadow) {
      simpleRenderer->shadowVolumeBegin(zPass,backFaced,false);

      for(SceneShadow *shadow : scene->getShadows()) {
        simpleRenderer->spotLightShadowVolumeRender(useGeometryShader,sl->pos,sl->dir,shadow->modelMat,zPass,robust,backFaced,shadow->geometry,false);
      }

      simpleRenderer->shadowVolumeEnd(backFaced,false);

      simpleRenderer->lightBegin(true);
      simpleRenderer->spotLightRender(sl->pos,sl->dir,sl->attenuation,
                                      sl->strength,sl->spotExponent,
                                      sl->spotCutoff,sl->color);
      simpleRenderer->lightEnd(true);
    }
  }

  for(SceneDirectionalLight *dl : scene->getDirectionallights()) {
    if(dl->shadow) {
      simpleRenderer->shadowVolumeBegin(zPass,backFaced,false);

      for(SceneShadow *shadow : scene->getShadows()) {
        simpleRenderer->dirLightShadowVolumeRender(useGeometryShader,dl->dir,shadow->modelMat,zPass,robust,backFaced,shadow->geometry,false);
      }

      simpleRenderer->shadowVolumeEnd(backFaced,false);

      simpleRenderer->lightBegin(true);
      simpleRenderer->dirLightRender(dl->dir,dl->strength,dl->color);
      simpleRenderer->lightEnd(true);
    }
  }
}

void renderShadowedLightsDebug(bool useGeometryShader,bool zPass,bool robust, bool backFaced) {

  simpleRenderer->shadowVolumeBegin(zPass,backFaced,true);

  for(ScenePointLight *pl : scene->getPointlights()) {
    if(pl->shadow) {

      for(SceneShadow *shadow : scene->getShadows()) {
        simpleRenderer->pointLightShadowVolumeRender(useGeometryShader,pl->pos,shadow->modelMat,zPass,robust,backFaced,shadow->geometry,true);
      }
    }
  }

  for(SceneSpotLight *sl : scene->getSpotlights()) {
    if(sl->shadow) {
      for(SceneShadow *shadow : scene->getShadows()) {
        simpleRenderer->spotLightShadowVolumeRender(useGeometryShader,sl->pos,sl->dir,shadow->modelMat,zPass,robust,backFaced,shadow->geometry,true);
      }
    }
  }

  for(SceneDirectionalLight *dl : scene->getDirectionallights()) {
    if(dl->shadow) {
      for(SceneShadow *shadow : scene->getShadows()) {
        simpleRenderer->dirLightShadowVolumeRender(useGeometryShader,dl->dir,shadow->modelMat,zPass,robust,backFaced,shadow->geometry,true);
      }
    }
  }

  simpleRenderer->shadowVolumeEnd(backFaced,true);
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




int main(int argc, char* argv[]) {
  if(argc>1) {
    mainScript=argv[1];
  }

  //glfw
  if(!glfwInit()) {
    std::cout << "GLFW init error.\n";
    return 1;
  }
  window=new Window("Demo",1280, 600);

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
  //programManager=new ProgramManager();

  //
  if(majorVer>3 || minorVer>=3) {
    geometryShaderSupport=true;
  }
  //printGLExts();

  //inits
  scene=new Scene(mainScript);
  simpleRenderer=new SimpleRenderer();
  //
  MouseSmooth mouseSmooth;
  //
  fileMonitor.addWatch("data/shader/geometry");
  fileMonitor.addWatch("data/shader/deferred");
  fileMonitor.addWatch("data/shader");
  fileMonitor.addWatch("data/texture");
  fileMonitor.addWatch("data/geometry");
  fileMonitor.addWatch("data");

  while(window->run()) {
    mouseSmooth.beforeInput();

    for(auto i : window->getCursorInputs()) {
      mouseSmooth.onCursor(i.x,i.y);
    }


    int clientWidth=window->getClientWidth();
    int clientHeight=window->getClientHeight();


    window->lockCursor(scene->isLockCursor());
    for(auto i : window->getKeyInputs()) {
      scene->onKey(i.key,i.action,i.mods);
    }
    for(auto i : window->getMouseButtonInputs()) {
      scene->onMouseButton(i.button,i.action,i.mods);
    }
    for(auto i : window->getCursorInputs()) {
      scene->onCursor(i.x,i.y);
    }

    fileMonitor.run();
    double time = glfwGetTime();

    for(auto i : fileMonitor.getModified()) {
      simpleRenderer->onFileModified(i);
      scene->onFileModified(i);
      std::cout << i << " modified.\n";
    }
      scene->setSmoothMouse(mouseSmooth.getX(),mouseSmooth.getY());
   // scene->setSmoothMouse(window->getSmoothMouseX(),window->getSmoothMouseY());
    scene->run(time,window->getClientWidth(),window->getClientHeight());



    if(!window->isIconified()) {

      //
      bool moveForward=false;
      bool moveBackward=false;
      bool moveRightward=false;
      bool moveLeftward=false;
      bool moveUpward=false;
      bool moveDownward=false;

      //

      for(auto i : window->getKeyInputs()) {
        if(i.key==87 && i.action!=0) { //w
          moveForward=true;

        } else if(i.key==83 && i.action!=0) { //s
          moveBackward=true;
          // direction.z -= 1.0f;
        } else if(i.key==65 && i.action!=0) { //a
          moveLeftward=true;
          // direction.x -= 1.0f;
        } else if(i.key==68 && i.action!=0) { //d
          moveRightward=true;
        } else if(i.key==81 && i.action!=0) { //q
          moveDownward=true;
        } else if(i.key==69 && i.action!=0) { //e
          moveUpward=true;
        }
      }

      if(moveForward) {

        //  g_camera.setCurrentVelocity(velocity.x, velocity.y, 0.0f);

      }

      /*
        if(moveBackward) {
        g_camera.setCurrentVelocity(velocity.x, velocity.y, 0.0f);
        }


        if(moveLeftward) {
        g_camera.setCurrentVelocity(0.0f, velocity.y, velocity.z);
        }

        if(moveRightward) {
        g_camera.setCurrentVelocity(0.0f, velocity.y, velocity.z);
        }

        if(moveUpward) {
        g_camera.setCurrentVelocity(velocity.x, 0.0f, velocity.z);
        }

        if(moveDownward) {
        g_camera.setCurrentVelocity(velocity.x, 0.0f, velocity.z);
        }*/


      glViewport(0,0,clientWidth,clientHeight);

      //std::cout << window->getDeltaTime() << std::endl;


      simpleRenderer->setEnvMap("data/texture/blue_light.cube");
      simpleRenderer->setZNear(1.0f);
      simpleRenderer->setZFar(500.0f);
      simpleRenderer->setFov((float)M_PI_4);
      simpleRenderer->setWidth(clientWidth);
      simpleRenderer->setHeight(clientHeight);
       simpleRenderer->setViewMat(scene->getViewMat());

     // simpleRenderer->setViewMat(&vm2[0][0]);
      simpleRenderer->update();


      simpleRenderer->geometryBegin();
      renderGeometry();
      simpleRenderer->geometryEnd();

      //
      bool useGeometryShader=geometryShaderSupport && scene->isGeometryShadows();
      bool backFacedShadows=false;
      bool zPassShadows=false;
      bool robustShadows=false;
      //

      simpleRenderer->deferredBegin();
      renderLights();
      renderShadowedLights(useGeometryShader, zPassShadows, robustShadows,  backFacedShadows);
      simpleRenderer->deferredEnd();



      simpleRenderer->skyboxRender();


      if(scene->isShadowDebug()) {
        renderShadowedLightsDebug(useGeometryShader,zPassShadows,robustShadows, backFacedShadows);
      }

      if(geometryShaderSupport) {
        //render->normalVectorsRender() ;
      }


      window->swapBuffers();

    }
    //
  }

  //uninits
  delete scene;
  delete simpleRenderer;
  delete window;
  return 0;
}
