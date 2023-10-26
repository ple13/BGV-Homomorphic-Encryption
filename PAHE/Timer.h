#ifndef _TIMER_H__
#define _TIMER_H__

#include <sstream>
#include <iostream>
#include <sys/time.h>
#include <chrono>
#include <ctime>

class Timer 
{
public:
  struct timeval diff, startTV, endTV;
  
  Timer() {
    gettimeofday(&startTV, NULL); 
  }
  
  double Tick(std::string str) {
    gettimeofday(&endTV, NULL); 
    timersub(&endTV, &startTV, &diff);
    
    if (!str.empty()) printf("**%s time: %f seconds\n", str.c_str(), diff.tv_sec + (float)diff.tv_usec/1000000.0);
    
    startTV = endTV;
    
    return diff.tv_sec + (float)diff.tv_usec/1000000.0;
  }
};

#endif
