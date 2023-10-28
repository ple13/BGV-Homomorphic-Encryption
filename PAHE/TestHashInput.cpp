// g++ -O3 -ftree-vectorize -march=native -msse4.1 -mavx TestHashInput.cpp HashInput.cpp -lssl -lntl -lgmp -lpthread -std=c++11 -lcrypto

#include "HashInput.h"

int main(int argc, char** argv) {
  int B = 1.2*1048576;
  vector<uint64_t> input;
  for (int idx = 0; idx < 3*1048576; idx++) {
    input.push_back(idx);
  }
  
  for (int idx = 0; idx < 100; idx++) {
    srand (time(NULL));
    int seed = rand();
    
    auto hashedInput = hashInput(input, B, to_string(seed));
  }
  return 0;
}