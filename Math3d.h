#ifndef MATH3D_H
#define MATH3D_H

#define _USE_MATH_DEFINES
#include <cmath>

#ifndef M_PI_4
#define 	M_PI_4   0.78539816339744830962
#endif

namespace vec3 {
  template<typename T>
  void cross(T *out,const T *a,const T *b) {
    T tmp[3]={ a[1]*b[2]-a[2]*b[1],a[2]*b[0]-a[0]*b[2],a[0]*b[1]-a[1]*b[0] };

    for(int i=0;i<3;i++) {
      out[i]=tmp[i];
    }
  }

  template<typename T>
  T dot(const T *a,const T *b) {
    return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
  }

  template<typename T>
  T length(const T *v) {
    return sqrt(dot(v,v));
  }

  template<typename T>
  void normal(T *out,const T *v) {
    T l=length(v);
    for(int i=0;i<3;i++) {
      out[i]=v[i]/l;
    }
  }

  template<typename T>
  void add(T *out,const T *a,const T *b) {
    for(int i=0;i<3;i++) {
      out[i]=a[i]+b[i];
    }
  }

  template<typename T>
  void sub(T *out,const T *a,const T *b) {
    for(int i=0;i<3;i++) {
      out[i]=a[i]-b[i];
    }
  }

  template<typename T>
  void multiplyScalar(T *out,const T *v,const T s) {
    for(int i=0;i<3;i++) {
      out[i]=v[i]*s;
    }
  }
}

namespace mat4 {
  template<typename T>
  void identity(T *out) {
    for(int i=0;i<16;i++) {
      out[i]=(i%5)?(T)0.0:(T)1.0;
    }
  }

  template<typename T>
  void transpose(T *out,T *m) {
    for(int i=0;i<4;i++){
      for(int j=i+1;j<4;j++){
        T a = m[i*4+j];
        out[i*4+j] = m[j*4+i];
        out[j*4+i] = a;
      }
    }
  }

  template<typename T>
  void multiply(T *out,const T *a,const T *b) {
    T tmp[16];

    for(int i=0;i<4;i++) {
      for(int j=0;j<4;j++){
        T x=(T)0;

        for(int k=0;k<4;k++){
          x+=a[i*4+k]*b[k*4+j];
        }

        tmp[i*4+j]=x;
      }
    }

    for(int i=0;i<16;i++) {
      out[i]=tmp[i];
    }
  }

  template<typename T>
  void multiplyVector(T *out,const T *m,const T *v) {
    T tmp[4];

    for(int i=0;i<4;i++) {
      tmp[i]=(T)0.0;

      for(int j=0;j<4;j++){
        tmp[i]+=m[i*4+j]*v[j];
      }
    }

    for(int i=0;i<4;i++) {
      out[i]=tmp[i];
    }
  }

  template<typename T>
  void multiplyVector(T *out,const T *m,const T *v,const T w) {
    T vv[4]={ v[0],v[1],v[2],w };
    multiplyVector(out,m,vv);
  }

  template<typename T>
  void inverse(T *out,const T *m) {
    T m2[16],tmp[16];
    identity(tmp);

    for(int i=0;i<16;i++) {
      m2[i]=m[i];
    }

    for(int K=0;K<4;++K) {
      T factor = m2[K*4+K];

      for(int q=0;q<4;q++) {
        m2[K*4+q] /= factor;
        tmp[K*4+q] /= factor;
      }

      for(int L=0;L<4;++L){
        if(K==L) {
          continue;
        }

        T coefficient = m2[L*4+K];

        for(int q=0;q<4;q++) {
          m2[L*4+q] -= coefficient*m2[K*4+q];
          tmp[L*4+q] -= coefficient*tmp[K*4+q];
        }
      }
    }

    for(int i=0;i<16;i++) {
      out[i]=tmp[i];
    }
  }

  template<typename T>
  void toMat3(T *out,const T *m) {
    //for(int i=0;i<3;i++) {
    //  for(int j=0;j<3;j++) {
    //    out[i*3+j]=m[i*4+j];
    //  }
    //}


    for(int i = 0; i<9; i++) { 
      out[i] = m[(i / 3) * 4 + i % 3];
    }
  }

  template<typename T>
  void normal(T *out,const T *m) {
    T m2[16];
    inverse(m2,m);
    transpose(m2,m2);

    for(int i=0;i<16;i++) {
      out[i]=m2[i];
    }
  }

  template<typename T>
  void normal3(T *out,const T *m) {
    T m2[16];
    normal(m2,m);
    toMat3(out,m2);
  }

  template<typename T>
  void perspective(T *out,T fov,T aspect,T znear,T zfar) {
    T yScale=(T)1/tan(fov/(T)2);
    T xScale=yScale/aspect;
    T nearmfar=znear-zfar;

    for(int i=0;i<16;i++) {
      out[i]=(T)0;
    }

    out[0]=xScale;
    out[5]=yScale;
    out[10]=(zfar+znear)/nearmfar;
    out[11]=(T)2*zfar*znear/nearmfar;
    out[14]=-(T)1;
  }

  template<typename T>
  void ortho(T *out,T left,T right,T bottom,T top,T nearVal,T farVal) {

    for(int i=0;i<16;i++) {
      out[i]=(T)0;
    }

    //
    T tx = -(right+left)/(right-left);
    T ty = -(top+bottom)/(top-bottom);
    T tz = -(farVal+nearVal)/(farVal-nearVal);
   
    out[0]=(T)2/(right-left);
    out[3]=tx;
    out[5]=(T)2/(top-bottom);
    out[7]=ty;
    out[10]=-(T)2/(nearVal-farVal);
    out[11]=tz;
    out[15]=(T)1;
  }

  template<typename T>
  void ortho2d(T *out,T left,T right,T bottom,T top) {
    ortho(out,left,right,bottom,top,-(T)1,(T)1);
  }

  template<typename T>
  void translate(T *out,const T *v, bool mul=false) {
    T mat[16];
    identity(mat);

    for(int i=0;i<3;i++) {
      mat[3+i*4]=v[i];
    }

    //
    if(mul) {
      multiply(out,out,mat);
    } else {
      for(int i=0;i<16;i++) {
        out[i]=mat[i];
      }
    }
  }
  
  template<typename T>
  void translate(T *out,T x,T y,T z, bool mul=false) {
    T v[3]={ x,y,z };
    translate(out,v,mul);
  }
  
  template<typename T>
  void scale(T *out,const T *v,bool mul=false) {
    T mat[16];
    identity(mat);

    for(int i=0;i<3;i++) {
      mat[i*5]=v[i];
    }

    //
    if(mul) {
      multiply(out,out,mat);
    } else {
      for(int i=0;i<16;i++) {
        out[i]=mat[i];
      }
    }
  }
  
  template<typename T>
  void scale(T *out,T x,T y,T z, bool mul=false) {
    T v[3]={ x,y,z };
    scale(out,v,mul);
  }

  template<typename T>
  void rotateX(T *out,T x, bool mul=false) {
    T mat[16];
    identity(mat);

    T c=cos(x);
    T s=sin(x);

    mat[5]=c;
    mat[6]=-s;
    mat[9]=s;
    mat[10]=c;

    //
    if(mul) {
      multiply(out,out,mat);
    } else {
      for(int i=0;i<16;i++) {
        out[i]=mat[i];
      }
    }
  }
  
  template<typename T>
  void rotateY(T *out,T y,bool mul=false) {
    T mat[16];
    identity(mat);

    T c=cos(y);
    T s=sin(y);

    mat[0]=c;
    mat[2]=s;
    mat[8]=-s;
    mat[10]=c;

    //
    if(mul) {
      multiply(out,out,mat);
    } else {
      for(int i=0;i<16;i++) {
        out[i]=mat[i];
      }
    }
  }
  
  template<typename T>
  void rotateZ(T *out,T z,bool mul=false) {
    T mat[16];
    identity(mat);

    T c=cos(z);
    T s=sin(z);

    mat[0]=c;
    mat[1]=-s;
    mat[4]=s;
    mat[5]=c;

    //
    if(mul) {
      multiply(out,out,mat);
    } else {
      for(int i=0;i<16;i++) {
        out[i]=mat[i];
      }
    }
  }
  
  template<typename T>
  void rotateAxis(T *out,const T *axis,T angle,bool mul=false) {
    T mat[16];
    identity(mat);
    float u=axis[0];
    float v=axis[1];
    float w=axis[2];
    T L=(u*u+v*v+w*w);
    float u2=axis[0]*axis[0];
    float v2=axis[1]*axis[1];
    float w2=axis[2]*axis[2];

    mat[0]=(u2+(v2+w2)*cos(angle))/L;
    mat[1]=(u*v*((T)1-cos(angle))-w*sqrt(L)*sin(angle))/L;
    mat[2]=(u*w*((T)1-cos(angle))+v*sqrt(L)*sin(angle))/L;

    mat[4]=(u*v*((T)1-cos(angle))+w*sqrt(L)*sin(angle))/L;
    mat[5]=(v2+(u2+w2)*cos(angle))/L;
    mat[6]=(v*w*((T)1-cos(angle))-u*sqrt(L)*sin(angle))/L;

    mat[8]=(u*w*((T)1-cos(angle))-v*sqrt(L)*sin(angle))/L;
    mat[9]=(v*w*((T)1-cos(angle))+u*sqrt(L)*sin(angle))/L;
    mat[10]=(w2+(u2+v2)*cos(angle))/L;

    //
    if(mul) {
      multiply(out,out,mat);
    } else {
      for(int i=0;i<16;i++) {
        out[i]=mat[i];
      }
    }
  }

  template<typename T>
  void rotateAxis(T *out,const T x,T y,T z,T angle,bool mul=false) {
    T axis[3]={ x,y,z };
    rotateAxis(out,axis,angle,mul);
  }
}

#endif