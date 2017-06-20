#ifndef GEOMETRYMANAGER_H
#define GEOMETRYMANAGER_H

#include "gl_core_3_3.h"
#include <list>
#include <set>
#include <string>
#include <map>
#include <cstdint>

/*
todo: look at having drawhandles know about layouts, for when invalid layouts are given, to not draw and avoid an exception. Or if a layout is invalid look at binding some fake positions...
*/

class GeometryManager {

private:
  struct Geometry {
    bool ok;
    struct Vertices { GLenum type; int size; GLuint buffer; };
    struct Indices { GLenum type; GLuint buffer; };
    struct Draw { GLenum mode; int first,count; };
    std::map<std::string,Vertices*> vertices;
    Indices *indices;
    std::map<std::string,Draw*> draws;
    std::set<std::string> layoutDeps;
  };
  struct Layout {
    bool ok;
    std::list<std::pair<int,std::string>> attribs;
    std::set<std::string> geometryDeps;
  };
public:
  class DrawHandle {
  public:
    Geometry *geometry;
    Geometry::Draw *geometryDraw;
    bool invalid;
  public:
    DrawHandle();
    DrawHandle(Geometry *geometry,Geometry::Draw *geometryDraw);
    DrawHandle(const DrawHandle& other);
    void draw();
  };
private:
  std::map<std::string,Layout*> layouts;
  std::map<std::string,Geometry*> geometries;
  std::map<std::string,GLuint> vaos;
  int maxAttribs;
public:
  GeometryManager();
  ~GeometryManager();
  void clear();
  GLuint getVao(const std::string &geometryFn,const std::string &layoutFn);
  DrawHandle getDraw(const std::string &geometryFn,const std::string &drawName);
private:
  int getMaxAttribs();
  void releaseGeometryBuffers(Geometry *geometry);
  void deleteGeometry(Geometry *geometry);
  Layout *getLayout(const std::string &fn);
  Geometry *getGeometry(const std::string &fn);
  bool loadGeometry(Geometry *geometry,const std::string &fn);
  bool loadLayout(Layout *layout,const std::string &fn);
  bool loadVao(GLuint vao,const std::string &geometryFn,const std::string &layoutFn);
  bool reloadVao(const std::string &geometryFn,const std::string &layoutFn);
public:
  bool reloadLayout(const std::string &fn);
  bool reloadGeometry(const std::string &fn);
  bool removeVao(const std::string &geometryFn,const std::string &layoutFn);
};

typedef GeometryManager::DrawHandle GeometryDraw;
#endif
