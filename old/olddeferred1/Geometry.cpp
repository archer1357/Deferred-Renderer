#include "Geometry.h"


GeometryVertices::GeometryVertices(GLenum type, int size, int dataSize, const void *data)
  : type(type), size(size)
{
  glBindVertexArray(0);
  glGenBuffers(1,&buffer);
  glBindBuffer(GL_ARRAY_BUFFER,buffer);
  glBufferData(GL_ARRAY_BUFFER,dataSize,data,GL_STATIC_DRAW);
}

GeometryVertices::~GeometryVertices() {
  glDeleteBuffers(1,&buffer);
}

GLenum GeometryVertices::getType() {
  return type;
}

int GeometryVertices::getSize() {
  return size;
}

GLuint GeometryVertices::getBuffer() {
  return buffer;
}

GeometryIndices::GeometryIndices(GLenum type, int dataSize, const void *data)
  : type(type)
{
  glBindVertexArray(0);
  glGenBuffers(1,&buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,dataSize,data,GL_STATIC_DRAW);
}

GeometryIndices::~GeometryIndices() {
  glDeleteBuffers(1,&buffer);
}

GLenum GeometryIndices::getType() {
  return type;
}

GLuint GeometryIndices::getBuffer() {
  return buffer;
}

Geometry::Geometry()
  : indices(0)
{

}

Geometry::~Geometry() {
  for(std::map<std::string,GeometryVertices*>::iterator i=vertices.begin();i!=vertices.end();i++) {
    delete i->second;
  }

  delete indices;

  for(std::map<std::string,GeometryDraw*>::iterator i=draws.begin();i!=draws.end();i++) {
    delete i->second;
  }
}

GeometryVertices *Geometry::getVertices(const std::string &name) {
  std::map<std::string,GeometryVertices*>::iterator it=vertices.find(name);

  if(it == vertices.end()) {
    return 0;
  }

  return it->second;
}

GeometryIndices *Geometry::getIndices() {
  return indices;
}

GeometryDraw *Geometry::getDraw(const std::string &name) {
  std::map<std::string,GeometryDraw*>::iterator it=draws.find(name);

  if(it == draws.end()) {
    return 0;
  }

  return it->second;
}


void Geometry::addVertices(const std::string &name,GLenum type, int size,
                           int dataSize, const void *data) {
  GeometryVertices *v=new GeometryVertices(type,size,dataSize,data);
  vertices.insert(std::make_pair(name,v));


}

void Geometry::setIndices(GLenum type, int dataSize, const void *data) {
  delete indices;
  indices=new GeometryIndices(type,dataSize,data);
}

void Geometry::addDrawElements(std::string name, GLenum mode,
                               int first, int count, GLenum type) {
  GeometryDraw *d=new GeometryDrawElements(mode,first,count,type);
  draws.insert(std::make_pair(name,d));
}

void Geometry::addDrawArrays(std::string name, GLenum mode,
                             int first, int count) {
  GeometryDraw *d=new GeometryDrawArrays(mode,first,count);
  draws.insert(std::make_pair(name,d));
}
