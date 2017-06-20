#include "Font.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void myfont_createBitmap(const stbtt_fontinfo *fontInfo,
                         const struct FontGlyph *glyphs,
                         int glyphsNum,float scale,int texWidth,
                         int texHeight,unsigned char *pBits) {
  //Originally taken from the Urho3D Engine
  int i,y,x;
  int Pitch=texWidth;
  memset(pBits,0,texWidth*texHeight);

  // Render glyphs into texture, and find out a scaling value in case font uses less than full opacity (thin outlines)
  int sumOpacity = 0;
  int nonEmptyGlyphs = 0;

  for(i = 0; i < glyphsNum; ++i)  {
    unsigned char *output;
    output=(unsigned char*)pBits + Pitch * glyphs[i].mY + glyphs[i].mX;
    stbtt_MakeGlyphBitmap(fontInfo,output,glyphs[i].mWidth,glyphs[i].mHeight,
                          Pitch,scale,scale,glyphs[i].index);

    int glyphMaxOpacity = 0;

    for( y = 0; y < glyphs[i].mHeight; ++y) {
      unsigned char* pixels;
      pixels=(unsigned char*)pBits+Pitch*(y+glyphs[i].mY)+glyphs[i].mX;

      for(x = 0; x < glyphs[i].mWidth; ++x) {
        if((int)pixels[x] > glyphMaxOpacity) {
          glyphMaxOpacity=(int)pixels[x];
        }
      }
    }

    if(glyphMaxOpacity > 0) {
      sumOpacity += glyphMaxOpacity;
      ++nonEmptyGlyphs;
    }
  }

  // Apply the scaling if necessary
  int avgOpacity = nonEmptyGlyphs ? sumOpacity / nonEmptyGlyphs : 255;

  if(avgOpacity < 255) {
    float scale = 255.0f / avgOpacity;

    for(i = 0; i < glyphsNum; ++i)  {
      for(y = 0; y < glyphs[i].mHeight; ++y) {
        unsigned char* dest;
        dest=(unsigned char*)pBits+Pitch*(y+glyphs[i].mY)+glyphs[i].mX;

        for(x = 0; x < glyphs[i].mWidth; ++x) {
          int aaa=(int)(dest[x]*scale);
          dest[x]=(aaa < 255)?aaa:255;
        }
      }
    }
  }
}

int myfont_glyphCompare(const void *a,const void *b) {
  const struct FontGlyph *aa,*bb;
  aa=*((const struct FontGlyph **)a);
  bb=*((const struct FontGlyph **)b);

  if(aa->mHeight<bb->mHeight) {
    return 1;
  }

  if(aa->mHeight==bb->mHeight) {
    return 0;
  }

  //if(aa->mHeight>bb->mHeight) {
    return -1;
  //}
}

void myfont_packGlyphs(struct FontGlyph *glyphs,
                       int glyphsNum,int padding,
                       int *texWidth,int *texHeight) {

  //todo: when glyph.height>glyph.width then flip them sideways
  // add sideways : bool to glyph struct
  // update glyphQuad func

  int i=0;
  int x=padding;
  int y=padding;
  int width=32;
  int height=32;
  int xEnd=0; //furthest x+glyphWidth has been entirely
  int xStart=padding; //where to start from after moving y down
  int yEnd=padding; //furthest y+glyphHeight has been entirely
  int yStep=0; //furthest y+glyphHeight has been within current area
  struct FontGlyph **sortedGlyphs;
  int j;

  sortedGlyphs=(struct FontGlyph **)malloc(sizeof(void*)*glyphsNum);

  for(j=0;j<glyphsNum;j++) {
    sortedGlyphs[j]=&glyphs[j];
  }

  qsort(sortedGlyphs, glyphsNum, sizeof(void*), myfont_glyphCompare);

  while(i<glyphsNum) {
    struct FontGlyph *glyph=sortedGlyphs[i];

    if(x+glyph->mWidth+padding <= width &&
       y+glyph->mHeight+padding <= height) { //go across
      glyph->mX=x;
      glyph->mY=y;
      x+=glyph->mWidth+padding;

      //set furthest down within area
      if(y+glyph->mHeight+padding > yStep) {
        yStep=y+glyph->mHeight+padding;
      }

      //set furthest across
      if(x > xEnd) {
        xEnd=x;
      }

      //set furthest down
      if(yStep>yEnd) {
        yEnd=yStep;
      }

      i++;
    } else if(x+glyph->mWidth+padding > width &&
              yStep+glyph->mHeight+padding<=height) { //can't go across, go down
      //set x to last starting point, like a typewriter
      x=xStart;

      //set y down to furthest down within area
      y=yStep;
    } else if(width<=height) { //can't go across, can't go down, expand rightward
      //set x to furthest across
      xStart=x=xEnd;

      //y and yStep goes to zero since we are expanding rightward
      y=yStep=padding;

      //expand across
      width*=2;
    } else { //can't go across, can't go down, expand downward
      //x and xStart goes to zero since we are expanding downward
      x=xStart=padding;

      //set y to furthest down
      y=yEnd;

      //expand downward
      height*=2;
    }
  }

  //
  free(sortedGlyphs);

  //
  *texWidth=width;
  *texHeight=height;


}

bool myfont_loadFile(const char *fn,unsigned char **data,
                     stbtt_fontinfo *fontInfo) {
  FILE *file;
  int dataLen;

  //load font file
  file = fopen(fn, "rb");

  if(!file) {
    fprintf(stderr,"font read file error : %s\n",fn);
    return false;
  }

  fseek(file,0L,SEEK_END);
  dataLen = ftell(file);
  fseek(file,0L,SEEK_SET);
  *data=(unsigned char*)malloc(dataLen);
  fread(*data,1,dataLen,file);
  fclose(file);

  //
  if(!stbtt_InitFont(fontInfo,*data,0)) {
    fprintf(stderr,"font load error : %s\n",fn);
    free(data);
    return false;
  }

  return true;
}

void myfont_glyphQuad(const struct FontGlyph *glyph,
                     bool flipped,float rowAdvance,
                     float tex_ww, float tex_hh,
                     float x, float y,
                     stbtt_aligned_quad *q) {

  if(flipped) {
    q->y0 = y+(float)glyph->originOffsetY; //use floorf?
    q->y1 = q->y0 + (float)glyph->mHeight;
  } else {
    q->y0 = y+(float)rowAdvance-(float)glyph->originOffsetY; //use floorf?
    q->y1 = q->y0 - (float)glyph->mHeight;
  }

  q->x0 = x + (float)glyph->originOffsetX; //use floorf?
  q->x1 = q->x0 + (float)glyph->mWidth;
  q->s0 = glyph->mX*tex_ww;
  q->t0 = (glyph->mY + glyph->mHeight)*tex_hh;
  q->s1 = (glyph->mX + glyph->mWidth)*tex_ww;
  q->t1 = glyph->mY*tex_hh;
}

void myfont_quadVerts(const stbtt_aligned_quad *q, float *verts) {
  //2
  *(verts++)=q->x1; *(verts++)=q->y1;
  *(verts++)=q->s1; *(verts++)=q->t0;

  //1
  *(verts++)=q->x1; *(verts++)=q->y0;
  *(verts++)=q->s1; *(verts++)=q->t1;

  //0
  *(verts++)=q->x0; *(verts++)=q->y0;
  *(verts++)=q->s0; *(verts++)=q->t1;

  //3
  *(verts++)=q->x0; *(verts++)=q->y1;
  *(verts++)=q->s0; *(verts++)=q->t0;

  //2
  *(verts++)=q->x1; *(verts++)=q->y1;
  *(verts++)=q->s1; *(verts++)=q->t0;

  //0
  *(verts++)=q->x0; *(verts++)=q->y0;
  *(verts++)=q->s0; *(verts++)=q->t1;
}

void myfont_asciiGlyphs(const stbtt_fontinfo *fontInfo,
                            float scale,int ascent,
                            struct FontGlyph *glyphs) {
  //Originally taken from the Urho3D Engine
  int i;
  //96 = 95 ascii + null = 126 - 32 + 1

  const int glyphsFrom=32;
  const int glyphsTo=127;

  int scaledAscent = (int)(scale * ascent);

  // Go through glyphs to get their dimensions & offsets
  for(i = glyphsFrom; i <= glyphsTo; ++i)  {

    //last one is for null
    int ii=(i==glyphsTo)?0:i;

    int mGlyphIndex = stbtt_FindGlyphIndex(fontInfo,ii);

    int ix0,iy0,ix1,iy1;
    int advanceWidth,leftSideBearing;

    stbtt_GetGlyphBitmapBox(fontInfo,mGlyphIndex,scale,scale,&ix0,&iy0,&ix1,&iy1);
    stbtt_GetGlyphHMetrics(fontInfo,mGlyphIndex,&advanceWidth,&leftSideBearing);
    glyphs[i-glyphsFrom].mWidth = ix1 - ix0;
    glyphs[i-glyphsFrom].mHeight = iy1 - iy0;
    glyphs[i-glyphsFrom].originOffsetX = (int)(leftSideBearing * scale);
    glyphs[i-glyphsFrom].originOffsetY = iy0+scaledAscent; //top align
    glyphs[i-glyphsFrom].mAdvanceX = (int)(advanceWidth * scale);
    glyphs[i-glyphsFrom].index=mGlyphIndex;
  }
}

void myfont_asciiCreate(const stbtt_fontinfo *fontInfo,
                              int pointSize,
                              struct FontGlyph *glyphs,
                              int *rowAdvance,
                              unsigned char **bits,
                              int *texWidth,int *texHeight) {

  int ascent,descent,lineGap;
  float scale;
  int w,h;

  //Get row height
  stbtt_GetFontVMetrics(fontInfo,&ascent,&descent,&lineGap);

  //Calculate scale (use ascent only)
  scale=(float)pointSize/ascent;

  //Calculate row advance
  *rowAdvance=(int)(scale*(ascent-descent+lineGap));

  //
  myfont_asciiGlyphs(fontInfo,scale,ascent,glyphs);
  myfont_packGlyphs(glyphs,FONT_ASCII_GLYPHS,1,&w,&h);

  //create bitmap
  *bits=(unsigned char*)malloc(w*h);
  myfont_createBitmap(fontInfo,glyphs,FONT_ASCII_GLYPHS,scale,w,h,*bits);

  //
  *texWidth=w;
  *texHeight=h;
}
bool myfont_asciiCreateFromFile(const char *fn,
                                    int pointSize,
                                    struct FontGlyph *glyphs,
                                    int *rowAdvance,
                                    unsigned char **bits,
                                    int *texWidth,int *texHeight) {
  unsigned char *data,*bm;
  stbtt_fontinfo fontInfo;

  if(!myfont_loadFile(fn,&data,&fontInfo)) {
    return false;
  }

  myfont_asciiCreate(&fontInfo,pointSize,glyphs,rowAdvance,
                         bits,texWidth,texHeight);

  free(data);
  return true;
}

void myfont_asciiDrawVerts(const char *str,int strCount,
                                 bool flipped,float x,float y,
                                 const struct FontGlyph *glyphs,
                                 int rowAdvance,
                                 int texWidth,int texHeight,
                                 float *verts, int *vertsNum) {

  const int glyphsFrom=32;
  const int glyphsTo=127;

  float tex_ww=1.0f/(float)texWidth;
  float tex_hh=1.0f/(float)texHeight;
  float x2=x;
  const char *strEnd=&str[strCount];
  *vertsNum=0;

  while(str[0] && str < strEnd) {
    if(str[0]=='\n') {
      y+=(float)rowAdvance;
      x2=x;
    } else {
      const struct FontGlyph *glyph;

      if(str[0] >= glyphsFrom && str[0] < glyphsTo) {
        glyph= &glyphs[str[0]-glyphsFrom];
      } else { //else use null char
        glyph= &glyphs[glyphsTo-glyphsFrom];
      }

      //
      stbtt_aligned_quad q;
      myfont_glyphQuad(glyph,flipped,(float)rowAdvance,tex_ww,tex_hh,x2,y,&q);
      myfont_quadVerts(&q,verts);

      verts+=24;
      x2+=(float)glyph->mAdvanceX;
      (*vertsNum)+=6;
    }

    ++str;
  }
}
