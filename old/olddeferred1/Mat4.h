#ifndef MAT4_H
#define MAT4_H

#define _USE_MATH_DEFINES
#include <cmath>

namespace mat4 {
  template<typename T>
  void identity(T *out) {
    for(int i=0;i<16;i++) {
      out[i]=(i%5)?(T)0.0:(T)1.0;
    }
  }

  template<typename T>
  void transpose(T *out, T *m) {
    for(int i=0;i<4;i++){
      for(int j=i+1;j<4;j++){
        T a = m[i*4+j];
        out[i*4+j] = m[j*4+i];
        out[j*4+i] = a;
      }
    }
  }

  template<typename T>
  void multiply(T *out, const T *a, const T *b) {
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
  void multiplyVector(T *out, const T *m, const T *v) {
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
  void multiplyVector(T *out, const T *m, const T *v, const T w) {
    T vv[4]={v[0],v[1],v[2],w};
    multiplyVector(out,m,vv);
  }

  template<typename T>
  void inverse(T *out, const T *m) {
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
  void perspective(T *out, T fov,T aspect,T znear,T zfar) {
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
  void translate(T *out, const T *v) {
    identity(out);

    for(int i=0;i<3;i++) {
      out[3+i*4]=v[i];
    }
  }

  template<typename T>
  void scale(T *out, const T *v) {
    identity(out);

    for(int i=0;i<3;i++) {
      out[i*5]=v[i];
    }
  }

  template<typename T>
  void rotateX(T *out, T x) {
    identity(out);

    T c=cos(x);
    T s=sin(x);

    out[5]=c;
    out[6]=-s;
    out[9]=s;
    out[10]=c;
  }

  template<typename T>
  void rotateY(T *out, T y) {
    identity(out);

    T c=cos(y);
    T s=sin(y);

    out[0]=c;
    out[2]=s;
    out[8]=-s;
    out[10]=c;
  }

  template<typename T>
  void rotateZ(T *out, T z) {
    identity(out);

    T c=cos(z);
    T s=sin(z);

    out[0]=c;
    out[1]=-s;
    out[4]=s;
    out[5]=c;
  }

  template<typename T>
  void rotateAxis(T *out, const T *axis, T angle) {
    identity(out);
    float u=axis[0];
    float v=axis[1];
    float w=axis[2];
    T L=(u*u+v*v+w*w);
    float u2=axis[0]*axis[0];
    float v2=axis[1]*axis[1];
    float w2=axis[2]*axis[2];

    out[0]=(u2+(v2+w2)*cos(angle))/L;
    out[1]=(u*v*((T)1-cos(angle))-w*sqrt(L)*sin(angle))/L;
    out[2]=(u*w*((T)1-cos(angle))+v*sqrt(L)*sin(angle))/L;

    out[4]=(u*v*((T)1-cos(angle))+w*sqrt(L)*sin(angle))/L;
    out[5]=(v2+(u2+w2)*cos(angle))/L;
    out[6]=(v*w*((T)1-cos(angle))-u*sqrt(L)*sin(angle))/L;

    out[8]=(u*w*((T)1-cos(angle))-v*sqrt(L)*sin(angle))/L;
    out[9]=(v*w*((T)1-cos(angle))+u*sqrt(L)*sin(angle))/L;
    out[10]=(w2+(u2+v2)*cos(angle))/L;
  }

  template<typename T>
  void toMat3(T *out, const T *m) {
    for(int i=0;i<3;i++) {
      for(int j=0;j<3;j++) {
        out[i*3+j]=m[i*4+j];
      }
    }
  }
}

#endif
