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

// g++ -std=c++11 FindMaxBucketSize.cpp
#include <math.h>
#include <quadmath.h>
#include <stdint.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>

// Function to compute the upper bound probability that any bucket has at most t
// items. n: num items. m: num buckets. t: prob a bucket has t balls.
__float128 prob(int64_t n, int64_t m, int64_t t);

// Find max bucket size with prob (1 - 2^sec)
int findMaxBucketSize(int n, int nBuckets, int sec);

std::vector<int> findCutAndChooseParams(int inputSize);

int main() {
  int n = 1048576;
  int nBuckets = 4096;
  int sec = 40;
  for (int n = (1 << 14); n <= (1 << 20); n = n * 4) {
    for (int i = 1; i <= n / nBuckets; i++) {
      int size = findMaxBucketSize(n, nBuckets * i, sec);
      std::cout << n << "\t" << nBuckets * i << "\t" << size << "\t" << size * i
                << "\t" << size * i * size << std::endl;
    }
  }

  return 0;
}

std::vector<int> findCutAndChooseParams(int inputSize) {
  if (inputSize == 1048576) {
    return {700, 162};
  } else if (inputSize == 65536) {
    return {82, 132};
  } else if (inputSize == 1 << 24) {
    return {8600, 745};
  }
  return {0, 0};
}

// Function to compute the upper bound probability that any bucket has at most t
// items. n: num items. m: num buckets. t: prob a bucket has t balls.
__float128 prob(int64_t n, int64_t m, int64_t t) {
  // m = n/c
  // Pr[X = t] = (n chooses t)*1/m^t*((m-1)/m)^(n-t) = m * n(n-1)...(n-t+1)/t! *
  // 1/(m-1)^t * e^(-c) Pr[X = t] = e^(-c)* Prod_{i=0...(t-1)}
  // (n-i)/((i+1)*(m-1))

  double c = (double)n / (double)m;

  __float128 d = exp(-c);

  __float128 prod = d;  // prod = e^(-c)

  for (int64_t idx = 0; idx < t; idx++) {
    d *= ((__float128)(n - idx)) /
         (((__float128)(idx + 1)) * ((__float128)(m - 1)));
  }

  return d;
}

// Find max bucket size with prob (1 - 2^sec)
int findMaxBucketSize(int n, int nBuckets, int sec) {
  int ret = (double)n / (double)nBuckets;
  __float128 secureParam = pow(2.0, -sec);
  __float128 probability = (__float128)nBuckets * prob(n, nBuckets, ret);

  for (int idx = ret; idx <= 2048; idx++) {
    if (probability < secureParam) {
      return idx;
    }

    __float128 scale =
        (__float128)(n - idx - 1) / (__float128)((idx + 2) * (nBuckets - 1));
    probability *= scale;
  }

  return 0;
}
