#include "Timer.h"


Timer::Timer() {
}

void Timer::start() {
  timeStart = std::chrono::system_clock::now();
}

double Timer::stop() {
  auto timeStop = std::chrono::system_clock::now();
  auto dif=timeStop-timeStart;
  return std::chrono::duration_cast<std::chrono::duration<double,std::ratio<1>>>(dif).count();
}