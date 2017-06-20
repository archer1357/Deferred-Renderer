#include "CpuShadow.h"
#include <cml/cml.h>

struct Edge {unsigned int a, b;};
struct Face {unsigned int a, b, c;};

CpuShadow::CpuShadow() {
  vao=0;

  for(int i=0;i<16;i++) {
    modelViewMat[i]=0.0f;
    modelMat[i]=0.0f;
  }

  modelViewMat[0]=1.0f;
  modelViewMat[5]=1.0f;
  modelViewMat[10]=1.0f;
  modelViewMat[15]=1.0f;

  modelMat[0]=1.0f;
  modelMat[5]=1.0f;
  modelMat[10]=1.0f;
  modelMat[15]=1.0f;


  // draw=0;
  shadowGeometry=0;

  vertsNum=0;
  glGenVertexArrays(1,&vao);
  glGenBuffers(1,&vertBuf);
}
CpuShadow::~CpuShadow() {
  glDeleteVertexArrays(1,&vao);
  glDeleteBuffers(1,&vertBuf);
}
void CpuShadow::calc(Pointlight *pl, bool debug) {


  int shadowVertsNum=0;
  float *shadowVerts=0;

  ShadowGeometry *geom=shadowGeometry;
  const float *vertData=geom->getVertices();
  int vertDataSize=geom->getVerticesNum()*4*sizeof(float);
  const unsigned int *indData=geom->getIndices();
  int indDataSize=geom->getIndicesNum()*sizeof(int);
  int indicesCount=geom->getIndicesNum();

  //
  std::list<Face> litFaces;
  std::list<Edge> silhouetteEdges;
  {
    int trianglesNum=geom->getIndicesNum()/3;
    const float *vertices=geom->getVertices();
    const unsigned int *indices=geom->getIndices();

    //invModelMat
    cml::matrix44f invModelMat;

    for(int i=0;i<16;i++) {
      invModelMat.data()[i]=modelMat[i];
    }

    invModelMat.inverse();

    //lightPos
    cml::vector3f lightPos;

    {
      cml::vector4f lightPos2(pl->pos[0],pl->pos[1],pl->pos[2],1.0);
      lightPos2=invModelMat*lightPos2;

      for(int i=0;i<3;i++) {
        lightPos.data()[i]=lightPos2.data()[i];
      }
    }

    //
    for(int i=0;i<trianglesNum;i++) {
      //
      const cml::vector3f pt0(&vertices[indices[i*3]*3]);
      const cml::vector3f pt1(&vertices[indices[i*3+1]*3]);
      const cml::vector3f pt2(&vertices[indices[i*3+2]*3]);

      cml::vector3f n=cml::cross(pt1-pt0,pt2-pt0);
      n.normalize();
      float d=cml::dot(lightPos,n);

      if(d>0) {
        Face face={indices[i*3],indices[i*3+1],indices[i*3+2]};
        litFaces.push_back(face);

        Edge newEdges[] = {
          {indices[i*3+0], indices[i*3+1]},
          {indices[i*3+1], indices[i*3+2]},
          {indices[i*3+2], indices[i*3+0]}};

        for(int j=0;j<3;j++) {
          Edge newEdge=newEdges[j];
          bool removed=false;
          std::list<Edge>::iterator k=silhouetteEdges.begin();

          while(k!=silhouetteEdges.end()) {
            Edge kk=*k;

            if((newEdge.a==kk.a && newEdge.b==kk.b) ||
               (newEdge.a==kk.b && newEdge.b==kk.a)) {
              k=silhouetteEdges.erase(k);
              removed=true;
              break;
            } else {
              k++;
            }
          }

          if(!removed) {
            silhouetteEdges.push_back(newEdge);
          }
        }
      }
    }

    //
    int s=0;
    s+=litFaces.size()*3; //lit faces
    s+=litFaces.size()*3; //extruded lit faces
    s+=silhouetteEdges.size()*6; //extruded edges
    float *outVerts=new float[s*4];
    shadowVerts=outVerts;
    shadowVertsNum=s;

    int c=0;

    //lit faces
    for(std::list<Face>::iterator i=litFaces.begin();i!=litFaces.end();i++) {
      //a
      for(int j=0;j<3;j++) {
        outVerts[c]=vertices[(*i).a*3+j];
        c++;
      }

      outVerts[c]=0;
      c++;

      //b
      for(int j=0;j<3;j++) {
        outVerts[c]=vertices[(*i).b*3+j];
        c++;
      }

      outVerts[c]=0;
      c++;


      //c
      for(int j=0;j<3;j++) {
        outVerts[c]=vertices[(*i).c*3+j];
        c++;
      }

      outVerts[c]=0;
      c++;
    }

    //extruded lit faces
    for(std::list<Face>::iterator i=litFaces.begin();
        i!=litFaces.end();i++) {

      //c
      for(int j=0;j<3;j++) {
        outVerts[c]=vertices[(*i).c*3+j];
        c++;
      }

      outVerts[c]=1;
      c++;

      //b
      for(int j=0;j<3;j++) {
        outVerts[c]=vertices[(*i).b*3+j];
        c++;
      }

      outVerts[c]=1;
      c++;

      //a
      for(int j=0;j<3;j++) {
        outVerts[c]=vertices[(*i).a*3+j];
        c++;
      }

      outVerts[c]=1;
      c++;
    }

    //extruded edges
    for(std::list<Edge>::iterator i=silhouetteEdges.begin();
        i!=silhouetteEdges.end();i++) {
      //
      //a0
      for(int j=0;j<3;j++) {
        outVerts[c]=vertices[(*i).a*3+j];
        c++;
      }

      outVerts[c]=0;
      c++;

      //a1
      for(int j=0;j<3;j++) {
        outVerts[c]=vertices[(*i).a*3+j];
        c++;
      }

      outVerts[c]=1;
      c++;

      //b0
      for(int j=0;j<3;j++) {
        outVerts[c]=vertices[(*i).b*3+j];
        c++;
      }

      outVerts[c]=0;
      c++;

      //

      //b0
      for(int j=0;j<3;j++) {
        outVerts[c]=vertices[(*i).b*3+j];
        c++;
      }

      outVerts[c]=0;
      c++;

      //a1
      for(int j=0;j<3;j++) {
        outVerts[c]=vertices[(*i).a*3+j];
        c++;
      }

      outVerts[c]=1;
      c++;

      //b1
      for(int j=0;j<3;j++) {
        outVerts[c]=vertices[(*i).b*3+j];
        c++;
      }

      outVerts[c]=1;
      c++;

    }
  }


  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER,vertBuf);

  glBufferData(GL_ARRAY_BUFFER,shadowVertsNum*4*sizeof(float),shadowVerts,GL_DYNAMIC_DRAW);
  glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,0,0);
  glEnableVertexAttribArray(0);

  delete []shadowVerts;

  if(debug) {
  this->vertsNum=shadowVertsNum;
  } else {
  this->vertsNum=shadowVertsNum;
  }
}
void CpuShadow::calc(Spotlight *sl, bool debug) {

}
void CpuShadow::draw() {
  glDrawArrays(GL_TRIANGLES,0,vertsNum);

}
