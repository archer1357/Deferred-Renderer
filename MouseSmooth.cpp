#include "MouseSmooth.h"

void pushMouseHistory(double x,double y,double *history,int historySize) {
  memmove(&history[2],history,sizeof(double)*(historySize*2-2));
  history[0]=x;
  history[1]=y;
}

void calcMouseSmooth(double weightModifier,int historySize,
  const double *history,
  double *mouseSmoothX,double *mouseSmoothY) {

  int i;
  double averageX = 0.0;
  double averageY = 0.0;
  double averageTotal = 0.0;
  double currentWeight = 1.0;
  double filteredX;
  double filteredY;

  for(i = 0; i < historySize; ++i) {
    averageX += history[i*2] * currentWeight;
    averageY += history[i*2+1] * currentWeight;
    averageTotal += 1.0 * currentWeight;
    currentWeight *= weightModifier;
  }

  filteredX=averageX/averageTotal;
  filteredY=averageY/averageTotal;

  //original algorithm (from www.dhpoware.com GLCamera3 demo) stored the
  //previous filteredX/filteredY and used that instead of the previous
  //mouseSmoothX/mouseSmoothY, this is used because it is less hassle
  *mouseSmoothX = (filteredX + (*mouseSmoothX)) * 0.5;
  *mouseSmoothY = (filteredY + (*mouseSmoothY)) * 0.5;
}

void updateMouseSmooth(double x,double y,double *history,int historySize,
  double weightModifier,double *smoothX,double *smoothY) {
  pushMouseHistory(x,y,history,historySize);
  calcMouseSmooth(weightModifier,historySize,history,smoothX,smoothY);
}

