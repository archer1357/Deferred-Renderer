#ifndef MOUSESMOOTH_H
#define MOUSESMOOTH_H

class MouseSmooth {
private:
  int m_historyBufferSize;
  int m_mouseIndex;
  double m_xPosRelative;
  double m_yPosRelative;
  double m_weightModifier;
  double m_filtered[2];
  double *m_history;
  double m_mouseMovementX[2];
  double m_mouseMovementY[2];
public:
  MouseSmooth(int historyBufferSize=10);
  ~MouseSmooth();
  void beforeInput();
  void onMouseMove(double x,double y);
  double getX();
  double getY();
  double weightModifier();
  void setWeightModifier(double weightModifier);
};

#endif