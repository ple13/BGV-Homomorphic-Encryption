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

#include "../PAHE/RandomGenerator.h"
#include "../utils/Timer.h"

using namespace std;

int main() {
  Timer t;
  NoiseGenerator noise_gen(4.0);
  vector<vector<uint64_t>> flooding_noise =
      noise_gen.GenerateFloodingNoiseVector(4 * 59 * 4096);
  t.Tick("Sample flooding noise");

  for (int i = 0; i < 10; i++) {
    cout << hex << flooding_noise[i][0] << " " << flooding_noise[i][1] << " "
         << flooding_noise[i][2] << endl;
  }
  return 0;
}
