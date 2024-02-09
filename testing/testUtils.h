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

#include <NTL/ZZ.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <numeric>
#include <random>

#include "../utils/Timer.h"
#include "immintrin.h"
#include "testUtils.h"

void ZZToU3(ZZ val, uint64_t* u);
ZZ U3ToZZ(vector<uint64_t> u);
void printZZ(ZZ val);
void printU3(vector<uint64_t> val);
std::ostream& operator<<(std::ostream& dest, __int128_t value);
