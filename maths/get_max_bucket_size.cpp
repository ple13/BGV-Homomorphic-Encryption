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
