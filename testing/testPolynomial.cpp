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

#include <iostream>
#include <numeric>
#include <vector>

#include "../maths/utils.h"

using namespace std;

int main() {
  vector<uint64_t> roots(4);
  iota(roots.begin(), roots.end(), 0);

  auto p = interpolate(roots);

  for (int i = 0; i < p.size(); i++) {
    cout << p[i] << "x^" << i << " + ";
  }
  cout << endl;
  return 0;
}
