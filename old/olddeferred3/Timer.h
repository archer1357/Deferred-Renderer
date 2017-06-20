#ifndef TIMER_H
#define TIMER_H

#include <chrono>

class Timer {
private:
  std::chrono::time_point<std::chrono::system_clock,std::chrono::system_clock::duration> timeStart;
public:
  Timer();
  void start();
  double stop();
};

#endif


/*

double timer() {
auto start = std::chrono::system_clock::now();

auto end= std::chrono::system_clock::now();
auto dif=end-start;
return std::chrono::duration_cast<std::chrono::duration<double,std::ratio<1>>>(dif).count();
}

*/