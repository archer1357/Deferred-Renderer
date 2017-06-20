#ifndef GEOMETRYVAOMANAGER_H
#define GEOMETRYVAOMANAGER_H

#include <map>
#include <fstream>
#include <sstream>
#include <vector>
#include "Geometry.h"
#include "GeometryVao.h"
#include "Base64.h"
#include <jsoncons/json.hpp>
#include "ShadowVolumeCpuGeometry.h"

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
  std::map<std::string,ShadowVolumeCpuGeometry*> shadowVolumeCpuGeometries;
  Vao *positionsVao;
public:
  GeometryVaoManager();
  ~GeometryVaoManager();
  GeometryVao *get(const std::string &geomFn,const std::string &vaoFn);

  void onFileAdded(const std::string &fn);
  void onFileModified(const std::string &fn);
  void onFileDeleted(const std::string &fn);
  void clear();
  ShadowVolumeCpuGeometry *getShadowVolumeCpuGeometry(const std::string &geomFn);
};

#endif
