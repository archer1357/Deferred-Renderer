#ifndef FONT_H
#define FONT_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include "stb_truetype.h"

struct FontGlyph {
  short mX,mY,mWidth,mHeight;
  short originOffsetX,originOffsetY;
  short mAdvanceX;
  int index;
};

#define FONT_ASCII_GLYPHS 96

#ifdef __cplusplus
extern "C" {
#endif

  void myfont_createBitmap(const stbtt_fontinfo *fontInfo,
                           const struct FontGlyph *glyphs,
                           int glyphsNum,float scale,
                           int texWidth,int texHeight,
                           unsigned char *pBits) ;

  void myfont_packGlyphs(struct FontGlyph *glyphs,
                         int glyphsNum,int padding,
                         int *texWidth,int *texHeight);

  bool myfont_loadFile(const char *fn,unsigned char **data,
                       stbtt_fontinfo *fontInfo);

  void myfont_glyphQuad(const struct FontGlyph *glyph,
                        bool flipped,float rowAdvance,
                        float tex_ww, float tex_hh,
                        float x, float y,
                        stbtt_aligned_quad *q);

  void myfont_quadVerts(const stbtt_aligned_quad *q, float *verts);


  void myfont_asciiGlyphs(const stbtt_fontinfo *fontInfo,
                          float scale,int ascent,
                          struct FontGlyph *glyphs);

  void myfont_asciiCreate(const stbtt_fontinfo *fontInfo,
                          int pointSize,
                          struct FontGlyph *glyphs,
                          int *rowAdvance,
                          unsigned char **bits,
                          int *texWidth,int *texHeight);

  bool myfont_asciiCreateFromFile(const char *fn,
                                  int pointSize,
                                  struct FontGlyph *glyphs,
                                  int *rowAdvance,
                                  unsigned char **bits,
                                  int *texWidth,int *texHeight);

  void myfont_asciiDrawVerts(const char *str,int strCount,
                             bool flipped,float x,float y,
                             const struct FontGlyph *glyphs,
                             int rowAdvance,
                             int texWidth,int texHeight,
                             float *verts, int *vertsNum);


#ifdef __cplusplus
}
#endif
#endif
