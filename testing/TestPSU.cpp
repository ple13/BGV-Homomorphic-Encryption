// g++ -O3 -ftree-vectorize -march=native -msse4.1 -mavx TestPAHE.cpp PolynomialRing.cpp RandomGenerator.cpp Transform.cpp ../Polynomial.cpp PAHE.cpp -lssl -lntl -lgmp -lpthread -std=c++11 -lcrypto

#include "PAHE.h"
#include "PolynomialRing.h"
#include "RandomGenerator.h"
#include "../Polynomial.h"
#include <cassert>

using namespace NTL;

void testPSU(ZZ plaintext_modulus, ZZ ciphertext_modulus, int n);

int main() {
  int n = 4096;
  ZZ plaintext_modulus = conv<ZZ>("17026560423622361089");
  ZZ ciphertext_modulus = conv<ZZ>("71227801505819413206549406819168704169200812033");
  Timer t;  
  testPSU(plaintext_modulus, ciphertext_modulus, n);
  t.Tick("PSU");
  
  return 0;
}

void testPSU(ZZ plaintext_modulus, ZZ ciphertext_modulus, int n) {
  BGV bgv;
  bgv.init(n, ciphertext_modulus, plaintext_modulus);
  bgv.keygen();
  
  SetSeed(ZZ(0));
  
  int deg = 10;
  vector<ZZ> input(10);
  for (int idx = 1; idx <= 10; idx++) {
    input.push_back(ZZ(idx));
  }
  
  ZZ_p::init(plaintext_modulus);
  PolynomialTree *tree = new PolynomialTree();
  buildTree(tree, input.data(), input.size());
  
  vector<ZZ> pX(input.size() + 1);
  for (int idx = 0; idx <= input.size(); idx++) {
    pX[idx] = conv<ZZ>(tree->val[idx]);
  }
  
  for (int idx = 0; idx < input.size(); idx++) {
    ZZ sum = pX[0];
    ZZ x = input[idx];
    for (int jdx = 1; jdx < pX.size(); jdx++) {
      sum = AddMod(sum, MulMod(x, pX[jdx], plaintext_modulus), plaintext_modulus);
      MulMod(x, x, input[idx], plaintext_modulus);
    }
    assert(sum == ZZ(0));
  }
  
  // Input
  vector<ZZ> pt(bgv.params.n);
  for (int idx = 0; idx < n; idx++) {
    pt[idx] = ZZ(idx  + 1);
  }
  
  // Encrypted ciphertexts
  vector<CompactedCiphertext> coefficients(input.size() + 1);
  vector<vector<ZZ>> plaintexts(input.size() + 1, vector<ZZ>(n, ZZ(1)));
  
  for (int idx = 0; idx < input.size() + 1; idx++) {
    plaintexts[idx] = vector<ZZ>(n, pX[idx]);
    auto pt = bgv.tf.packed_encode(plaintexts[idx], bgv.params.plaintext_modulus, bgv.params.logn);
    coefficients[idx] = bgv.compacted_encrypt_with_sk(pt);
  }
  
  vector<ZZ> temp(n, ZZ(1));
  Ciphertext output(n);
  for (int idx = 0; idx < input.size() + 1; idx++) {
    auto pt_enc = bgv.tf.packed_encode(temp, bgv.params.plaintext_modulus, bgv.params.logn);
    auto pt_null = bgv.tf.ToEval(pt_enc, bgv.params.ciphertext_modulus, bgv.params.logn);
    auto c = bgv.EvalMultPlain(coefficients[idx], pt_null);
    bgv.EvalAdd(output, c);
    
    for (int jdx = 0; jdx < n; jdx++) {
      MulMod(temp[jdx], temp[jdx], pt[jdx], plaintext_modulus);
    }
  }
  
  auto d = bgv.decrypt(output);
  d = bgv.tf.packed_decode(d, bgv.params.plaintext_modulus, bgv.params.logn);
  
  for (int idx = 0; idx < 30; idx++) {
      cout << idx << " " << d[idx] << endl;
  }
}