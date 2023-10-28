// g++ -O3 -ftree-vectorize -march=native -msse4.1 -mavx TestPolynomialRing.cpp PolynomialRing.cpp RandomGenerator.cpp Transform.cpp PAHE.cpp -lssl -lntl -lgmp -lpthread -std=c++11 -lcrypto -l:/usr/local/lib/libcryptopp.a -maes -pthread -mrdseed -rdynamic /usr/local/lib/librelic.so -lboost_system -lgmp /usr/local/lib/libemp-tool.so -Wl,-rpath,/usr/local/lib
#include <NTL/ZZ_pX.h>
#include "PAHE.h"
#include "PolynomialRing.h"
#include "assert.h"

void testNTLMul();
void testPolynomialMult();
void testPolynomialMultFTT();
void testPackedPlaintext();

using namespace NTL;

int main() {
  testNTLMul();
  testPolynomialMult();
  testPolynomialMultFTT();
  testPackedPlaintext();
  return 0;
}

void testNTLMul() {
  ZZ plaintext_modulus = conv<ZZ>("17026560423622361089");
  ZZ_p::init(plaintext_modulus);
  
  ZZ_pX p;
  ZZ_pX p1;
  ZZ_pX p2;
  
  random(p1, 1048576);
  random(p2, 1048576);
  
  vector<ZZ> pp, pp1, pp2;
  for (int idx = 0; idx < 1048576; idx++) {
    pp1.push_back(RandomBnd(plaintext_modulus));
    pp2.push_back(RandomBnd(plaintext_modulus));
  }
  
  Timer t;
  mul(p, p1, p2);
  t.Tick("NTL Mul");
//   pp = MulPolynomialRing(pp1, pp2, plaintext_modulus);
//   t.Tick("ZZ Mul");
}

void testPolynomialMult() {
  ZZ modulus = conv<ZZ>("23068673");
  vector<ZZ> x(20), y(20);
  for (int idx = 0; idx < x.size(); idx++) {
    x[idx] = ZZ(1);
    y[idx] = ZZ(1);
  }
  
  auto z = MulPolynomialRing(x, y, modulus);
  
  for (int idx = 0; idx < x.size(); idx++) {
    ZZ res = SubMod(conv<ZZ>(2*(idx+1)), conv<ZZ>(x.size()), modulus);
    assert(z[idx] == res);
  }
}

void testPolynomialMultFTT() {
  int n = 2048;
  BGV bgv;
  bgv.init(2048, conv<ZZ>("792704719208255489"), conv<ZZ>("23068673"));
  
  SetSeed(ZZ(0));
  
  vector<ZZ> v1(bgv.params.n);
  vector<ZZ> v2(bgv.params.n);
  
  for(int idx = 0; idx < bgv.params.n; idx++) {
    v1[idx] = RandomBnd(bgv.params.ciphertext_modulus);
    v2[idx] = RandomBnd(bgv.params.ciphertext_modulus);
  }
  
  auto ftt_v1 = bgv.tf.ftt_fwd(v1, bgv.params.ciphertext_modulus, bgv.params.logn);
  auto ftt_v2 = bgv.tf.ftt_fwd(v2, bgv.params.ciphertext_modulus, bgv.params.logn);
  
  vector<ZZ> ftt_prod(n);
  for (int idx = 0; idx < bgv.params.n; idx++) {
    ftt_prod[idx] = MulMod(ftt_v1[idx], ftt_v2[idx], bgv.params.ciphertext_modulus);
  }
  
  auto prod = bgv.tf.ftt_inv(ftt_prod, bgv.params.ciphertext_modulus, bgv.params.logn);
    
  auto prod2 = MulPolynomialRing(v1, v2, bgv.params.ciphertext_modulus);
    
  for (int idx = 0; idx < bgv.params.n; idx++) {
    assert(prod[idx] == prod2[idx]);
  }
}

void testPackedPlaintext() {
  int n = 2048;
  BGV bgv;
  bgv.init(2048, conv<ZZ>("792704719208255489"), conv<ZZ>("23068673"));
  
  SetSeed(ZZ(0));
  
  vector<ZZ> v1(bgv.params.n);
  vector<ZZ> v2(bgv.params.n);
  
  for(int idx = 0; idx < bgv.params.n; idx++) {
    v1[idx] = RandomBnd(bgv.params.plaintext_modulus);
    v2[idx] = RandomBnd(bgv.params.plaintext_modulus);
  }
  
//   auto iftt_v1 = bgv.tf.ftt_inv(v1, bgv.params.plaintext_modulus, bgv.params.logn);
//   auto iftt_v2 = bgv.tf.ftt_inv(v2, bgv.params.plaintext_modulus, bgv.params.logn);
  auto iftt_v1 = bgv.tf.packed_encode(v1, bgv.params.plaintext_modulus, bgv.params.logn);
  auto iftt_v2 = bgv.tf.packed_encode(v2, bgv.params.plaintext_modulus, bgv.params.logn);
  
  vector<ZZ> iftt_prod = MulPolynomialRing(iftt_v1, iftt_v2, bgv.params.plaintext_modulus);
  
  auto prod = bgv.tf.packed_decode(iftt_prod, bgv.params.plaintext_modulus, bgv.params.logn);
//   auto prod = bgv.tf.ftt_fwd(iftt_prod, bgv.params.plaintext_modulus, bgv.params.logn);
  
  vector<ZZ> prod2(n);
  for (int idx = 0; idx < bgv.params.n; idx++) {
    prod2[idx] = MulMod(v1[idx], v2[idx], bgv.params.plaintext_modulus);
  }
  
  for (int idx = 0; idx < bgv.params.n; idx++) {
    assert(prod[idx] == prod2[idx]);
  }
}