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

#include <iostream>
#include <numeric>
#include <random>
#include <vector>

#include "HashInput.h"

int main() {
  std::vector<uint64_t> input(1 << 20);
  srand(time(NULL));
  for (int k = 1; k < 100; k++) {
    for (int i = 0; i < input.size(); i++) {
      input[i] = rand();
    }
    hashInput(input, (1 << 16), "adfalsd");
  }
  return 0;
}
