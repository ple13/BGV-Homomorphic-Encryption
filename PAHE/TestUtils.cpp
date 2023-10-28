// g++ -O3 -msse -msse2 -mavx -ftree-vectorizer-verbose=2 -lssl -lntl -lgmp -lpthread -std=c++11 -lcrypto -m64 -floop-parallelize-all -ftree-parallelize-loops=4 -fopenmp TestUtils.cpp utils.cpp

#include "Timer.h"
// #include "utils.h"
#include <vector>

// p1 = 2^62 + 106497
inline __int128_t mod_slow_p1(__int128_t x) {
  __int128_t y = ((__int128)4611686018427494401ULL << 20) + (x & 0x3FFFFFFFFFFFFFFFULL) - 106497ULL*(x >> 62) ;
  __int128_t z = (y & 0x3FFFFFFFFFFFFFFFULL) - 106497ULL*(y >> 62);
  while (z < 0) {
    z += 4611686018427494401ULL;
  }
  return z;
}

// p2 = 2^62 + 622593
inline __int128_t mod_slow_p2(__int128_t x) {
  __int128_t y = ((__int128)4611686018428010497ULL << 20) + (x & 0x3FFFFFFFFFFFFFFFULL) - 622593ULL*(x >> 62) ;
  __int128_t z = (y & 0x3FFFFFFFFFFFFFFFULL) - 622593ULL*(y >> 62);
  while (z < 0) {
    z += 4611686018428010497ULL;
  }
  return z;
}

inline __int128_t mod_slow_p0(__int128_t x) {
  __int128_t y = (1152921504606904321ULL << 20) + (x & 0xFFFFFFFFFFFFFFFULL) - 57345ULL*(x >> 60) ;
  __int128_t z = (y & 0xFFFFFFFFFFFFFFFULL) - 57345ULL*(y >> 60);
  while (z < 0) {
    z += 1152921504606904321ULL;
  }
  return z;
}

inline __int128_t modp(__int128_t x, __int128_t modulus) {
  if (modulus == 4611686018427494401ULL) return mod_slow_p1(x);
  else return mod_slow_p2(x);
}

int main(int argc, char** argv) {
  

__int128_t p1 = (1ULL << 62) + 106497;
__int128_t p2 = (1ULL << 62) + 622593;
__int128_t Q = p1*p2;
  int n = 1048576;
  uint64_t modulus64 = 17026560423622361089ULL;
  __int128_t modulus127 = modulus64;
  
  std::vector<__int128_t> a0(n), b0(n), a1(n), b1(n), c0(n), c1(n);
  
  srand (time(NULL));
  int seed = rand();
    
  for (int idx = 0; idx < n; idx++) {
    a1[idx] = rand() % modulus64;
    b1[idx] = rand() % modulus64;
    a0[idx] = a1[idx]*a1[idx]*a1[idx]*a1[idx] % p1;
    b0[idx] = b1[idx]*b1[idx]*b1[idx]*b1[idx] % p1;
    a1[idx] = a1[idx]*a1[idx]*a1[idx]*a1[idx] % p2;
    b1[idx] = b1[idx]*b1[idx]*b1[idx]*b1[idx] % p2;
  }
  
  Timer t;
  
  for (int idx = 0; idx < n; idx++) {
    c1[idx] = a1[idx]*b1[idx] % modulus64;
  }
  
  t.Tick("Int mult 64");
  
  for (int idx = 0; idx < n; idx++) {
    c0[idx] = a0[idx]*b0[idx] % 4611686018427494401ULL;
  }
  
  for (int idx = 0; idx < n; idx++) {
    c1[idx] = a1[idx]*b1[idx] % 4611686018428010497ULL;
  }
  
  double t1 = t.Tick("Int mult 128");
    
  for (int idx = 0; idx < n; idx++) {
    c0[idx] = mod_slow_p1(a0[idx]*b0[idx]);
  }
  
  for (int idx = 0; idx < n; idx++) {
    c1[idx] = mod_slow_p2(a1[idx]*b1[idx]);
  }
  
  double t2 = t.Tick("Mul CRT");
  
  for (int idx = 0; idx < n; idx++) {
    c0[idx] = modp(a0[idx]*b0[idx], 4611686018427494401ULL);
  }
  
  for (int idx = 0; idx < n; idx++) {
    c1[idx] = modp(a1[idx]*b1[idx], 4611686018428010497ULL);
  }
  
  double t3 = t.Tick("Mul CRT modp");
  
  for (int idx = 0; idx < n; idx++) {
    c0[idx] = mod_slow_p0(a0[idx]*b0[idx]);
  }
  
  for (int idx = 0; idx < n; idx++) {
    c1[idx] = mod_slow_p0(a1[idx]*b1[idx]);
  }
  
  t.Tick("Mul CRT P0");
  std::cout << t1 << " " << t2 << " " << t3 << std::endl;
  std::cout << "speed up by: " << t1/t2 << "X and " << t1/t3 << "X" << std::endl;
  std::cout << "slow down by: " << t3/t2 << "X" << std::endl;
  return 0;
}