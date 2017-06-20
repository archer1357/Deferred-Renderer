#include "ScreenFont.h"

GLuint initFontProg() {
  GLuint vs,fs;
  GLint status;
  int texLoc;

  const char *vsSrcs[]={
#ifndef __EMSCRIPTEN__
    "#version 110\n"
#endif
    "attribute vec4 a_pos_tex;\n"
    "varying vec2 v_tex;\n"
    "void main() {\n"
    " gl_Position =vec4(a_pos_tex.xy,0.0,1.0);\n"
    " v_tex=a_pos_tex.zw;\n"
    "}\n"
  };

  const char *fsSrcs[]={
#ifndef __EMSCRIPTEN__
    "#version 110\n"
#endif
    "#ifdef GL_ES\n"
    "precision highp float;\n"
    "#endif\n"
    "varying vec2 v_tex;\n"
    "uniform sampler2D u_colMap;\n"
    "void main() {\n"
    " float a=texture2D(u_colMap,v_tex).a;\n"
    // " float a=texture2D(u_colMap,v_tex).r;\n"
    " gl_FragColor=vec4(1.0,1.0,0.2,a);\n"
    "}\n"
  };

  vs=glCreateShader(GL_VERTEX_SHADER);
  fs=glCreateShader(GL_FRAGMENT_SHADER);

  //
  glShaderSource(vs,1,vsSrcs,0);
  glShaderSource(fs,1,fsSrcs,0);

  //
  glCompileShader(vs);
  glGetShaderiv(vs,GL_COMPILE_STATUS,&status);

  if(status==0) {
    fprintf(stderr,"font vs err\n");
    return 0;
  }

  //
  glCompileShader(fs);
  glGetShaderiv(fs,GL_COMPILE_STATUS,&status);

  if(status==0) {
    fprintf(stderr,"font fs err\n");
    glDeleteShader(vs);
    return 0;
  }

  //
  GLuint prog=glCreateProgram();

  //
  glAttachShader(prog,vs);
  glAttachShader(prog,fs);

  //
  glDeleteShader(vs);
  glDeleteShader(fs);

  //
  glBindAttribLocation(prog,0,"a_pos_tex");

  //
  glLinkProgram(prog);
  glGetProgramiv(prog,GL_LINK_STATUS,&status);

  if(status==0) {
    fprintf(stderr,"font prog link err\n");
    return 0;
  }

  texLoc=glGetUniformLocation(prog,"u_colMap");
  glUseProgram(prog);
  glUniform1i(texLoc,1);

  return prog;
}

void initFontTex(const unsigned char *bitmap, int w, int h, GLuint *tex) {
  GLenum format;

  format=GL_ALPHA;
  // format=GL_RED;

  glGenTextures(1,tex);
  glBindTexture(GL_TEXTURE_2D,*tex);
  glTexImage2D(GL_TEXTURE_2D,0,format,w,h,0,format,GL_UNSIGNED_BYTE,bitmap);

  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  // glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  // glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void initFontGeom(int maxChars,GLuint *vao, GLuint *vertBuf) {
  const int vertsSize=sizeof(float)*maxChars*6*4;

  glGenVertexArrays(1,vao);
  glBindVertexArray(*vao);

  glGenBuffers(1,vertBuf);
  glBindBuffer(GL_ARRAY_BUFFER,*vertBuf);
  glBufferData(GL_ARRAY_BUFFER,vertsSize,0,GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,0,0);
  glEnableVertexAttribArray(0);
}



struct ScreenFont *createScreenFont(const char *fn, int pointSize, int maxChars) {
  struct ScreenFont *screenFont;
  screenFont=(struct ScreenFont*)malloc(sizeof(struct ScreenFont));

  screenFont->vao=0;
  screenFont->vertBuf=0;

  screenFont->tex=0;
  screenFont->verts=(float*)malloc(maxChars*6*4*sizeof(float));
  screenFont->prog=initFontProg();

  if(!screenFont->prog) {
    destroyScreenFont(screenFont);
    return 0;
  }

  //
  unsigned char *bm;




  if(!myfont_asciiCreateFromFile(fn,pointSize,screenFont->glyphs,
                                 &screenFont->rowAdvance,&bm,
                                 &screenFont->texWidth,
                                 &screenFont->texHeight)) {
    destroyScreenFont(screenFont);
    return 0;
  }


  initFontTex(bm, screenFont->texWidth,screenFont->texHeight,&screenFont->tex);

  free(bm);


  //
  initFontGeom(maxChars,&screenFont->vao,&screenFont->vertBuf);

  return screenFont;
}
void destroyScreenFont(struct ScreenFont *screenFont) {
  glDeleteVertexArrays(1,&screenFont->vao);
  glDeleteBuffers(1,&screenFont->vertBuf);
  glDeleteProgram(screenFont->prog);
  glDeleteTextures(1,&screenFont->tex);
  free(screenFont->verts);
}
void drawScreenFont(struct ScreenFont *screenFont,
                    const char *text,int width,int height) {

  GLuint prog=screenFont->prog;
  GLuint tex=screenFont->tex;
  const struct FontGlyph *glyphs=screenFont->glyphs;
  int rowAdvance=screenFont->rowAdvance;
  int texWidth=screenFont->texWidth;
  int texHeight=screenFont->texHeight;
  float *verts=screenFont->verts;
  GLuint vao=screenFont->vao;
  GLuint vertBuf=screenFont->vertBuf;

  //
  int vertsNum;
  int size;

  size=strlen(text);

  //


  //program
  glUseProgram(prog);

  //tex
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D,tex);

  //draw states begin
  glEnable(GL_CULL_FACE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE);

  //generate verts
  // myfont_asciiDrawVerts(text,size,true,0.0f,0.0f,face,verts,&vertsNum);
  myfont_asciiDrawVerts(text,size,true,0.0f,0.0f,
                            glyphs,
                            rowAdvance,
                            texWidth,texHeight,
                            verts,&vertsNum);
    int i;
  for(i=0;i<vertsNum;i++) {
    verts[i*4]=-1.0f+2.0f*verts[i*4]/(float)width;
    verts[i*4+1]=1.0f-2.0f*verts[i*4+1]/(float)height;
  }

  //update vert buf
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER,vertBuf);
  glBufferSubData(GL_ARRAY_BUFFER,0,vertsNum*4*sizeof(float),verts);

  //draw
  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES,0,vertsNum);

  //draw states end
  glDisable(GL_CULL_FACE);
  glDisable(GL_BLEND);
  glBlendFunc(GL_ONE,GL_ZERO);
}
