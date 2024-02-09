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

#include "utils.h"

#include <iostream>

__int128_t mask_lo = (1ULL << 62) - 1;
__int128_t p1 = (1ULL << 62) + 106497;
__int128_t p2 = (1ULL << 62) + 622593;
__int128_t Q = p1 * p2;
__int128_t offset_p1 = p1 << 22;
__int128_t offset_p2 = p2 << 22;

__int128_t lslow = (1ULL << 60) - 1;
__int128_t pslow = (1ULL << 60) + 7 * (1ULL << 13) + 1ULL;
__int128_t offset = ((__int128_t)pslow) << 20;

std::ostream& operator<<(std::ostream& dest, __int128_t value) {
  std::ostream::sentry s(dest);
  if (s) {
    __int128_t tmp = value < 0 ? -value : value;
    char buffer[128];
    char* d = std::end(buffer);
    do {
      --d;
      *d = "0123456789"[tmp % 10];
      tmp /= 10;
    } while (tmp != 0);
    if (value < 0) {
      --d;
      *d = '-';
    }
    int len = std::end(buffer) - d;
    if (dest.rdbuf()->sputn(d, len) != len) {
      dest.setstate(std::ios_base::badbit);
    }
  }
  return dest;
}

// p1 = 2^62 + 106497
__int128_t mod_slow_p1(__int128_t x) {
  __int128_t y = ((__int128)4611686018427494401ULL << 20) +
                 (x & 0x3FFFFFFFFFFFFFFF) - 106497ULL * (x >> 62);
  __int128_t z = (y & 0x3FFFFFFFFFFFFFFF) - 106497ULL * (y >> 62);
  while (z < 0) {
    z += p1;
  }
  return z;
}

// p2 = 2^62 + 622593
__int128_t mod_slow_p2(__int128_t x) {
  __int128_t y = ((__int128)4611686018428010497ULL << 20) +
                 (x & 0x3FFFFFFFFFFFFFFF) - 622593ULL * (x >> 62);
  __int128_t z = (y & 0x3FFFFFFFFFFFFFFF) - 622593ULL * (y >> 62);
  while (z < 0) {
    z += p2;
  }
  return z;
}

__int128_t mod_slow_p0(__int128_t x) {
  __int128_t y = offset + (x & lslow) - 57345ULL * (x >> 60);
  __int128_t z = (y & lslow) - 57345ULL * (y >> 60);
  while (z < 0) {
    z += pslow;
  }
  return z;
}
