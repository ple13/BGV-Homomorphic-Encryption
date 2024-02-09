// Copyright 2024 Phi Hung Le
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _TIMER_H__
#define _TIMER_H__

#include <sys/time.h>

#include <chrono>
#include <ctime>
#include <iostream>
#include <sstream>

class Timer {
 public:
  struct timeval diff, startTV, endTV;

  Timer() { gettimeofday(&startTV, NULL); }

  double Tick(std::string str) {
    gettimeofday(&endTV, NULL);
    timersub(&endTV, &startTV, &diff);

    if (!str.empty())
      printf("**%s time: %f seconds\n", str.c_str(),
             diff.tv_sec + (float)diff.tv_usec / 1000000.0);

    startTV = endTV;

    return diff.tv_sec + (float)diff.tv_usec / 1000000.0;
  }
};

#endif
