#ifndef FONT_H
#define FONT_H
#include <vector>
#include <string>
#include "gl_core_3_3.h"
#include <algorithm>
 
struct FontGlyph
{
  //! X position in texture
  short mX;
  //! Y position in texture
  short mY;
  //! Width
  short mWidth;
  //! Height
  short mHeight;
  //! Glyph X offset from origin
  short mOffsetX;
  //! Glyph Y offset from origin
  short mOffsetY;
  //! Horizontal advance
  short mAdvanceX;
};

struct FontFace {
  int mPointSize;
  int mRowHeight;
  FontGlyph glyphs[96];
};

class MyFont {
public:
  int texWidth,texHeight;
  FontFace newFace;
  GLuint texture;
  GLuint fontVao,fontVertBuf,fontIndBuf;
  int fontDrawCount;
  int maxChars;
public:
  MyFont(const std::string &fn,int pointSize, int maxChars);
  ~MyFont();
  GLuint getTexture();
  GLuint getVao();
  void generate(const std::string &text,float x, float y,bool flipped);
  void draw();
};

#endif
