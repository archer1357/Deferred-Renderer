#include "Draw.h"

GeometryDraw::GeometryDraw(GLenum mode,int first,int count)
  : mode(mode), first(first), count(count)
{

}

GeometryDraw::~GeometryDraw() {

}

void GeometryDraw::setCount(int count) {
  this->count=count;
}

GeometryDrawArrays::GeometryDrawArrays(GLenum mode, int first, int count)
  : GeometryDraw(mode,first,count) {

}
void GeometryDrawArrays::draw() {
  glDrawArrays(mode,first,count);
}

GeometryDrawElements::GeometryDrawElements(GLenum mode, int first, int count, GLenum type)
  : GeometryDraw(mode,first,count), type(type)
{
  if(type==GL_UNSIGNED_INT) {
    this->first=sizeof(int)*first;
  } else if(type==GL_UNSIGNED_SHORT) {
    this->first=sizeof(short)*first;
  } else if(type==GL_UNSIGNED_BYTE) {
    this->first=sizeof(char)*first;
  }
}

void GeometryDrawElements::draw() {
 // if(mode==GL_TRIANGLES_ADJACENCY) {
 ///   glDrawElementsBaseVertex(mode,count,type, (void*)((intptr_t)first),0);
 // } else {
  glDrawElements(mode,count,type, (void*)((intptr_t)first));
 // }
}
