#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <map>
#include <iostream>
#include <string>
#include <GL/glew.h>
#include "Draw.h"

class GeometryVertices {
private:
  GLenum type;
  int size;
  GLuint buffer;
public:
  GeometryVertices(GLenum type, int size, int dataSize, const void *data);
  ~GeometryVertices();
  GLenum getType();
  int getSize();
  GLuint getBuffer();
};

class GeometryIndices {
private:
  GLenum type;
  GLuint buffer;
public:
  GeometryIndices(GLenum type, int dataSize, const void *data);
  ~GeometryIndices();
  GLenum getType();
  GLuint getBuffer();
};

class Geometry {
private:
  std::map<std::string,GeometryVertices*> vertices;
  GeometryIndices *indices;
  std::map<std::string,GeometryDraw*> draws;
public:
  Geometry();
  ~Geometry();
  GeometryVertices *getVertices(const std::string &name);
  GeometryIndices *getIndices();
  GeometryDraw *getDraw(const std::string &name);

  void addVertices(const std::string &name, GLenum type, int size,
                   int dataSize, const void *data);
  void setIndices(GLenum type, int dataSize, const void *data);
  void addDrawElements(std::string name, GLenum mode, int first, int count, GLenum type);
  void addDrawArrays(std::string name, GLenum mode, int first, int count);
};
#endif
