#include "ShadowVolumeCpuGeometry.h"


bool ShadowVolumeCpuGeometry::pointlightCalc2(const float *v0,const float *v1,const float *v2,
                                              const float *lightPos) {

  float ns[3][3],d[3][3];

  float sub10[3],sub20[3],sub21[3],sub01[3],sub02[3],sub12[3];
  vec3::sub(sub10,v1,v0);
  vec3::sub(sub20,v2,v0);
  vec3::sub(sub21,v2,v1);
  vec3::sub(sub01,v0,v1);
  vec3::sub(sub02,v0,v2);
  vec3::sub(sub12,v1,v2);

  vec3::cross(ns[0],sub10,sub20);
  vec3::cross(ns[1],sub21,sub01);
  vec3::cross(ns[2],sub02,sub12);


  vec3::sub(d[0],lightPos,v0);
  vec3::sub(d[1],lightPos,v1);
  vec3::sub(d[2],lightPos,v2);

  return (vec3::dot(ns[0],d[0])>0.0f || vec3::dot(ns[1],d[1])>0.0f || vec3::dot(ns[2],d[2])>0.0f);

  // return vec3::dot(ns[0],d[0])>0.0f;
}

bool ShadowVolumeCpuGeometry::directionallightCalc2(const float *v0,const float *v1,const float *v2,
                                                    const float *lightDir) {

  float ns[3][3];

  float sub10[3],sub20[3],sub21[3],sub01[3],sub02[3],sub12[3];
  vec3::sub(sub10,v1,v0);
  vec3::sub(sub20,v2,v0);
  vec3::sub(sub21,v2,v1);
  vec3::sub(sub01,v0,v1);
  vec3::sub(sub02,v0,v2);
  vec3::sub(sub12,v1,v2);

  vec3::cross(ns[0],sub10,sub20);
  vec3::cross(ns[1],sub21,sub01);
  vec3::cross(ns[2],sub02,sub12);


  return (vec3::dot(ns[0],lightDir)>0.0f ||
          vec3::dot(ns[1],lightDir)>0.0f ||
          vec3::dot(ns[2],lightDir)>0.0f);


  // return vec3::dot(ns[0],lightDir)>0.0f;

}

bool ShadowVolumeCpuGeometry::pointlightCalc(const float *v0,const float *v1,const float *v2,
                                             const float *en0,const float *en1,const float *en2,
                                             const float *lightPos) {
  const float *ns[3]={en0,en1,en2};

  float d[3][3];
  vec3::sub(d[0],lightPos,v0);
  vec3::sub(d[1],lightPos,v1);
  vec3::sub(d[2],lightPos,v2);

  return (vec3::dot(ns[0],d[0])>0.0f || vec3::dot(ns[1],d[1])>0.0f || vec3::dot(ns[2],d[2])>0.0f);

}
bool ShadowVolumeCpuGeometry::directionallightCalc(const float *en0,const float *en1,const float *en2,const float *lightDir) {


  const float *ns[3]={en0,en1,en2};
  return (vec3::dot(ns[0],lightDir)>0.0f ||
          vec3::dot(ns[1],lightDir)>0.0f ||
          vec3::dot(ns[2],lightDir)>0.0f);
}


void ShadowVolumeCpuGeometry::calcPointLight(int i,bool zpass, bool robust,const float *lightPos,
                                                       const float *vertices,const unsigned int *indices,
                                                       std::list<Face> &litFaces,std::list<Edge> &silhouetteEdges
                                                       ) {
  const unsigned int *adjInds=&indices[i*6];

  const float *inVerts[6];

  for(int j=0;j<6;j++) {
    unsigned int a=adjInds[j];
    inVerts[j]=(a==0)?0:&vertices[a*3]; //index 0 is used for missing neighbours

  }

  bool faces_light = true;
  bool waaa=pointlightCalc2(inVerts[0],inVerts[2],inVerts[4], lightPos);
  if(!waaa) {
    // Not facing the light and not robust, ignore.
    if(!robust) {
      //continue;
      return;
    } else if(!zpass) {
      faces_light = false;
      // Flip vertex winding order in or_pos.
      if(waaa) {
        Face face1={adjInds[0],adjInds[4],adjInds[2]};
        litFaces.push_back(face1);
      }
    }
  } else if(!zpass) {
    if(waaa) {
      Face face1={adjInds[0],adjInds[2],adjInds[4]};
      litFaces.push_back(face1);
    }
  }

  for(int j=0; j<3; j++) {
    // Compute indices of neighbor triangle.
    int v0 = j*2;
    int nb = (j*2+1);
    int v1 = (j*2+2) % 6;

    if(!inVerts[nb] ||
       faces_light != pointlightCalc2(inVerts[v0],inVerts[nb],inVerts[v1], lightPos)) {
      // Make sure sides are oriented correctly.
      int i0 = faces_light ? v0 : v1;
      int i1 = faces_light ? v1 : v0;

      Edge newEdge = {adjInds[i0],adjInds[i1]};
      silhouetteEdges.push_back(newEdge);
    }
  }
}

void ShadowVolumeCpuGeometry::calcDirLight(int i,bool zpass, bool robust,const float *lightDir,
                                                             const float *vertices,const unsigned int *indices,
                                                             std::list<Face> &litFaces,std::list<Edge> &silhouetteEdges
                                                             ) {
  const unsigned int *adjInds=&indices[i*6];

  const float *inVerts[6];

  for(int j=0;j<6;j++) {
    unsigned int a=adjInds[j];
    inVerts[j]=(a==0)?0:&vertices[a*3]; //index 0 is used for missing neighbours

  }

  bool faces_light = true;
  bool waaa=directionallightCalc2(inVerts[0],inVerts[2],inVerts[4], lightDir);
  if(!waaa) {
    // Not facing the light and not robust, ignore.
    if(!robust) {
      //continue;
      return;
    } else if(!zpass) {
      faces_light = false;
      // Flip vertex winding order in or_pos.

      if(waaa) {
        Face face1={adjInds[0],adjInds[4],adjInds[2]};
        litFaces.push_back(face1);
      }
    }
  } else if(!zpass) {
    if(waaa) {
      Face face1={adjInds[0],adjInds[2],adjInds[4]};
      litFaces.push_back(face1);
    }
  }

  for(int j=0; j<3; j++) {
    // Compute indices of neighbor triangle.
    int v0 = j*2;
    int nb = (j*2+1);
    int v1 = (j*2+2) % 6;

    if(!inVerts[nb] ||
       faces_light != directionallightCalc2(inVerts[v0],inVerts[nb],inVerts[v1], lightDir)) {
      // Make sure sides are oriented correctly.
      int i0 = faces_light ? v0 : v1;
      int i1 = faces_light ? v1 : v0;

      Edge newEdge = {adjInds[i0],adjInds[i1]};
      silhouetteEdges.push_back(newEdge);
    }
  }
}

ShadowVolumeCpuGeometry::ShadowVolumeCpuGeometry()
  : vertices(0), indices(0), verticesNum(0), indicesNum(0), edgeNormals(0)
{
  glGenBuffers(1,&vertexBuffer);
  glGenBuffers(1,&indexBuffer);
  glGenVertexArrays(1,&vao);


}

ShadowVolumeCpuGeometry::~ShadowVolumeCpuGeometry() {
  delete [] vertices;
  delete [] indices;
  glDeleteBuffers(1,&vertexBuffer);
  glDeleteBuffers(1,&indexBuffer);
  glDeleteVertexArrays(1,&vao);

  delete [] edgeNormals;
}

void ShadowVolumeCpuGeometry::calcNormals() {
  edgeNormals=new float[3];
}
void ShadowVolumeCpuGeometry::setVertices(const float *vertices, int verticesSize) {
  delete [] this->vertices;

  verticesNum=(verticesSize/4);
  this->vertices=new float[verticesNum*3];

  for(int i=0;i<verticesNum;i++) {
    for(int j=0;j<3;j++) {
      this->vertices[i*3+j]=vertices[i*4+j];
    }
  }

  //
  float *vertices2=new float[verticesSize*2];

  for(int i=0;i<verticesNum;i++) {
    for(int j=0;j<3;j++) {
      vertices2[i*4+j]=vertices[i*4+j];
      vertices2[i*4+j+verticesSize]=vertices[i*4+j];
    }

    vertices2[i*4+3]=0.0f;
    vertices2[i*4+3+verticesSize]=1.0f;
  }

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER,vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER,verticesSize*2*sizeof(float),vertices2,GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER,0);

  delete [] vertices2;
}
void ShadowVolumeCpuGeometry::setIndices(const unsigned int *indices, int indicesSize) {
  delete [] this->indices;
  this->indices=new unsigned int[indicesSize];
  this->indicesNum=indicesSize;

  for(int i=0;i<indicesSize;i++) {
    this->indices[i]=indices[i];
  }

}
void ShadowVolumeCpuGeometry::setIndices(const unsigned short *indices, int indicesSize) {
  delete [] this->indices;
  this->indices=new unsigned int[indicesSize];

  this->indicesNum=indicesSize;

  for(int i=0;i<indicesSize;i++) {
    this->indices[i]=(indices[i]==(unsigned short)-1)?(unsigned int)-1:(unsigned int)indices[i];
  }
}


void ShadowVolumeCpuGeometry::pointLightRender(const float *lightPos, const float *invModelMat, int threadsNum, bool zpass, bool robust, bool backFace, bool debug) {


  //
  std::list<Face> *litFaces=new std::list<Face>[threadsNum];
  std::list<Edge> *silhouetteEdges=new  std::list<Edge>[threadsNum];

  //

  //lightPos
  float lightPos2[4];
  mat4::multiplyVector(lightPos2,invModelMat,lightPos,1.0f);

  //
  int trianglesNum=indicesNum/6;

  //
  if(threadsNum==1) {
    for(int i=0;i<trianglesNum;i++) {
      calcPointLight(i,zpass, robust,lightPos2,
                               vertices,indices,litFaces[0],silhouetteEdges[0]);
    }
  } else {
    int trianglesPerThread=trianglesNum/threadsNum;
    std::list<std::thread> mythreads;

    for(int i=0;i<threadsNum;i++) {
      mythreads.push_back(std::thread([&](int ii){
            int start=ii*trianglesPerThread;
            int end=(ii+1==threadsNum)?trianglesNum:(ii+1)*trianglesPerThread;

            for(int j=start;j<end;j++) {
              calcPointLight(j,zpass, robust,lightPos2,
                                       vertices,indices,litFaces[ii],silhouetteEdges[ii]);
            }
          },i));
    }

    for(auto &i : mythreads) {
      i.join();
    }
  }

  //
  unsigned int *outInds=0;
  int indsNum=0;
  int ccc=0;
  if(!debug) {
    for(int j=0;j<threadsNum;j++) {
      indsNum+=litFaces[j].size()*3; //lit faces
      indsNum+=litFaces[j].size()*3; //extruded lit faces
      indsNum+=silhouetteEdges[j].size()*6; //extruded edges
    }
    outInds=new unsigned int[indsNum];

    for(int j=0;j<threadsNum;j++) {
      //faces
      for(std::list<Face>::iterator i=litFaces[j].begin();i!=litFaces[j].end();i++) {
        outInds[ccc++]=(*i).a;
        outInds[ccc++]=(*i).b;
        outInds[ccc++]=(*i).c;

        outInds[ccc++]=(*i).a+verticesNum;
        outInds[ccc++]=(*i).c+verticesNum;
        outInds[ccc++]=(*i).b+verticesNum;
      }

      //edges
      for(std::list<Edge>::iterator i=silhouetteEdges[j].begin();
          i!=silhouetteEdges[j].end();i++) {
        //
        outInds[ccc++]=(*i).a;
        outInds[ccc++]=(*i).a+verticesNum;
        outInds[ccc++]=(*i).b;

        //
        outInds[ccc++]=(*i).b;
        outInds[ccc++]=(*i).a+verticesNum;
        outInds[ccc++]=(*i).b+verticesNum;
      }
    }
  } else {
    for(int j=0;j<threadsNum;j++) {
      //indsNum+=litFaces[j].size()*6; //lit faces
      //indsNum+=litFaces[j].size()*6; //extruded lit faces
      //indsNum+=silhouetteEdges[j].size()*10; //extruded edges

      indsNum+=litFaces[j].size()*4; //lit faces
      indsNum+=litFaces[j].size()*4; //extruded lit faces
      indsNum+=silhouetteEdges[j].size()*6; //extruded edges
    }

    outInds=new unsigned int[indsNum];

    for(int j=0;j<threadsNum;j++) {
      //faces
      for(std::list<Face>::iterator i=litFaces[j].begin();i!=litFaces[j].end();i++) {
        //outInds[ccc++]=(*i).a;
        //outInds[ccc++]=(*i).b;

        //outInds[ccc++]=(*i).b;
        //outInds[ccc++]=(*i).c;

        //outInds[ccc++]=(*i).c;
        //outInds[ccc++]=(*i).a;

        //
        //outInds[ccc++]=(*i).a+verticesNum;
        //outInds[ccc++]=(*i).b+verticesNum;

        //outInds[ccc++]=(*i).b+verticesNum;
        //outInds[ccc++]=(*i).c+verticesNum;

        //outInds[ccc++]=(*i).c+verticesNum;
        //outInds[ccc++]=(*i).a+verticesNum;

        ////
        outInds[ccc++]=(*i).a;
        outInds[ccc++]=(*i).b;

        outInds[ccc++]=(*i).b;
        outInds[ccc++]=(*i).c;

        //
        outInds[ccc++]=(*i).a+verticesNum;
        outInds[ccc++]=(*i).c+verticesNum;

        outInds[ccc++]=(*i).c+verticesNum;
        outInds[ccc++]=(*i).b+verticesNum;

      }

      //edges
      for(std::list<Edge>::iterator i=silhouetteEdges[j].begin();
          i!=silhouetteEdges[j].end();i++) {
        //outInds[ccc++]=(*i).a;
        //outInds[ccc++]=(*i).a+verticesNum;

        //outInds[ccc++]=(*i).b;
        //outInds[ccc++]=(*i).b+verticesNum;

        //outInds[ccc++]=(*i).a;
        //outInds[ccc++]=(*i).b;

        //outInds[ccc++]=(*i).a+verticesNum;
        //outInds[ccc++]=(*i).b+verticesNum;

        //outInds[ccc++]=(*i).a;
        //outInds[ccc++]=(*i).b+verticesNum;

        ////
        outInds[ccc++]=(*i).a;
        outInds[ccc++]=(*i).a+verticesNum;

        outInds[ccc++]=(*i).a+verticesNum;
        outInds[ccc++]=(*i).b;

        outInds[ccc++]=(*i).b;
        outInds[ccc++]=(*i).b+verticesNum;

      }
    }
  }

  delete []litFaces;
  delete []silhouetteEdges;

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER,vertexBuffer);
  glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,0,0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,indexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(int)*indsNum,outInds,GL_STATIC_DRAW);
  delete [] outInds;

  if(debug) {
    glDrawElements(GL_LINES,indsNum,GL_UNSIGNED_INT,0);
  } else {
    glDrawElements(GL_TRIANGLES,indsNum,GL_UNSIGNED_INT,0);
  }
}
void ShadowVolumeCpuGeometry::spotLightRender(const float *lightPos, const float *lightDir, const float *invModelMat, int threadsNum, bool zpass, bool robust, bool backFace, bool debug) {
  pointLightRender(lightPos, invModelMat, threadsNum, zpass, robust, backFace, debug);
}
void ShadowVolumeCpuGeometry::directionalLightRender(const float *lightDir, const float *invModelMat, int threadsNum, bool zpass, bool robust, bool backFace, bool debug) {


  //
  std::list<Face> *litFaces=new std::list<Face>[threadsNum];
  std::list<Edge> *silhouetteEdges=new  std::list<Edge>[threadsNum];

  //lightDir (different)
  float lightDir2[4];
  mat4::multiplyVector(lightDir2,invModelMat,lightDir,0.0f);

  //
  int trianglesNum=indicesNum/6;

  //
  if(threadsNum==1) {
    for(int i=0;i<trianglesNum;i++) {
      //(differentish)
      calcDirLight(i,zpass, robust,lightDir2,
                                     vertices,indices,litFaces[0],silhouetteEdges[0]);
    }
  } else {
    int trianglesPerThread=trianglesNum/threadsNum;
    std::list<std::thread> mythreads;

    for(int i=0;i<threadsNum;i++) {
      mythreads.push_back(std::thread([&](int ii){
            int start=ii*trianglesPerThread;
            int end=(ii+1==threadsNum)?trianglesNum:(ii+1)*trianglesPerThread;

            for(int j=start;j<end;j++) {
              //differentish
              calcDirLight(j,zpass, robust,lightDir2,
                                             vertices,indices,litFaces[ii],silhouetteEdges[ii]);
            }
          },i));
    }

    for(auto &i : mythreads) {
      i.join();
    }
  }

  //
  unsigned int *outInds=0;
  int indsNum=0;
  int ccc=0;


  if(!debug) {
    for(int j=0;j<threadsNum;j++) {
      indsNum+=litFaces[j].size()*3; //lit faces (different)
      indsNum+=silhouetteEdges[j].size()*3; //extruded edges (different)
    }

    outInds=new unsigned int[indsNum];

    for(int j=0;j<threadsNum;j++) {
      //faces
      for(std::list<Face>::iterator i=litFaces[j].begin();i!=litFaces[j].end();i++) {
        outInds[ccc++]=(*i).a;
        outInds[ccc++]=(*i).b;
        outInds[ccc++]=(*i).c;

        //different
      }

      //edges
      for(std::list<Edge>::iterator i=silhouetteEdges[j].begin();
          i!=silhouetteEdges[j].end();i++) {

        //
        outInds[ccc++]=(*i).a;
        outInds[ccc++]=(*i).a+verticesNum; //doesnt actually use the vertex
        outInds[ccc++]=(*i).b;

      }
    }
  } else {
    for(int j=0;j<threadsNum;j++) {
      //indsNum+=litFaces[j].size()*6; //lit faces (diff)
      //indsNum+=silhouetteEdges[j].size()*6; //extruded edges (diff)
      indsNum+=litFaces[j].size()*4; //lit faces (diff)
      indsNum+=silhouetteEdges[j].size()*4; //extruded edges (diff)
    }

    outInds=new unsigned int[indsNum];

    for(int j=0;j<threadsNum;j++) {
      //faces
      for(std::list<Face>::iterator i=litFaces[j].begin();i!=litFaces[j].end();i++) {
        //outInds[ccc++]=(*i).a;
        //outInds[ccc++]=(*i).b;

        //outInds[ccc++]=(*i).b;
        //outInds[ccc++]=(*i).c;

        //outInds[ccc++]=(*i).c;
        //outInds[ccc++]=(*i).a;

        ////
        outInds[ccc++]=(*i).a;
        outInds[ccc++]=(*i).b;

        outInds[ccc++]=(*i).b;
        outInds[ccc++]=(*i).c;

      }

      //edges
      for(std::list<Edge>::iterator i=silhouetteEdges[j].begin();
          i!=silhouetteEdges[j].end();i++) {

        //outInds[ccc++]=(*i).a;
        //outInds[ccc++]=(*i).a+verticesNum; //doesnt actually use the vertex

        //outInds[ccc++]=(*i).a+verticesNum; //doesnt actually use the vertex
        //outInds[ccc++]=(*i).b;

        //outInds[ccc++]=(*i).b;
        //outInds[ccc++]=(*i).a;

        ////
        outInds[ccc++]=(*i).a;
        outInds[ccc++]=(*i).a+verticesNum; //doesnt actually use the vertex

        outInds[ccc++]=(*i).a+verticesNum; //doesnt actually use the vertex
        outInds[ccc++]=(*i).b;
      }
    }
  }

  delete []litFaces;
  delete []silhouetteEdges;

  // glBindVertexArray(0);
  // float viewDir2[4]={dl->viewDir[0],dl->viewDir[1],dl->viewDir[2],1.0f};
  // glBindBuffer(GL_ARRAY_BUFFER,geometry->getVertexBuffer());
  // glBufferSubData(GL_ARRAY_BUFFER,geometry->getVerticesNum()*4*sizeof(float),
  //                 4*sizeof(float),viewDir2);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER,vertexBuffer);
  glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,0,0);
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,indexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(int)*indsNum,outInds,GL_STATIC_DRAW);
  delete [] outInds;

  if(debug) {
    glDrawElements(GL_LINES,indsNum,GL_UNSIGNED_INT,0);
  } else {
    glDrawElements(GL_TRIANGLES,indsNum,GL_UNSIGNED_INT,0);
  }

}
