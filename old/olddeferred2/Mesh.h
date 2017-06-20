#include "Geometry.h"
#include "Draw.h"
#include <GL/glew.h>

class Mesh {
public:
  float color[3];
  float emissive;
  float reflective;
  float shininess;
  GLuint colorTex;
  GLuint normalTex;
  GLuint heightTex;
  GLuint specularTex;
  GLuint vao;
  GeometryDraw *draw;
  float modelViewProjMat[16];
  float normalMat[9];
  float modelViewMat[16];
  float bumpBias,bumpScale;
  enum Material {Color,Normal,Bump,Relief,Parallax} material;
  bool unlit;
  Mesh();
};
