#ifndef MOUSESMOOTH_H
#define MOUSESMOOTH_H

#include <cstring>

void pushMouseHistory(double x,double y,double *history,int historySize);

void calcMouseSmooth(double weightModifier,int historySize,
  const double *history,
  double *mouseSmoothX,double *mouseSmoothY);

void updateMouseSmooth(double x,double y,double *history,int historySize,
  double weightModifier,double *smoothX,double *smoothY);
#endif