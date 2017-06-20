#include "MouseSmooth.h"


MouseSmooth::MouseSmooth()
  :   m_historyBufferSize(10)
{
  m_history=new double[m_historyBufferSize * 2];

  m_xPosRelative = 0;
  m_yPosRelative = 0;

  m_weightModifier = 0.2f;


  m_filtered[0] = 0.0f;
  m_filtered[1] = 0.0f;

  m_mouseIndex = 0;
  m_mouseMovementX[0] = m_mouseMovementX[1] = 0.0f;
  m_mouseMovementY[0] = m_mouseMovementY[1] = 0.0f;

  for(int i=0;i<m_historyBufferSize * 2;i++) {
    m_history[i]=0.0f;
  }

  //
  lastCursorX=0.0,lastCursorY=0.0;
  lastCursorSet=false;
}


MouseSmooth::~MouseSmooth() {
  delete [] m_history;
}


void MouseSmooth::beforeInput() {
  m_xPosRelative = 0.0f;
  m_yPosRelative = 0.0f;
}

void MouseSmooth::onMouseMove(double x, double y) {
  m_xPosRelative = x;
  m_yPosRelative = y;

  performMouseFiltering(m_xPosRelative, m_yPosRelative);

  m_xPosRelative = m_filtered[0];
  m_yPosRelative = m_filtered[1];

  performMouseSmoothing(m_xPosRelative, m_yPosRelative);

  m_xPosRelative = m_filtered[0];
  m_yPosRelative = m_filtered[1];
}

void MouseSmooth::onCursor(double x, double y) {
  if(!lastCursorSet) {
    lastCursorX=x;
    lastCursorY=y;
    lastCursorSet=true;
  } else {
    onMouseMove(x-lastCursorX,y-lastCursorY);
    lastCursorX=x;
    lastCursorY=y;
  }
}

void MouseSmooth::performMouseFiltering(double x, double y) {
  // Filter the relative mouse movement based on a weighted sum of the mouse
  // movement from previous frames to ensure that the mouse movement this
  // frame is smooth.
  //
  // For further details see:
  //  Nettle, Paul "Smooth Mouse2 Filtering", flipCode's Ask Midnight column.
  //  http://www.flipcode.com/cgi-bin/fcarticles.cgi?show=64462

  // Newer mouse entries towards front and older mouse entries towards end.
  for (int i = m_historyBufferSize - 1; i > 0; --i)
    {
      m_history[i * 2] = m_history[(i - 1) * 2];
      m_history[i * 2 + 1] = m_history[(i - 1) * 2 + 1];
    }

  // Store current mouse entry at front of array.
  m_history[0] = x;
  m_history[1] = y;

  double averageX = 0.0f;
  double averageY = 0.0f;
  double averageTotal = 0.0f;
  double currentWeight = 1.0f;

  // Filter the mouse.
  for (int i = 0; i < m_historyBufferSize; ++i)
    {
      averageX += m_history[i * 2] * currentWeight;
      averageY += m_history[i * 2 + 1] * currentWeight;
      averageTotal += 1.0f * currentWeight;
      currentWeight *= m_weightModifier;
    }

  m_filtered[0] = averageX / averageTotal;
  m_filtered[1] = averageY / averageTotal;
}
void MouseSmooth::performMouseSmoothing(double x, double y) {
  // Smooth out the mouse movement by averaging the distance the mouse
  // has moved over a couple of frames.

  m_mouseMovementX[m_mouseIndex] = x;
  m_mouseMovementY[m_mouseIndex] = y;

  m_filtered[0] = (m_mouseMovementX[0] + m_mouseMovementX[1]) * 0.5f;
  m_filtered[1] = (m_mouseMovementY[0] + m_mouseMovementY[1]) * 0.5f;

  m_mouseIndex ^= 1;
  m_mouseMovementX[m_mouseIndex] = 0.0f;
  m_mouseMovementY[m_mouseIndex] = 0.0f;
}
double MouseSmooth::getX() {
  return m_xPosRelative;
}
double MouseSmooth::getY() {
  return m_yPosRelative;
}
double MouseSmooth::weightModifier() {
  return m_weightModifier;
}

void MouseSmooth::setWeightModifier(double weightModifier) {
  m_weightModifier=weightModifier;
}
