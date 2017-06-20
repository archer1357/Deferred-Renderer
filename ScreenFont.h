#ifndef SCREENFONT_H
#define SCREENFONT_H


#include <string.h>
#include <stdio.h>
#include <stdlib.h>


#ifndef __cplusplus
#include <stdbool.h>
#endif

#include "gl_core_3_3.h"
#include "Font.h"

struct ScreenFont {
  struct FontGlyph glyphs[FONT_ASCII_GLYPHS];
  int rowAdvance;
  int texWidth,texHeight;

  GLuint vao,vertBuf,prog,tex;
  char *text;
  float *verts;
  int maxChars;
};

#ifdef __cplusplus
extern "C" {
#endif

  struct ScreenFont *createScreenFont(const char *fn, int pointSize, int maxChars);
  void destroyScreenFont(struct ScreenFont *screenFont);
  void drawScreenFont(struct ScreenFont *screenFont,const char *text,int width,int height);



#ifdef __cplusplus
}
#endif
#endif
