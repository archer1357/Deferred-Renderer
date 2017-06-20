#include "Geometry.h"
#include <list>
#include <iostream>
#include <string>
#include <GL/glew.h>
#include "Vao.h"

class GeometryVao {
private:
  GLuint gvao;
  Geometry *geometry;
public:
  GeometryVao(Geometry *geometry, Vao *vao);
  ~GeometryVao();
  GLuint getVao();
  GeometryDraw *getDraw(const std::string &name);
  GeometryDraw *getDraw();
};
