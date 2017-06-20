#ifndef VEC3_H
#define VEC3_H

#define _USE_MATH_DEFINES
#include <cmath>

namespace vec3 {
  template<typename T>
  void cross(T *out, const T *a, const T *b) {
    T tmp[3]={a[1]*b[2]-a[2]*b[1],a[2]*b[0]-a[0]*b[2],a[0]*b[1]-a[1]*b[0]};

    for(int i=0;i<3;i++) {
      out[i]=tmp[i];
    }
  }

  template<typename T>
  T dot(const T *a, const T *b) {
    return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
  }

  template<typename T>
  T length(const T *v) {
    return sqrt(dot(v,v));
  }

  template<typename T>
  void normal(T *out, const T *v) {
    T l=length(v);
    for(int i=0;i<3;i++) {
      out[i]=v[i]/l;
    }
  }

  template<typename T>
  void add(T *out, const T *a, const T *b) {
    for(int i=0;i<3;i++) {
      out[i]=a[i]+b[i];
    }
  }

  template<typename T>
  void sub(T *out, const T *a, const T *b) {
    for(int i=0;i<3;i++) {
      out[i]=a[i]-b[i];
    }
  }

  template<typename T>
  void multiplyScalar(T *out, const T *v, const T s) {
    for(int i=0;i<3;i++) {
      out[i]=v[i]*s;
    }
  }
}

#endif
