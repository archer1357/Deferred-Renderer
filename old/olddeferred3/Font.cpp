//
// Urho3D Engine
// Copyright (c) 2008-2011 Lasse Öörni
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// ***heavily modified***

#include "Font.h"

#include "stb_truetype.h"
#include <iostream>

#include <cstdlib>
#include <cmath>
#include <stdio.h>
#include <string.h>
#include <fstream>

class IntRect {
public:

  IntRect() { }
  IntRect(int left,int top,int right,int bottom) :
    mLeft(left), mTop(top), mRight(right), mBottom(bottom) { }

  const int* getData() const { return &mLeft; }

  int mLeft;
  int mTop;
  int mRight;
  int mBottom;
};
 

class AreaAllocator {
public:
  //! Construct with given width and height
  AreaAllocator(int width,int height);

  //! Reset to given width and height and remove all previous allocations
  void reset(int width,int height);
  //! Try to allocate an area. Return true if successful, with x & y coordinates filled
  bool reserve(int width,int height,int& x,int& y);

private:
  //! Cut the reserved area from a rectangle. Return true if the rectangle should be removed from the vector
  bool splitRect(IntRect original,const IntRect& reserved);
  //! Remove overlapping free rectangles
  void cleanup();

  //! Free rectangles
  std::vector<IntRect> mFreeAreas;
};


AreaAllocator::AreaAllocator(int width,int height)
{
  reset(width,height);
}

void AreaAllocator::reset(int width,int height)
{
  mFreeAreas.clear();

  IntRect initialArea(0,0,width,height);
  mFreeAreas.push_back(initialArea);
}

bool AreaAllocator::reserve(int width,int height,int& x,int& y)
{
  if(width < 0)
    width = 0;
  if(height < 0)
    height = 0;

  std::vector<IntRect>::iterator best = mFreeAreas.end();

  static const int M_MAX_INT = 0x7fffffff;
  int bestFreeArea = M_MAX_INT;

  for(std::vector<IntRect>::iterator i = mFreeAreas.begin(); i != mFreeAreas.end(); ++i)
  {
    int freeWidth = i->mRight - i->mLeft;
    int freeHeight = i->mBottom - i->mTop;

    if((freeWidth >= width) && (freeHeight >= height))
    {
      // Calculate rank for free area. Lower is better
      int freeArea = freeWidth * freeHeight;

      if((best == mFreeAreas.end()) || (freeArea < bestFreeArea))
      {
        best = i;
        bestFreeArea = freeArea;
      }
    }
  }

  if(best == mFreeAreas.end())
    return false;

  IntRect reserved;
  reserved.mLeft = best->mLeft;
  reserved.mTop = best->mTop;
  reserved.mRight = best->mLeft + width;
  reserved.mBottom = best->mTop + height;

  x = best->mLeft;
  y = best->mTop;

  // Remove the reserved area from all free areas
  for(unsigned i = 0; i < mFreeAreas.size();)
  {
    if(splitRect(mFreeAreas[i],reserved))
      mFreeAreas.erase(mFreeAreas.begin() + i);
    else
      ++i;
  }

  cleanup();
  return true;
}

bool AreaAllocator::splitRect(IntRect original,const IntRect& reserve)
{
  if((reserve.mRight > original.mLeft) && (reserve.mLeft < original.mRight) &&
    (reserve.mBottom > original.mTop) && (reserve.mTop < original.mBottom))
  {
    // Check for splitting from the right
    if(reserve.mRight < original.mRight)
    {
      IntRect newRect = original;
      newRect.mLeft = reserve.mRight;
      mFreeAreas.push_back(newRect);
    }
    // Check for splitting from the left
    if(reserve.mLeft > original.mLeft)
    {
      IntRect newRect = original;
      newRect.mRight = reserve.mLeft;
      mFreeAreas.push_back(newRect);
    }
    // Check for splitting from the bottom
    if(reserve.mBottom < original.mBottom)
    {
      IntRect newRect = original;
      newRect.mTop = reserve.mBottom;
      mFreeAreas.push_back(newRect);
    }
    // Check for splitting from the top
    if(reserve.mTop > original.mTop)
    {
      IntRect newRect = original;
      newRect.mBottom = reserve.mTop;
      mFreeAreas.push_back(newRect);
    }

    return true;
  }

  return false;
}

void AreaAllocator::cleanup()
{
  // Remove rects which are contained within another rect
  for(unsigned i = 0; i < mFreeAreas.size();)
  {
    bool erased = false;
    for(unsigned j = i + 1; j < mFreeAreas.size();)
    {
      if((mFreeAreas[i].mLeft >= mFreeAreas[j].mLeft) &&
        (mFreeAreas[i].mTop >= mFreeAreas[j].mTop) &&
        (mFreeAreas[i].mRight <= mFreeAreas[j].mRight) &&
        (mFreeAreas[i].mBottom <= mFreeAreas[j].mBottom))
      {
        mFreeAreas.erase(mFreeAreas.begin() + i);
        erased = true;
        break;
      }
      if((mFreeAreas[j].mLeft >= mFreeAreas[i].mLeft) &&
        (mFreeAreas[j].mTop >= mFreeAreas[i].mTop) &&
        (mFreeAreas[j].mRight <= mFreeAreas[i].mRight) &&
        (mFreeAreas[j].mBottom <= mFreeAreas[i].mBottom))
        mFreeAreas.erase(mFreeAreas.begin() + j);
      else
        ++j;
    }
    if(!erased)
      ++i;
  }
}

MyFont::MyFont(const std::string &fn,int pointSize,int maxChars) : maxChars(maxChars),fontVao(0),fontDrawCount(0),texture(0){
  //FILE *file=fopen(fn.c_str(),"rb");
  //fseek(file,0L,SEEK_END);

  //unsigned int dataSize = ftell(file);
  //unsigned char *mFontData=new unsigned char[dataSize]; //[1<<20]
  //fseek(file,0L,SEEK_SET);
  //fread(mFontData,1,dataSize,file);//1<<20
  //fclose(file);

  std::ifstream ifs(fn.c_str(),std::ios::binary|std::ios::ate);

  if(!ifs) {
    std::cout << fn << " could not open font.\n";
    return;
  }
  std::ifstream::pos_type pos = ifs.tellg();
  unsigned int fileLen=(unsigned int)pos;
  unsigned char *mFontData = new unsigned char[fileLen];
  ifs.seekg(0,std::ios::beg);
  ifs.read((char *)mFontData,pos);
  ifs.close();

  //
  static const int MAX_FONT_CHARS = 256;
  unsigned short mGlyphIndex[MAX_FONT_CHARS];
  std::vector<FontGlyph> mGlyphs;
//
  stbtt_fontinfo fontInfo;

  // Assume 1 font per file for now
  //if(!stbtt_InitFont(&fontInfo,&mFontData[0],0)) {
  if(!stbtt_InitFont(&fontInfo,mFontData,0)) {
    std::cout << "Could not initialize font\n";
    return;
  }


  std::vector<bool> glyphUsed;
  glyphUsed.resize(fontInfo.numGlyphs);
  for(int i = 0; i < fontInfo.numGlyphs; ++i) {
    glyphUsed[i] = false;
  }


  // Build glyph mapping and mark used glyphs
  //for(int i = 0; i < MAX_FONT_CHARS; ++i)  {
  for(int i = 32; i < 128; ++i)  {
    mGlyphIndex[i] = stbtt_FindGlyphIndex(&fontInfo,i);
    glyphUsed[mGlyphIndex[i]] = true;
  }

  // Get row height
  int ascent,descent,lineGap;
  stbtt_GetFontVMetrics(&fontInfo,&ascent,&descent,&lineGap);

  // Calculate scale (use ascent only)
  float scale = (float)pointSize / ascent;

  // Go through glyphs to get their dimensions & offsets
  //std::cout << fontInfo.numGlyphs << std::endl;
  for(int i = 0; i < fontInfo.numGlyphs; ++i) {
    FontGlyph newGlyph;
    int ix0,iy0,ix1,iy1;
    int advanceWidth,leftSideBearing;

    if(glyphUsed[i]) {
      stbtt_GetGlyphBitmapBox(&fontInfo,i,scale,scale,&ix0,&iy0,&ix1,&iy1);
      stbtt_GetGlyphHMetrics(&fontInfo,i,&advanceWidth,&leftSideBearing);
      newGlyph.mWidth = ix1 - ix0;
      newGlyph.mHeight = iy1 - iy0;
      newGlyph.mOffsetX = (int)(leftSideBearing * scale);
      newGlyph.mOffsetY = iy0;
      newGlyph.mAdvanceX = (int)(advanceWidth * scale);
    } else {
      newGlyph.mWidth = 0;
      newGlyph.mHeight = 0;
      newGlyph.mOffsetX = 0;
      newGlyph.mOffsetY = 0;
      newGlyph.mAdvanceX = 0;
    }

    mGlyphs.push_back(newGlyph);
  }

  //

  int scaledAscent = (int)(scale * ascent);

  //todo 
  /*
  for(int ii = 32; ii < 128; ++ii)  {
    int i=mGlyphIndex[i];
    FontGlyph newGlyph;
    int ix0,iy0,ix1,iy1;
    int advanceWidth,leftSideBearing;

    stbtt_GetGlyphBitmapBox(&fontInfo,i,scale,scale,&ix0,&iy0,&ix1,&iy1);
    stbtt_GetGlyphHMetrics(&fontInfo,i,&advanceWidth,&leftSideBearing);
    newGlyph.mWidth = ix1 - ix0;
    newGlyph.mHeight = iy1 - iy0;
    newGlyph.mOffsetX = (int)(leftSideBearing * scale);
    newGlyph.mOffsetY = iy0 ;
    newGlyph.mOffsetY+= scaledAscent;// Adjust the Y-offset so that the fonts are top-aligned
    newGlyph.mAdvanceX = (int)(advanceWidth * scale);


    newFace.glyphs[i-32]=newGlyph;
  }*/

  // Adjust the Y-offset so that the fonts are top-aligned
  for(int i = 0; i < fontInfo.numGlyphs; ++i) {
    if(glyphUsed[i]) {
      mGlyphs[i].mOffsetY += scaledAscent;
    }

  }

  //
  static const unsigned FONT_TEXTURE_MIN_SIZE = 128;
  static const unsigned FONT_TEXTURE_MAX_SIZE = 2048;

  // Calculate row advance
  newFace.mRowHeight = (int)(scale * (ascent - descent + lineGap));

  // Now try to pack into the smallest possible texture
   texWidth = FONT_TEXTURE_MIN_SIZE;
   texHeight = FONT_TEXTURE_MIN_SIZE;
  bool doubleHorizontal = true;
  for(;;) {
    bool success = true;

    // Check first for theoretical possible fit. If it fails, there is no need to try to fit
    int totalArea = 0;
    for(int i = 0; i < fontInfo.numGlyphs; ++i)
    {
      if((mGlyphs[i].mWidth) && (mGlyphs[i].mHeight))
        totalArea += (mGlyphs[i].mWidth + 1) * (mGlyphs[i].mHeight + 1);
    }

    if(totalArea > texWidth * texHeight) {
      success = false;
    } else {


      AreaAllocator allocator(texWidth,texHeight);
      for(int i = 0; i < fontInfo.numGlyphs; ++i) {
        if((mGlyphs[i].mWidth) && (mGlyphs[i].mHeight))
        {
          int x,y;
          // Reserve an empty border between glyphs for filtering
          if(!allocator.reserve(mGlyphs[i].mWidth + 1,mGlyphs[i].mHeight + 1,x,y)) {
            success = false;
            break;
          } else {
            mGlyphs[i].mX = x;
            mGlyphs[i].mY = y;
          }
        } else {
          mGlyphs[i].mX = 0;
          mGlyphs[i].mY = 0;
        }
      }
    }

    if(!success) {
      // Alternate between doubling the horizontal and the vertical dimension
      if(doubleHorizontal) {
        texWidth <<= 1;
      } else {
        texHeight <<= 1;
      }


      if((texWidth > FONT_TEXTURE_MAX_SIZE) || (texHeight > FONT_TEXTURE_MAX_SIZE)) {
        std::cout << "Font face could not be fit into the largest possible texture\n";
        return;
      }


      doubleHorizontal = !doubleHorizontal;
    } else
      break;
  }
  //
  std::cout << "TexSize " << texWidth << "," <<texHeight << std::endl;
  //



  int  Pitch=texWidth;
  unsigned char *pBits=new unsigned char[texWidth*texHeight];
  memset(pBits,0,texWidth*texHeight);

  // Render glyphs into texture, and find out a scaling value in case font uses less than full opacity (thin outlines)
  int sumOpacity = 0;
  int nonEmptyGlyphs = 0;
  for(int i = 0; i < fontInfo.numGlyphs; ++i)
  {
    if((mGlyphs[i].mWidth) && (mGlyphs[i].mHeight))
    {
      stbtt_MakeGlyphBitmap(&fontInfo,(unsigned char*)pBits + Pitch * mGlyphs[i].mY + mGlyphs[i].mX,mGlyphs[i].mWidth,mGlyphs[i].mHeight,Pitch,scale,scale,i);

      int glyphMaxOpacity = 0;
      for(int y = 0; y < mGlyphs[i].mHeight; ++y)
      {
        unsigned char* pixels = (unsigned char*)pBits + Pitch * (y + mGlyphs[i].mY) + mGlyphs[i].mX;

        for(int x = 0; x < mGlyphs[i].mWidth; ++x)
          glyphMaxOpacity = std::max(glyphMaxOpacity,(int)pixels[x]);
      }

      if(glyphMaxOpacity > 0)
      {
        sumOpacity += glyphMaxOpacity;
        ++nonEmptyGlyphs;
      }
    }
  }

  // Apply the scaling if necessary
  int avgOpacity = nonEmptyGlyphs ? sumOpacity / nonEmptyGlyphs : 255;
  if(avgOpacity < 255)
  {
    float scale = 255.0f / avgOpacity;

    for(int i = 0; i < fontInfo.numGlyphs; ++i)
    {
      for(int y = 0; y < mGlyphs[i].mHeight; ++y)
      {
        unsigned char* dest = (unsigned char*)pBits + Pitch * (y + mGlyphs[i].mY) + mGlyphs[i].mX;
        for(int x = 0; x < mGlyphs[i].mWidth; ++x)
        {
          int pixel = dest[x];
          dest[x] = std::min((int)(pixel * scale),255);
        }
      }
    }
  }
  //

  for(int i=32;i<128;i++) {
    newFace.glyphs[i-32]=mGlyphs[mGlyphIndex[i]];
  }

  //
  glGenTextures(1,&texture);
  glBindTexture(GL_TEXTURE_2D,texture);

  glTexImage2D(GL_TEXTURE_2D,0,GL_RED,texWidth,texHeight,0,GL_RED,GL_UNSIGNED_BYTE,pBits);

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

  // can free mFontData at this point
  //delete[] mFontData;

  //

  glGenVertexArrays(1,&fontVao);
  glGenBuffers(1,&fontVertBuf);
  glGenBuffers(1,&fontIndBuf);
  glBindVertexArray(fontVao);

  glBindBuffer(GL_ARRAY_BUFFER,fontVertBuf);
  glBufferData(GL_ARRAY_BUFFER,maxChars*16*sizeof(float),0,GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(float)*4,0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(float)*4,(const GLvoid*)(sizeof(float)*2));
  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,fontIndBuf);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,maxChars*6*sizeof(int),0,GL_DYNAMIC_DRAW);

   fontDrawCount=0;
}


MyFont::~MyFont() {
  glDeleteVertexArrays(1,&fontVao);
  glDeleteBuffers(1,&fontVertBuf);
  glDeleteBuffers(1,&fontIndBuf);
  glDeleteTextures(1,&texture);
}
GLuint MyFont::getTexture() {
  return texture;
}
GLuint MyFont::getVao() {
  return fontVao;
}
void MyFont::generate(const std::string &text2,float x,float y,bool flipped) {


  const char *text=text2.c_str();
  float *vertData=new float[text2.size()*20];
  unsigned int *indData=new unsigned int[6*text2.size()];

  int c=0;
  int d=0;
  fontDrawCount=0;
  int count=0;
  while(*text && count < maxChars) {
    if(*text=='\n') {
      y+=(float)newFace.mRowHeight;
      x=0.0f;
    } else if(*text >= 32 && *text < 128) {
      stbtt_aligned_quad q;
      
      FontGlyph *glyph = &newFace.glyphs[*text-32];
      float ww=1.0f/(float)texWidth;
      float hh=1.0f/(float)texHeight;
     
     // y= (float)glyph->mHeight;
      if(flipped) {
        q.y0 = floorf(y+(float)glyph->mOffsetY);
        q.y1 = q.y0 + (float)glyph->mHeight;
      } else {
        q.y0 = floorf(y+(float)newFace.mRowHeight-(float)glyph->mOffsetY);
        q.y1 = q.y0 - (float)glyph->mHeight;
         
      }

      if(*text=='b') {
        //q.y1-=(float)glyph->mHeight;
      }
      q.x0 = floorf(x+(float)glyph->mOffsetX);
      q.x1 = q.x0 + (float)glyph->mWidth;


      q.s0 = glyph->mX*ww;
      q.t0 = (glyph->mY + glyph->mHeight)*hh;
      q.s1 = (glyph->mX + glyph->mWidth)*ww;
      q.t1 = glyph->mY*hh;

      if(flipped) {
        //float temp;
        //temp=q.y0;
        //q.y0=q.y1;
        //q.y1=temp;
       // q.y0-=(float)glyph->mHeight*2.0f;
        //q.y1+=(float)glyph->mHeight*2.0f;
        
        //temp=q.t0;
        //q.t0=q.t1;
       // q.t1=temp;
      }
 
      x+=(float)glyph->mAdvanceX;
      //std::cout <<"("<< q.x0 << "," << q.y0 << ") => (" <<q.x1 << "," << q.y1 << ")" << std::endl;
    
      //0
      vertData[c++]=q.x0;
      vertData[c++]=q.y0;
      vertData[c++]=q.s0;
      vertData[c++]=q.t1;

      //1
      vertData[c++]=q.x1;
      vertData[c++]=q.y0;
      vertData[c++]=q.s1;
      vertData[c++]=q.t1;

      //2
      vertData[c++]=q.x1;
      vertData[c++]=q.y1;
      vertData[c++]=q.s1;
      vertData[c++]=q.t0;

      //3
      vertData[c++]=q.x0;
      vertData[c++]=q.y1;
      vertData[c++]=q.s0;
      vertData[c++]=q.t0;

      //

      //
      count++;
    }
    ++text;
  }
  for(int i=0;i<(c/16);i++) {
    unsigned int ind=i*4;

   // if(flipped) {

    //  indData[d++]=ind;
    //  indData[d++]=ind+1;
     // indData[d++]=ind+2;

    //  indData[d++]=ind+0;
    //  indData[d++]=ind+2;
   //   indData[d++]=ind+3;
    //} else {

      indData[d++]=ind+2;
      indData[d++]=ind+1;
      indData[d++]=ind;

      indData[d++]=ind+3;
      indData[d++]=ind+2;
      indData[d++]=ind+0;
  //  }
  }
  //

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER,fontVertBuf);

  //glBufferData(GL_ARRAY_BUFFER,maxChars*16*sizeof(float),vertData,GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER,0,c*sizeof(float),vertData);

  //
 // glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(float)*4,0);//3


 // glEnableVertexAttribArray(0);

 // glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(float)*4,(const GLvoid*)(sizeof(float)*2));
 // glEnableVertexAttribArray(2);

  //
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,fontIndBuf);
  //glBufferData(GL_ELEMENT_ARRAY_BUFFER,maxChars*6*sizeof(int),indData,GL_DYNAMIC_DRAW);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER,0,d*sizeof(float),indData);

  //
  delete[] vertData;
  delete[] indData;




  fontDrawCount=d;
}
void MyFont::draw() {
  glDrawElements(GL_TRIANGLES,fontDrawCount,GL_UNSIGNED_INT,0);
}