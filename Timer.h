#ifndef TIMER_H
#define TIMER_H

#include <chrono>
//#include <boost/chrono.hpp>
//#include <boost/chrono/duration.hpp>
//#include <boost/ratio.hpp>
//#include <ctime>
class Timer {
private:

 // time_t timeStart;

  std::chrono::time_point<std::chrono::system_clock,std::chrono::system_clock::duration> timeStart;
  //boost::chrono::time_point<boost::chrono::system_clock,boost::chrono::system_clock::duration> timeStart;
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