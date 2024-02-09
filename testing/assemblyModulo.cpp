#include <cassert>
#include <cstdint>
#include <array>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <random>
#include <NTL/ZZ.h>

#include "immintrin.h"
#include "../utils/Timer.h"

using namespace NTL;
using namespace std;

extern "C" void asmMulModQ(uint64_t* Z, uint64_t * X, uint64_t * Y);
extern "C" void asmAddModQ(uint64_t* Z, uint64_t * X, uint64_t * Y);
extern "C" void asmSubModQ(uint64_t* Z, uint64_t * X, uint64_t * Y);
extern "C" void asmMulModP(uint64_t* Z, uint64_t * X, uint64_t * Y);
extern "C" void asmAddModP(uint64_t* Z, uint64_t * X, uint64_t * Y);
extern "C" void asmSubModP(uint64_t* Z, uint64_t * X, uint64_t * Y);
extern "C" void asmMulWithPModQ(uint64_t* Z, uint64_t * X);
extern "C" void asmModPInQ(uint64_t* Z, uint64_t * X);

void testModMult192bits();
void testModMult64bits();
void testModAdd192bits();
void testModAdd64bits();
void testModSub192bits();
void testModSub64bits();
void testModMultWithP();
void testModSub64bitsInQ();

void ZZToU3(ZZ val, uint64_t *u) {
  ZZ temp;
  ZZ one = ZZ(1);
  temp = val % (one << 64);
  BytesFromZZ((unsigned char *)&u[0], val, 8);
  val = val >> 64;temp = val % (one << 64);
  BytesFromZZ((unsigned char *)&u[1], val, 8);
  val = val >> 64;temp = val % (one << 64);
  BytesFromZZ((unsigned char *)&u[2], val, 8);
}

ZZ U3ToZZ(vector<uint64_t> u) {
  ZZ ret = ZZ(0);
  ret += ZZFromBytes((unsigned char *)&u[2], 8);
  ret = (ret << 64);
  ret += ZZFromBytes((unsigned char *)&u[1], 8);
  ret = (ret << 64);
  ret += ZZFromBytes((unsigned char *)&u[0], 8);
  return ret;
}

void printZZ(ZZ val) {
  ZZ one = ZZ(1);
  cout << val << endl;
  cout << "ZZ[0]: " << (val % (one << 64)) << endl;
  val = val >> 64;
  cout << "ZZ[1]: " << (val % (one << 64)) << endl;
  val = val >> 64;
  cout << "ZZ[2]: " << (val % (one << 64)) << endl;
}

void printU3(vector<uint64_t> val) {
  cout << "v[0]: " << val[0] << endl;
  cout << "v[1]: " << val[1] << endl;
  cout << "v[2]: " << val[2] << endl;
}

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
  testModMult192bits();
  testModMult64bits();
  testModMultWithP();
  testModAdd192bits();
  testModAdd64bits();
  testModSub192bits();
  testModSub64bits();
  testModSub64bitsInQ();
  return 0;
}

void testModMult192bits() {
  vector<uint64_t> X(3), Y(3), Z(3);
  ZZ zx, zy, zz;
  ZZ mod192bit = (ZZ(1) << 192) - ZZ(933887);

  zx = RandomBnd(mod192bit);
  zy = RandomBnd(mod192bit);

  ZZToU3(zx, X.data());
  ZZToU3(zy, Y.data());
  cout << "zx: " << zx << endl;
  cout << "zy: " << zy << endl;

  asmMulModQ(Z.data(), X.data(), Y.data());
  MulMod(zz, zx, zy, mod192bit);
  cout << "zx: " << U3ToZZ(X) << endl;
  cout << "zy: " << U3ToZZ(Y) << endl;

  assert(zz == U3ToZZ(Z));
  cout << "testModMult192bits.........    Done" << endl;
}

void testModMult64bits() {
  uint64_t X, Y, Z;
  ZZ zx, zy, zz;
  ZZ mod64bit = (ZZ(1) << 64) - ZZ(114687);

  zx = RandomBnd(mod64bit);
  zy = RandomBnd(mod64bit);

  BytesFromZZ((unsigned char *)&X, zx, 8);
  BytesFromZZ((unsigned char *)&Y, zy, 8);

  asmMulModP(&Z, &X, &Y);
  MulMod(zz, zx, zy, mod64bit);

  assert(zz == ZZFromBytes((unsigned char *)&Z, 8));
  cout << "testModMult64bits..........    Done" << endl;
}

void testModMultWithP() {
  vector<uint64_t> X(3), Z(3);
  ZZ zx, zy, zz;
  ZZ mod192bit = (ZZ(1) << 192) - ZZ(933887);

  zx = RandomBnd(mod192bit);
  zy = (ZZ(1) << 64) - ZZ(114687);
  ZZToU3(zx, X.data());

  asmMulWithPModQ(Z.data(), X.data());
  MulMod(zz, zx, zy, mod192bit);

  assert(zz == U3ToZZ(Z));
  cout << "testModMultWithP...........    Done" << endl;
}

void testModAdd192bits() {
  vector<uint64_t> X(3), Y(3), Z(3);
  ZZ zx, zy, zz;
  ZZ mod192bit = (ZZ(1) << 192) - ZZ(933887);

  zx = RandomBnd(mod192bit);
  zy = RandomBnd(mod192bit);

  ZZToU3(zx, X.data());
  ZZToU3(zy, Y.data());

  asmAddModQ(Z.data(), X.data(), Y.data());
  AddMod(zz, zx, zy, mod192bit);

  assert(zz == U3ToZZ(Z));
  cout << "testModAdd192bits..........    Done" << endl;
}

void testModAdd64bits() {
  uint64_t X, Y, Z;
  ZZ zx, zy, zz;
  ZZ mod64bit = (ZZ(1) << 64) - ZZ(114687);

  zx = RandomBnd(mod64bit);
  zy = RandomBnd(mod64bit);

  BytesFromZZ((unsigned char *)&X, zx, 8);
  BytesFromZZ((unsigned char *)&Y, zy, 8);

  asmAddModP(&Z, &X, &Y);
  AddMod(zz, zx, zy, mod64bit);

  assert(zz == ZZFromBytes((unsigned char *)&Z, 8));
  cout << "testModAdd64bits...........    Done" << endl;
}

void testModSub192bits() {
  vector<uint64_t> X(3), Y(3), Z(3);
  ZZ zx, zy, zz;
  ZZ mod192bit = (ZZ(1) << 192) - ZZ(933887);

  zx = RandomBnd(mod192bit);
  zy = RandomBnd(mod192bit);

  ZZToU3(zx, X.data());
  ZZToU3(zy, Y.data());

  asmSubModQ(Z.data(), X.data(), Y.data());
  SubMod(zz, zx, zy, mod192bit);

  assert(zz == U3ToZZ(Z));
  cout << "testModSub192bits..........    Done" << endl;
}

void testModSub64bits() {
  uint64_t X, Y, Z;
  ZZ zx, zy, zz;
  ZZ mod64bit = (ZZ(1) << 64) - ZZ(114687);

  zx = RandomBnd(mod64bit);
  zy = RandomBnd(mod64bit);

  BytesFromZZ((unsigned char *)&X, zx, 8);
  BytesFromZZ((unsigned char *)&Y, zy, 8);

  asmSubModP(&Z, &X, &Y);
  SubMod(zz, zx, zy, mod64bit);

  assert(zz == ZZFromBytes((unsigned char *)&Z, 8));
  cout << "testModSub64bits...........    Done" << endl;
}

void testModSub64bitsInQ() {
  vector<uint64_t> X(3), Z(1);
  ZZ zx, zz;
  ZZ mod192bit = (ZZ(1) << 192) - ZZ(933887);
  ZZ mod64bit = (ZZ(1) << 64) - ZZ(114687);

  zx = mod192bit - mod64bit;
  cout << zx << endl;
  ZZToU3(zx, X.data());
  asmModPInQ(Z.data(), X.data());
  cout << Z[0] << endl;
  zz = zx % mod64bit;
  cout << zz << endl;
  assert(zz == ZZFromBytes((unsigned char *)&Z[0], 8));
  cout << "testModSub64bitsInQ........    Done" << endl;
}
