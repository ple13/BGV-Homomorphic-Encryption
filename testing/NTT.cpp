// g++ -O3 -ftree-vectorize -march=native -msse4.1 -mavx TestNTT.cpp RandomGenerator.cpp Transform.cpp PAHE.cpp PolynomialRing.cpp ../Polynomial.cpp -lssl -lntl -lgmp -lpthread -std=c++11 -lcrypto -l:/usr/local/lib/libcryptopp.a -maes -pthread -mrdseed -rdynamic /usr/local/lib/librelic.so -lboost_system -lgmp /usr/local/lib/libemp-tool.so -Wl,-rpath,/usr/local/lib

#include "PAHE.h"
#include "PolynomialRing.h"
#include "../Polynomial.h"
#include <cassert>

using namespace NTL;

void testNTT();
void testNTTandPolynomialInterpolation();

int main() {

  testNTT();
  testNTTandPolynomialInterpolation();

  return 0;
}

void testNTT() {
  BGV bgv;
  bgv.init(4096, conv<ZZ>("71227801505819413206549406819168704169200812033"), conv<ZZ>("17026560423622361089"));
  bgv.keygen();

  SetSeed(ZZ(0));

  vector<ZZ> v1(bgv.params.n);
  vector<ZZ> v2(bgv.params.n);

  for(int idx = 0; idx < bgv.params.n; idx++) {
    v1[idx] = RandomBnd(bgv.params.plaintext_modulus);
    v2[idx] = RandomBnd(bgv.params.ciphertext_modulus);
  }

  Timer t;
  auto ntt_v1 = bgv.tf.ntt_fwd(v1, bgv.tf.g_rootOfUnityMap[bgv.params.plaintext_modulus], bgv.params.plaintext_modulus, bgv.params.logn);
  t.Tick("NTT plaintext");
  auto ntt_v2 = bgv.tf.ntt_fwd(v2, bgv.tf.g_rootOfUnityMap[bgv.params.ciphertext_modulus], bgv.params.ciphertext_modulus, bgv.params.logn);
  t.Tick("NTT ciphertext");

  ZZ zp2 = MulMod(bgv.params.root_of_unity_plaintext, bgv.params.root_of_unity_plaintext, bgv.params.plaintext_modulus);
  ZZ zc2 = MulMod(bgv.params.root_of_unity_ciphertext, bgv.params.root_of_unity_ciphertext, bgv.params.ciphertext_modulus);

  ZZ temp1 = ZZ(0);
  ZZ temp2 = ZZ(0);

  int index = rand() % bgv.params.n;

  for (int idx = 0; idx < bgv.params.n; idx++) {
    ZZ pow1 = PowerMod(zp2, index*idx, bgv.params.plaintext_modulus);
    ZZ pow2 = PowerMod(zc2, index*idx, bgv.params.ciphertext_modulus);
    temp1 = AddMod(temp1, MulMod(v1[idx], pow1, bgv.params.plaintext_modulus), bgv.params.plaintext_modulus);
    temp2 = AddMod(temp2, MulMod(v2[idx], pow2, bgv.params.ciphertext_modulus), bgv.params.ciphertext_modulus);
  }

  assert(temp1 == ntt_v1[index]);
  assert(temp2 == ntt_v2[index]);
}

void testNTTandPolynomialInterpolation() {
  BGV bgv;
  bgv.init(4096, conv<ZZ>("71227801505819413206549406819168704169200812033"), conv<ZZ>("17026560423622361089"));
  bgv.keygen();

  SetSeed(ZZ(0));

  vector<ZZ> v1(bgv.params.n);
  vector<ZZ> v2(bgv.params.n);

  vector<ZZ> v3(bgv.params.n);
  vector<ZZ> v4(bgv.params.n);

  for(int idx = 0; idx < bgv.params.n/4; idx++) {
    v1[idx] = RandomBnd(bgv.params.plaintext_modulus);
    v2[idx] = RandomBnd(bgv.params.plaintext_modulus);
    v3[idx] = RandomBnd(bgv.params.plaintext_modulus);
    v4[idx] = RandomBnd(bgv.params.plaintext_modulus);
  }

  for(int idx = bgv.params.n/4; idx < bgv.params.n; idx++) {
    v1[idx] = ZZ(0);
    v2[idx] = ZZ(0);
    v3[idx] = ZZ(0);
    v4[idx] = ZZ(0);
  }

  Timer t;
  auto ntt_v1 = bgv.tf.ntt_fwd(v1, bgv.tf.g_rootOfUnityMap[bgv.params.plaintext_modulus], bgv.params.plaintext_modulus, bgv.params.logn);
  auto ntt_v2 = bgv.tf.ntt_fwd(v2, bgv.tf.g_rootOfUnityMap[bgv.params.plaintext_modulus], bgv.params.plaintext_modulus, bgv.params.logn);
  auto ntt_v3 = bgv.tf.ntt_fwd(v3, bgv.tf.g_rootOfUnityMap[bgv.params.plaintext_modulus], bgv.params.plaintext_modulus, bgv.params.logn);
  auto ntt_v4 = bgv.tf.ntt_fwd(v4, bgv.tf.g_rootOfUnityMap[bgv.params.plaintext_modulus], bgv.params.plaintext_modulus, bgv.params.logn);
  t.Tick("NTT forward");

  auto inv_ntt_v1 = bgv.tf.ntt_inv(ntt_v1, bgv.tf.g_rootOfUnityInverseMap[bgv.params.plaintext_modulus], bgv.params.plaintext_modulus, bgv.params.logn);
  t.Tick("NTT inverse");

  assert(v1 == inv_ntt_v1);

  auto ntt_v12 = ntt_v1;
  auto ntt_v34 = ntt_v3;

  vector<ZZ> ntt(bgv.params.n);
  for (int idx = 0; idx < ntt_v1.size(); idx++) {
    ntt_v12[idx] = MulMod(ntt_v12[idx], ntt_v2[idx], bgv.params.plaintext_modulus);
    ntt_v34[idx] = MulMod(ntt_v34[idx], ntt_v4[idx], bgv.params.plaintext_modulus);
    ntt[idx]     = AddMod(ntt_v12[idx], ntt_v34[idx], bgv.params.plaintext_modulus);
  }

  vector<ZZ> roots(bgv.params.n/2);
  for (int idx = 0; idx < roots.size(); idx++) {
    roots[idx] = bgv.tf.g_rootOfUnityMap[bgv.params.plaintext_modulus][idx];
    roots[idx] = MulMod(roots[idx], roots[idx], bgv.params.plaintext_modulus);
  }

  ZZ_p::init(bgv.params.plaintext_modulus);

  PolynomialTree *tree = new PolynomialTree();
  buildTree(tree, roots.data(), roots.size());
  ZZ_pX derivativePolynomial;
  int size = roots.size();

  derivativePolynomial.SetLength(size);

  for(int idx = 0; idx < size; idx++) {
    derivativePolynomial[idx] = (tree->val[idx + 1]*(idx + 1));
  }

//   ntt_v1.resize(bgv.params.n/2 + 1);

  downtreeDivisionReconstruct(ntt.data(), tree, derivativePolynomial, 0, size);

  t.Tick("Interpolation");

  vector<ZZ> output;
  for (int idx = 0; idx <= deg(tree->interp); idx++) {
    output.push_back(conv<ZZ>(tree->interp[idx]));
  }

  auto v12 = MulPolynomialRing(v1, v2, bgv.params.plaintext_modulus);
  auto v34 = MulPolynomialRing(v3, v4, bgv.params.plaintext_modulus);

  for (int idx = 0; idx < v12.size(); idx++) {
    v12[idx] = AddMod(v12[idx], v34[idx], bgv.params.plaintext_modulus);
  }

  v12.resize(deg(tree->interp) + 1, ZZ(0));

  assert(v12 == output);
}
