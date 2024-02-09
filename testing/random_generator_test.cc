#include <iostream>
#include "../PAHE/RandomGenerator.h"
#include "../utils/Timer.h"

using namespace std;

int main() {
  Timer t;
  NoiseGenerator noise_gen(4.0);
  vector<vector<uint64_t>> flooding_noise = noise_gen.GenerateFloodingNoiseVector(4*59*4096);
  t.Tick("Sample flooding noise");

  for (int i = 0; i < 10; i++) {
    cout << hex << flooding_noise[i][0] << " " << flooding_noise[i][1] << " " << flooding_noise[i][2] << endl;
  }
  return 0;
}
