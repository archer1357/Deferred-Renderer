#include "GeometryVao.h"


GeometryVao::GeometryVao(Geometry *geometry, Vao *vao)
  : geometry(geometry)
{
  glGenVertexArrays(1,&gvao);
  glBindVertexArray(gvao);

  for(Vao::iterator i=vao->begin();i!=vao->end();i++) {
    int index=i->first;
    std::string name=i->second;

    if(GeometryVertices *geometryVertices=geometry->getVertices(name)) {
      GLuint buffer=geometryVertices->getBuffer();
      int size=geometryVertices->getSize();
      GLenum type=geometryVertices->getType();

      // std::cout << name << " " << index << " " << size << " " << type << std::endl;

      glBindBuffer(GL_ARRAY_BUFFER,buffer);
      glVertexAttribPointer(index,size,type,GL_FALSE,0,0);
      glEnableVertexAttribArray(index);
    }
  }

  if(GeometryIndices *indices=geometry->getIndices()) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,indices->getBuffer());
  }
}

GeometryVao::~GeometryVao() {
  glDeleteVertexArrays(1,&gvao);
}

GLuint GeometryVao::getVao() {
  return gvao;
}

GeometryDraw *GeometryVao::getDraw(const std::string &name) {
  return geometry->getDraw(name);
}

GeometryDraw *GeometryVao::getDraw() {
  return getDraw("default");
}
