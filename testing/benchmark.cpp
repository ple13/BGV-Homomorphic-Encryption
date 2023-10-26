// g++ -O3 -msse -msse2 -mavx TestNTLvsInt.cpp -lssl -lntl -lgmp -lpthread -std=c++11 -lcrypto ../fastMod.a


#include "Timer.h"
#include "immintrin.h"
#include <cassert>
#include <cstdint>
#include <array>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <random>
#include <NTL/ZZ.h>

using namespace NTL;
using namespace std;

extern "C" void asmMulModQ(uint64_t* Z, uint64_t * X, uint64_t * Y);
extern "C" void asmMulModP(uint64_t* Z, uint64_t * X, uint64_t * Y);
extern "C" void asmSubModQ(uint64_t* Z, uint64_t * X, uint64_t * Y);
extern "C" void asmMulModP(uint64_t* Z, uint64_t * X, uint64_t * Y);
extern "C" void asmAddModP(uint64_t* Z, uint64_t * X, uint64_t * Y);
extern "C" void asmSubModP(uint64_t* Z, uint64_t * X, uint64_t * Y);
extern "C" void asmMulWithPModQ(uint64_t* Z, uint64_t * X);

void testModMult192bits();
void testModMult64bits();

std::ostream&
operator<<( std::ostream& dest, __int128_t value ) {
  std::ostream::sentry s( dest );
  if ( s ) {
    __int128_t tmp = value < 0 ? -value : value;
    char buffer[ 128 ];
    char* d = std::end( buffer );
    do
    {
      -- d;
      *d = "0123456789"[ tmp % 10 ];
      tmp /= 10;
    } while ( tmp != 0 );
    if ( value < 0 ) {
      -- d;
      *d = '-';
    }
    int len = std::end( buffer ) - d;
    if ( dest.rdbuf()->sputn( d, len ) != len ) {
      dest.setstate( std::ios_base::badbit );
    }
  }
  return dest;
}

int main(int argc, char** argv) {
  cout << "Testing 64 bit fast mod" << endl;
  testModMult64bits();

  cout << "Testing 192 bit fast mod" << endl;
  testModMult192bits();

  return 0;
}

void testModMult64bits() {
  int n = 10485760;
  /*--------------------------25919----------------------------*/
  vector<uint64_t> vecA(n, 0), vecB(n, 0), vecC(n, 0);
  vector<ZZ> ZA(n), ZB(n), ZC(n);
  vector<unsigned __int128_t> vA(n), vB(n), vC(n);

  uint64_t mod64bit_ = -114687;
  ZZ mod64bit = (ZZ(1) << 64) - ZZ(114687);
  ZA[0] = (ZZ(1) << 62);
  ZB[0] = (ZZ(1) << 62);
  vecA[0] = 1;
  vecB[0] = 1;
  vecA[0] = (vecA[0] << 62);
  vecB[0] = (vecB[0] << 62);
  vA[0] = vecA[0];
  vB[0] = vecB[0];
  for (int idx = 1; idx < n; idx++) {
    ZA[idx] = ZA[idx-1] + ZZ(1);
    ZB[idx] = ZB[idx-1] + ZZ(1);
    vecA[idx] = vecA[idx - 1] + 1;
    vecB[idx] = vecB[idx - 1] + 1;
    vA[idx] = vecA[idx];
    vB[idx] = vecB[idx];
  }


  Timer t;
  for (int idx = 0; idx < n; idx++) {
    asmMulModP(&vecC[idx], &vecA[idx], &vecB[idx]);
  }

  double fast = t.Tick("Fast mult 64 bits");

  for (int idx = 0; idx < n; idx++) {
    MulMod(ZC[idx], ZA[idx], ZB[idx], mod64bit);
  }
  double slow = t.Tick("ZZ mult 64 bits");

  cout << "asm/NTL: " << slow/fast << endl;

  for (int idx = 0; idx < n; idx++) {
    vC[idx] = vA[idx]*vB[idx] % mod64bit_;
  }
  slow = t.Tick("c++ 64 bits");
  cout << "asm/c++: " << slow/fast << endl;
}

void testModMult192bits() {
  int n = 10485760;        
  /*--------------------------25919----------------------------*/
  vector<uint64_t> vecA(3*n, 0), vecB(3*n, 0), vecC(3*n, 0);
  vector<ZZ> ZA(n), ZB(n), ZC(n);
  ZZ mod192bit = (ZZ(1) << 192) - ZZ(933887);
  cout << mod192bit << endl;
  Timer t;
  for (int idx = 0; idx < n; idx++) {
    ZA[idx] = ZZ(idx+1)*(ZZ(1) + (ZZ(1) << 64) + (ZZ(1)<<128));
    ZB[idx] = ZZ(1)*(ZZ(1) + (ZZ(1) << 64) + (ZZ(1)<<128));
    vecA[3*idx] = idx+1;
    vecA[3*idx+1] = idx+1;
    vecA[3*idx+2] = idx+1;
    vecB[3*idx] = 1;
    vecB[3*idx+1] = 1;
    vecB[3*idx+2] = 1;
  }

  t.Tick("convert");
  for (int idx = 0; idx < n; idx++) {
    MulMod(ZC[idx], ZA[idx], ZB[idx], mod192bit);
  }
  double slow = t.Tick("ZZ mult 192 bits");

  t.Tick("Print");
  for (int idx = 0; idx < n; idx++) {
//     cout << idx << endl;
    uint64_t *X = vecA.data() + 3*idx;
    uint64_t *Y = vecB.data() + 3*idx;
    uint64_t *Z = vecC.data() + 3*idx;
    asmMulModQ(Z, X, Y);
  }
  double fast = t.Tick("Fast mult 192 bits");
  cout << "Gain: " << slow/fast << endl;

  for (int i = 0; i < n; i++) {
    asmMulWithPModQ(vecC.data() + 3*i, vecA.data() + 3*i);
  }
  fast = t.Tick("Fast mult 192 bits with 64 bits");
  cout << "Gain: " << slow/fast << endl;
}

