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

#ifndef _SRC_MAIN_CC_MATHS_HASH_INPUT_
#define _SRC_MAIN_CC_MATHS_HASH_INPUT_

#include <NTL/ZZ.h>

#include <cassert>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

map<int, vector<uint64_t>> hashInput(const vector<uint64_t>& input,
                                     int nBuckets, const string seed);

#endif  // _SRC_MAIN_CC_MATHS_HASH_INPUT_
