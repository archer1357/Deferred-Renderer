#include "main.h"

GLuint vao,vertBuf,colBuf,prog;

float angleInterp=0.0;

bool checkGlError(const char *n) {
  const char *errStr=0;
  int errCode = glGetError();

  if(GL_INVALID_ENUM==errCode) {
    errStr="invalid enum";
  } else if(GL_INVALID_VALUE==errCode) {
    errStr="invalid value";
  } else if(GL_INVALID_OPERATION==errCode) {
    errStr="invalid operation";
  } else if(GL_INVALID_FRAMEBUFFER_OPERATION==errCode) {
    errStr="invalid frame buffer operation";
  } else if(GL_OUT_OF_MEMORY==errCode) {
    errStr="out of memory";
  }

  if(errStr) {
    fprintf(stderr,"%s : gl error : %s.\n",n,errStr);
    return false;
  }

  return true;
}

void initProg() {
  const char *vsSrcs[]={
#ifndef __EMSCRIPTEN__
    "#version 110\n"
#endif
    "attribute vec3 a_pos;\n"
    "attribute vec3 a_col;\n"
    "uniform float u_angle;\n"
    "varying vec3 v_col;\n"
    "void main() {\n"
    "  vec3 move=vec3(cos(u_angle)*0.3,sin(u_angle)*0.3,0.0);\n"
    "  gl_Position=vec4(a_pos*0.5+move,1.0);\n"
    "  v_col=a_col;\n"
    "}\n"};

  const char *fsSrcs[]={
#ifndef __EMSCRIPTEN__
    "#version 110\n"
#else
    "precision highp float;\n"
#endif
    "varying vec3 v_col;\n"
    "void main() {\n"
    "  gl_FragColor=vec4(v_col,1.0);\n"
    "}\n"};

  //
  char log[2048];
  GLint status;
  GLuint vs,fs;

  //
  vs=glCreateShader(GL_VERTEX_SHADER);

  glShaderSource(vs,1,vsSrcs,0);
  glCompileShader(vs);
  glGetShaderiv(vs,GL_COMPILE_STATUS,&status);

  if(!status) {
    glGetShaderInfoLog(vs,sizeof(log),0,log);
    fprintf(stderr,"vs compile error.\n\"%s\"\n",log);
  }

  //
  fs=glCreateShader(GL_FRAGMENT_SHADER);

  glShaderSource(fs,1,fsSrcs,0);
  glCompileShader(fs);
  glGetShaderiv(fs,GL_COMPILE_STATUS,&status);

  if(!status) {
    glGetShaderInfoLog(fs,sizeof(log),0,log);
    fprintf(stderr,"fs compile error.\n\"%s\"\n",log);
  }

  //
  prog=glCreateProgram();

  glAttachShader(prog,vs);
  glAttachShader(prog,fs);

  glBindAttribLocation(prog,0,"a_pos");
  glBindAttribLocation(prog,1,"a_col");

  glLinkProgram(prog);
  glGetProgramiv(prog,GL_LINK_STATUS,&status);

  if(!status) {
    glGetProgramInfoLog(prog,sizeof(log),0,log);
    fprintf(stderr,"prog link error.\n\"%s\"\n",log);
  }
}

void initGeom() {
  float verts[]={
    -1.0f,-1.0f,0.0f,
    1.0f,-1.0f,0.0f,
    0.0f,1.0f,0.0f
  };

  float cols[]={
    1.0f,0.0f,0.0f,
    0.0f,1.0f,0.0f,
    0.0f,0.0f,1.0f
  };

  //
  glGenVertexArrays(1,&vao);
  glBindVertexArray(vao);

  //
  glGenBuffers(1,&vertBuf);
  glBindBuffer(GL_ARRAY_BUFFER,vertBuf);
  glBufferData(GL_ARRAY_BUFFER,sizeof(verts),verts,GL_STATIC_DRAW);

  glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);
  glEnableVertexAttribArray(0);

  //
  glGenBuffers(1,&colBuf);
  glBindBuffer(GL_ARRAY_BUFFER,colBuf);
  glBufferData(GL_ARRAY_BUFFER,sizeof(cols),cols,GL_STATIC_DRAW);

  glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,0);
  glEnableVertexAttribArray(1);

  //
  glBindVertexArray(0);
}

void render() {
  glViewport(0,0,window_client_width(),window_client_height());
  glClearColor(0.2f,0.3f,0.6f,1.0f);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  //
  glUseProgram(prog);
  GLint angleUniformloc=glGetUniformLocation(prog,"u_angle");
  // printf("%i\n",angleUniformloc);
  glUniform1f(angleUniformloc,angleInterp);
  //
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  //
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES,0,3);

  //
  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);

  //
  checkGlError("draw");

}

void update() {
  static int steps=0;
  const int maxSteps=5;

  static double accumTime=0.0;
  const double stepTime=1.0/60.0;

  static double angle=0.0;
  const double angleRate=1.4;

  //
  double curTime=timer();

  //
  if(accumTime==0.0 || window_restored()) {
    accumTime=curTime;
  }

  steps=0;

  while(steps<maxSteps && accumTime+stepTime < curTime) {
    accumTime+=stepTime;
    steps++;

    //on step
    angle+=stepTime*angleRate;

    if(angle>2.0*M_PI) {
      angle-=2.0*M_PI;
    }
  }

  if(steps==maxSteps) {
    accumTime=curTime;
  }

  double interpTime=curTime-accumTime;

  //on interp
  angleInterp=(float)(angle+angleRate*interpTime);
}
void main_loop() {
  if(window_restored()) {
    printf("restored\n");
  }

  if(window_sized()) {
    printf("sized %i %i\n",window_client_width(),window_client_height());
  }



  //
  update();
  render();

  //
  window_swap_buffers();
}

#ifdef __EMSCRIPTEN__
void em_main_loop() {
  window_update();
  main_loop();
}
#endif

int main() {
  if(!window_create("demo",640,480)) {
    return 1;
  }

  if(ogl_LoadFunctions() == ogl_LOAD_FAILED) {
    return 1;
  }

  checkGlError("load gl funcs");

  initGeom();
  checkGlError("geom init");

  initProg();
  checkGlError("prog init");

  //
#ifdef __EMSCRIPTEN__
  emscripten_set_main_loop(em_main_loop, 0, 1);
#else
  while(window_update() && !window_key_press(KEY_ESCAPE)) {
    main_loop();
  }
#endif

  window_destroy();
  return 0;
}
