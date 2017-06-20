#ifndef MOUSESMOOTH_H
#define MOUSESMOOTH_H

class MouseSmooth {
public:
  MouseSmooth();
  ~MouseSmooth();
  void beforeInput();
  void onMouseMove(double x, double y);
  void onCursor(double x, double y);
private:
  void performMouseFiltering(double x, double y);
  void performMouseSmoothing(double x, double y);
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
  double getX();
  double getY();
  double weightModifier();
  double wheelPos();
  void setWeightModifier(double weightModifier);
private:
  double lastCursorX,lastCursorY;
  bool lastCursorSet;
};

#endif
