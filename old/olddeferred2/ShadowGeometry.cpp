#include "ShadowGeometry.h"

ShadowGeometry::ShadowGeometry()
  : vertices(0), indices(0), verticesNum(0), indicesNum(0)
{
 // glGenVertexArrays(1,&vao);
 //  glBindVertexArray(vao);

 // glBindVertexArray(0);
 //  glGenBuffers(1,&buffer);
 //  glBindBuffer(GL_ARRAY_BUFFER,buffer);
 //  glBufferData(GL_ARRAY_BUFFER,dataSize,data,GL_STATIC_DRAW);
// glVertexAttribPointer(index,size,type,GL_FALSE,0,0);
//       glEnableVertexAttribArray(index);
  // glBindVertexArray(0);
  // glGenBuffers(1,&buffer);
  // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,buffer);
  // glBufferData(GL_ELEMENT_ARRAY_BUFFER,dataSize,data,GL_STATIC_DRAW);
}


ShadowGeometry::~ShadowGeometry() {
  delete [] vertices;
}

void ShadowGeometry::setVertices(const float *vertices, int verticesSize) {
  delete [] this->vertices;

  verticesNum=(verticesSize/3);

  // this->vertices=new float[verticesNum*4*2];

  // for(int i=0;i<verticesNum;i++) {
  //   for(int j=0;j<3;j++) {
  //     this->vertices[i*4+j]=vertices[i*3+j];
  //     this->vertices[i*4+j+verticesNum*4]=vertices[i*3+j];
  //   }

  //   this->vertices[i*4+3]=0.0f;
  //   this->vertices[i*4+3+verticesNum*4]=1.0f;
  // }



  this->vertices=new float[verticesSize];

  for(int i=0;i<verticesSize;i++) {
    this->vertices[i]=vertices[i];
  }

}

const float *ShadowGeometry::getVertices() {
  return vertices;
}


void ShadowGeometry::setIndices(const unsigned int *indices, int indicesSize) {
  delete [] this->indices;
  this->indices=new unsigned int[indicesSize];
  this->indicesNum=indicesSize;

  for(int i=0;i<indicesSize;i++) {
    this->indices[i]=indices[i];
  }
}

void ShadowGeometry::setIndices(const unsigned short *indices, int indicesSize) {
  delete [] this->indices;
  this->indices=new unsigned int[indicesSize];

  this->indicesNum=indicesSize;

  for(int i=0;i<indicesSize;i++) {
    this->indices[i]=(unsigned int)indices[i];
  }
}

const unsigned int *ShadowGeometry::getIndices() {
  return indices;
}

int ShadowGeometry::getVerticesNum() {
  return verticesNum;
}

int ShadowGeometry::getIndicesNum() {
  return indicesNum;
}
