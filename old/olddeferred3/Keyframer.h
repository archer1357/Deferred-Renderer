#ifndef KEYFRAMER_H
#define KEYFRAMER_H

#include <list>
#include <cmath>
#include <iostream>

template<typename T, int n>
class Keyframer {
private:
  struct Keyframe {
    T pos;
    T data[n];
  };

  std::list<Keyframe> keyframes;
private:
  T saturate(T x) {
    return std::max((T)0.0,std::min((T)1.0,x));
  }

  T smoothStep(T a,T b,T x) {
    T t = saturate((x-a)/(b-a));
    return t*t*((T)3.0-(T)2.0*t);
  }

  T lerp(T f0,T f1,T u) {
    return ((T)1.0-u)*f0+u*f1;
  }
public:
  Keyframer() {

  }
  void add(T pos,const T *data) {
    Keyframe keyframe;
    keyframe.pos=pos;

    for(int i=0;i<n;i++) {
      keyframe.data[i]=data[i];
    }

    keyframes.push_back(keyframe);
  }
  void calc(T frame,bool smooth,T *out) {
    //
    if(keyframes.empty()) {
      for(int i=0;i<n;i++) {
        out[i]=(T)0.0;
      }

      return;
    }

    //
    T frame2=std::fmod(frame,keyframes.back().pos);

    //
    if(keyframes.size()==1 || frame2 <= keyframes.front().pos) {
      for(int i=0;i<n;i++) {
        out[i]=keyframes.front().data[i];
      }

      return;
    }

    //
    auto it=keyframes.begin();
    auto it2=keyframes.begin();
    it2++;

    while(true) {
      T pos1=it->pos;
      T pos2=it2->pos;

      if(frame2>=pos1 && frame2<=pos2) {
        T i=(frame2-pos1)/(pos2-pos1);

        for(int j=0;j<n;j++) {
          T val1=it->data[j];
          T val2=it2->data[j];
          //std::cout << "j " << j << " / " << n << std::endl;
          if(smooth) {
            out[j]=lerp(val1,val2,smoothStep((T)0,(T)1,i));
          } else {
            out[j]=lerp(val1,val2,i);
          }
        }

        return;
      }

      it++;
      it2++;
    }
  }
};
#endif
