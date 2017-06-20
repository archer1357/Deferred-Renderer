#ifndef SHADOWVOLUMECPUGEOMETRY_H
#define SHADOWVOLUMECPUGEOMETRY_H

#include <GL/glew.h>
#include "Vec3.h"
#include "Mat4.h"
#include <list>
#include <thread>

class ShadowVolumeCpuGeometry {
private:
  GLuint vertexBuffer,vao,indexBuffer;
  float *vertices;
  unsigned int *indices;
  int verticesNum,indicesNum;
  float *edgeNormals;

  struct Edge {unsigned int a, b;};
  struct Face {unsigned int a, b, c;};
  static bool pointlightCalc2(const float *v0,const float *v1,const float *v2, const float *lightPos);
  static bool directionallightCalc2(const float *v0,const float *v1,const float *v2,const float *lightDir);

  static bool pointlightCalc(const float *v0,const float *v1,const float *v2,
                             const float *en0,const float *en1,const float *en2,
                             const float *lightPos);
  static bool directionallightCalc(const float *en0,const float *en1,const float *en2,const float *lightDir);

  static void calcPointLight(int i,bool zpass, bool robust,const float *lightPos,
                                       const float *vertices,const unsigned int *indices,
                                       std::list<Face> &litFaces,std::list<Edge> &silhouetteEdges
                                       );
  static void calcDirLight(int i,bool zpass, bool robust,const float *lightDir,
                                             const float *vertices,const unsigned int *indices,
                             std::list<Face> &litFaces,std::list<Edge> &silhouetteEdges
                                    );
public:
  ShadowVolumeCpuGeometry();
  ~ShadowVolumeCpuGeometry();
  void calcNormals();
  void setVertices(const float *vertices, int verticesSize);
  void setIndices(const unsigned int *indices, int indicesSize);
  void setIndices(const unsigned short *indices, int indicesSize);
  void pointLightRender(const float *lightPos, const float *invModelMat,
                        int threadsNum, bool zpass, bool robust,
                        bool backFace, bool debug);
  void spotLightRender(const float *lightPos, const float *lightDir,
                       const float *invModelMat, int threadsNum,
                       bool zpass, bool robust, bool backFace, bool debug);
  void directionalLightRender(const float *lightDir, const float *invModelMat,
                              int threadsNum, bool zpass, bool robust,
                              bool backFace, bool debug);
};

#endif
