#include "MouseSmooth.h"

//From http://www.dhpoware.com/demos/index.html

MouseSmooth::MouseSmooth(int historyBufferSize)
: m_historyBufferSize(historyBufferSize)
{
  m_history=new double[m_historyBufferSize * 2];

  m_xPosRelative = 0.0;
  m_yPosRelative = 0.0;

  m_weightModifier = 0.2f;


  m_filtered[0] = 0.0;
  m_filtered[1] = 0.0;

  m_mouseIndex = 0;
  m_mouseMovementX[0] = m_mouseMovementX[1] = 0.0;
  m_mouseMovementY[0] = m_mouseMovementY[1] = 0.0;

  for(int i=0;i<m_historyBufferSize * 2;i++) {
    m_history[i]=0.0;
  }

}


MouseSmooth::~MouseSmooth() {
  delete[] m_history;
}


void MouseSmooth::beforeInput() {
  m_xPosRelative = 0.0;
  m_yPosRelative = 0.0;
}


void MouseSmooth::onMouseMove(double x,double y) {
  //
  m_xPosRelative = x;
  m_yPosRelative = y;

  //
  {
    // Filter the relative mouse movement based on a weighted sum of the mouse
    // movement from previous frames to ensure that the mouse movement this
    // frame is smooth.

    // Newer mouse entries towards front and older mouse entries towards end.
    for(int i = m_historyBufferSize - 1; i > 0; --i)
    {
      m_history[i * 2] = m_history[(i - 1) * 2];
      m_history[i * 2 + 1] = m_history[(i - 1) * 2 + 1];
    }

    // Store current mouse entry at front of array.
    m_history[0] = m_xPosRelative;
    m_history[1] = m_yPosRelative;

    double averageX = 0.0;
    double averageY = 0.0;
    double averageTotal = 0.0;
    double currentWeight = 1.0;

    // Filter the mouse.
    for(int i = 0; i < m_historyBufferSize; ++i)
    {
      averageX += m_history[i * 2] * currentWeight;
      averageY += m_history[i * 2 + 1] * currentWeight;
      averageTotal += 1.0 * currentWeight;
      currentWeight *= m_weightModifier;
    }

    m_filtered[0] = averageX / averageTotal;
    m_filtered[1] = averageY / averageTotal;
  }

  //
  m_xPosRelative = m_filtered[0];
  m_yPosRelative = m_filtered[1];

  //
  {
    // Smooth out the mouse movement by averaging the distance the mouse
    // has moved over a couple of frames.

    m_mouseMovementX[m_mouseIndex] = m_xPosRelative;
    m_mouseMovementY[m_mouseIndex] = m_yPosRelative;

    m_filtered[0] = (m_mouseMovementX[0] + m_mouseMovementX[1]) * 0.5f;
    m_filtered[1] = (m_mouseMovementY[0] + m_mouseMovementY[1]) * 0.5f;

    m_mouseIndex ^= 1;
    m_mouseMovementX[m_mouseIndex] = 0.0;
    m_mouseMovementY[m_mouseIndex] = 0.0;
  }

  m_xPosRelative = m_filtered[0];
  m_yPosRelative = m_filtered[1];
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
  this->m_weightModifier=weightModifier;
}

