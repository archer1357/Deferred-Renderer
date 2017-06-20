#ifndef GEOMETRYVAOMANAGER_H
#define GEOMETRYVAOMANAGER_H

#include <map>
#include <fstream>
#include <sstream>
#include <vector>
#include "Geometry.h"
#include "ShadowGeometry.h"
#include "GeometryVao.h"
#include "Base64.h"
#include <jsoncons/json.hpp>

class GeometryVaoManager {
private:
  std::map<std::string,Geometry*> geoms;
  std::map<std::string,Vao*> vaos;
  std::map<std::string,GeometryVao*> geomVaos;
  std::map<std::string,GLenum> modeMap;
  std::map<std::string,GLenum> typeMap;
  bool convStrMode(const std::string &s, GLenum &out);
  bool convStrType(const std::string &s, GLenum &out);
  Geometry *getGeometry(const std::string &fn);
  Vao *getVao(const std::string &fn);
  std::map<std::string,ShadowGeometry*> shadowGeometries;
  Vao *positionsVao;
public:
  GeometryVaoManager();
  ~GeometryVaoManager();
  GeometryVao *get(const std::string &fn);
  GeometryVao *get(const std::string &geomFn,const std::string &vaoFn);
  void onFileModified(const std::string &fn);
  void clear();
  ShadowGeometry *getShadow(const std::string &geomFn); //for cpu based
};

#endif
