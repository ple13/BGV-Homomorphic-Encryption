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

#include "HashInput.h"

#include <NTL/ZZ.h>

using namespace std;

map<int, vector<uint64_t>> hashInput(const vector<uint64_t>& input,
                                     int nBuckets, const string seed) {
  hash<string> ptr_hash;
  map<int, vector<uint64_t>> ret;
  for (auto x : input) {
    auto str = seed + to_string(x);
    size_t index = ptr_hash(str);
    index %= nBuckets;
    ret[index].push_back(x);
  }

  return ret;
}
