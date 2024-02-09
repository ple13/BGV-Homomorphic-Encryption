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

#ifndef __UTILS_H__
#define __UTILS_H__

#include <cstdint>
#include <ostream>

std::ostream& operator<<(std::ostream& dest, __int128_t value);

// p1 = 2^62 + 106497
__int128_t mod_slow_p1(__int128_t x);

// p2 = 2^62 + 622593
__int128_t mod_slow_p2(__int128_t x);

__int128_t mod_slow_p0(__int128_t x);

#endif  // __UTILS_H__
