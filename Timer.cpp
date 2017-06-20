#include "Timer.h"


Timer::Timer() {
}

void Timer::start() {
  timeStart = std::chrono::system_clock::now();
  //timeStart = boost::chrono::system_clock::now();
}

double Timer::stop() {
  auto timeStop = std::chrono::system_clock::now();
  auto dif=timeStop-timeStart;
  return std::chrono::duration_cast<std::chrono::duration<double,std::ratio<1>>>(dif).count();

  //auto timeStop = boost::chrono::system_clock::now();
  //auto dif=timeStop;//-timeStart;
  //boost::chrono::duration_cast<boost::chrono::milliseconds>(timeStop).count();
  //return boost::chrono::duration_cast<boost::chrono::duration<double,boost::ratio<1>>>(dif).count();
 // return 0.0;
}
