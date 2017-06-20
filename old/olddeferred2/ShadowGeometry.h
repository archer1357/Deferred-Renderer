#ifndef SHADOWGEOMETRY_H
#define SHADOWGEOMETRY_H

#include <GL/glew.h>

class ShadowGeometry {
private:
  GLuint vao,vertexBuffer,indexBuffer;
  float *vertices;
  unsigned int *indices;
  int verticesNum,indicesNum;
public:
  ShadowGeometry();
  ~ShadowGeometry();
  void setVertices(const float *vertices, int verticesSize);
  const float *getVertices();
  void setIndices(const unsigned int *indices, int indicesSize);
  void setIndices(const unsigned short *indices, int indicesSize);
  const unsigned int *getIndices();
  int getVerticesNum();
  int getIndicesNum();
};

#endif
