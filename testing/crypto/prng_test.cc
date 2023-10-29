#include <cassert>
#include <iostream>
#include <vector>

#include "../../crypto/prng.h"

void test_deterministic_rand();

int main() {
  test_deterministic_rand();
  return 0;
}

void test_deterministic_rand() {
  PRNG *prng_1 = new PRNG((const unsigned char *)"seed", (const unsigned char *)"IV");
  PRNG *prng_2 = new PRNG((const unsigned char *)"seed", (const unsigned char *)"IV");

  std::vector<unsigned char> dst_1;
  std::vector<unsigned char> dst_2;

  int size = 4096*3*sizeof(uint64_t);

  prng_1->sampleBytes(dst_1, size);
  prng_2->sampleBytes(dst_2, size);

  delete prng_1;
  delete prng_2;

  assert(dst_1 == dst_2);

  std::cout << "test_deterministic_rand.....    PASSED\n";
}
