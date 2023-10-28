// g++ -O3 -ftree-vectorize -march=native -msse4.1 -mavx TestPAHE.cpp RandomGenerator.cpp Transform.cpp  PAHE.cpp -lssl -lntl -lgmp -lpthread -std=c++11 -lcrypto

#include "PAHE.h"
#include "RandomGenerator.h"
#include <cassert>
#include <vector>

using namespace NTL;
using namespace std;

void testPublicKey(ZZ plaintext_modulus, ZZ ciphertext_modulus, int n);
void testEncryption(ZZ plaintext_modulus, ZZ ciphertext_modulus, int n);
void testEncryptionWithPk(ZZ plaintext_modulus, ZZ ciphertext_modulus, int n);
void testPlainMult(ZZ plaintext_modulus, ZZ ciphertext_modulus, int n);
void testCompactedPlainMult(ZZ plaintext_modulus, ZZ ciphertext_modulus, int n);
void testSHE(ZZ plaintext_modulus, ZZ ciphertext_modulus, int n);
void testSHEwithPK(ZZ plaintext_modulus, ZZ ciphertext_modulus, int n);
void testPolynomialEval(ZZ plaintext_modulus, ZZ ciphertext_modulus, int n);
void testSHEPackedEncode(ZZ plaintext_modulus, ZZ ciphertext_modulus, int n);

int main() {
  int n = 4096;
  ZZ plaintext_modulus = conv<ZZ>("17026560423622361089");
  ZZ ciphertext_modulus = conv<ZZ>("71227801505819413206549406819168704169200812033");
  Timer t;
  testPublicKey(plaintext_modulus, ciphertext_modulus, n);
  t.Tick("Public Key test");
  
  testEncryption(plaintext_modulus, ciphertext_modulus, n);
  t.Tick("Encryption");
  
  testEncryptionWithPk(plaintext_modulus, ciphertext_modulus, n);
  t.Tick("Encryption With PK");
  
  testPlainMult(plaintext_modulus, ciphertext_modulus, n);
  t.Tick("Plain Mult");
  
  testCompactedPlainMult(plaintext_modulus, ciphertext_modulus, n);
  t.Tick("Compacted Plain Mult");
  
  testSHE(plaintext_modulus, ciphertext_modulus, n);
  t.Tick("SHE");
  
  testSHEwithPK(plaintext_modulus, ciphertext_modulus, n);
  t.Tick("SHE with PK");
  
  testPolynomialEval(plaintext_modulus, ciphertext_modulus, n);
  t.Tick("Polynomial eval");
  
  testSHEPackedEncode(plaintext_modulus, ciphertext_modulus, n);
  t.Tick("SHE packed encode/decode");
  return 0;
}

void testPublicKey(ZZ plaintext_modulus, ZZ ciphertext_modulus, int n) {
  BGV bgv;
  bgv.init(n, ciphertext_modulus, plaintext_modulus);
  bgv.keygen();
  
  Ciphertext c;
  c.a = bgv.params.pk.a;
  c.b = bgv.params.pk.b;
  
  vector<ZZ> d = bgv.decrypt(c);
    
  for(int idx = 0; idx < bgv.params.n; idx++) {
    assert(d[idx] == ZZ(0));
  }
}

void testEncryption(ZZ plaintext_modulus, ZZ ciphertext_modulus, int n) {
  BGV bgv;
  bgv.init(n, ciphertext_modulus, plaintext_modulus);
  bgv.keygen();
  
  SetSeed(ZZ(0));
  
  vector<ZZ> v(bgv.params.n);
  
  for(int idx = 0; idx < bgv.params.n; idx++) {
    v[idx] = RandomBnd(10);
  }
    
  auto pt = bgv.tf.packed_encode(v, bgv.params.plaintext_modulus, bgv.params.logn);
    
  Ciphertext c = bgv.encrypt_with_sk(pt);    
  vector<ZZ> d = bgv.decrypt(c);
  
  auto dd = bgv.tf.packed_decode(d, bgv.params.plaintext_modulus, bgv.params.logn);
  
  for(int idx = 0; idx < bgv.params.n; idx++) {
    assert(dd[idx] == v[idx]);
  }
}

void testEncryptionWithPk(ZZ plaintext_modulus, ZZ ciphertext_modulus, int n) {
  BGV bgv;
  bgv.init(n, ciphertext_modulus, plaintext_modulus);
  bgv.keygen();
  
  SetSeed(ZZ(0));
  
  vector<ZZ> v(bgv.params.n);
  
  for(int idx = 0; idx < bgv.params.n; idx++) {
    v[idx] = RandomBnd(10);
  }
    
  auto pt = bgv.tf.packed_encode(v, bgv.params.plaintext_modulus, bgv.params.logn);
  
  Ciphertext c = bgv.encrypt_with_pk(pt, ZZ(0));    
  vector<ZZ> d = bgv.decrypt(c);
  
  d = bgv.tf.packed_decode(d, bgv.params.plaintext_modulus, bgv.params.logn);
  
  for(int idx = 0; idx < bgv.params.n; idx++) {
    assert(d[idx] == v[idx]);
  }
}

void testPlainMult(ZZ plaintext_modulus, ZZ ciphertext_modulus, int n) {
  BGV bgv;
  bgv.init(n, ciphertext_modulus, plaintext_modulus);
  bgv.keygen();
  
  srand (time(NULL));
  SetSeed(ZZ(rand()));
  
  vector<ZZ> v1(bgv.params.n);
  vector<ZZ> v2(bgv.params.n);
  
  for(int idx = 0; idx < bgv.params.n; idx++) {
    v1[idx] = RandomBnd(ZZ(bgv.params.plaintext_modulus));
    v2[idx] = RandomBnd(ZZ(bgv.params.plaintext_modulus));
  }
  
  auto pt1 = bgv.tf.packed_encode(v1, bgv.params.plaintext_modulus, bgv.params.logn);
  auto pt2 = bgv.tf.packed_encode(v2, bgv.params.plaintext_modulus, bgv.params.logn);
  
  Ciphertext c1 = bgv.encrypt_with_sk(pt1);  
  auto c2_null  = bgv.tf.ToEval(pt2, bgv.params.ciphertext_modulus, bgv.params.logn);
  
  bgv.EvalMultPlain(c1, c2_null);
    
  vector<ZZ> d = bgv.decrypt(c1);
  
  d = bgv.tf.packed_decode(d, bgv.params.plaintext_modulus, bgv.params.logn);
  
  for(int idx = 0; idx < bgv.params.n; idx++) {
    rem(d[idx], d[idx], bgv.params.plaintext_modulus);
    assert(d[idx] == MulMod(v1[idx], v2[idx], bgv.params.plaintext_modulus));
  }
}

void testCompactedPlainMult(ZZ plaintext_modulus, ZZ ciphertext_modulus, int n) {
  BGV bgv;
  bgv.init(n, ciphertext_modulus, plaintext_modulus);
  bgv.keygen();
  
  SetSeed(ZZ(0));
  
  vector<ZZ> v1(bgv.params.n);
  vector<ZZ> v2(bgv.params.n);
  
  for(int idx = 0; idx < bgv.params.n; idx++) {
    v1[idx] = RandomBnd(ZZ(3000));
    v2[idx] = RandomBnd(ZZ(3000));
  }
  
  auto pt1 = bgv.tf.packed_encode(v1, bgv.params.plaintext_modulus, bgv.params.logn);
  auto pt2 = bgv.tf.packed_encode(v2, bgv.params.plaintext_modulus, bgv.params.logn);
  
  CompactedCiphertext c1 = bgv.compacted_encrypt_with_sk(pt1);  
  auto c2_null  = bgv.tf.ToEval(pt2, bgv.params.ciphertext_modulus, bgv.params.logn);
  
  Ciphertext c2 = bgv.EvalMultPlain(c1, c2_null);
    
  vector<ZZ> d = bgv.decrypt(c2);
  
  d = bgv.tf.packed_decode(d, bgv.params.plaintext_modulus, bgv.params.logn);
  
  for(int idx = 0; idx < bgv.params.n; idx++) {
    rem(d[idx], d[idx], bgv.params.plaintext_modulus);
    assert(d[idx] == MulMod(v1[idx], v2[idx], bgv.params.plaintext_modulus));
  }
}

void testSHE(ZZ plaintext_modulus, ZZ ciphertext_modulus, int n) {
  BGV bgv;
  bgv.init(n, ciphertext_modulus, plaintext_modulus);
  bgv.keygen();
  
  SetSeed(ZZ(0));
  
  vector<ZZ> v1(bgv.params.n);
  vector<ZZ> v2(bgv.params.n);
  vector<ZZ> v3(bgv.params.n);
  
  for(int idx = 0; idx < bgv.params.n; idx++) {
    v1[idx] = RandomBnd(ZZ(3000));
    v2[idx] = RandomBnd(ZZ(3000));
    v3[idx] = RandomBnd(bgv.params.plaintext_modulus);
  }
  
  auto pt1 = bgv.tf.packed_encode(v1, bgv.params.plaintext_modulus, bgv.params.logn);
  auto pt2 = bgv.tf.packed_encode(v2, bgv.params.plaintext_modulus, bgv.params.logn);
  auto pt3 = bgv.tf.packed_encode(v3, bgv.params.plaintext_modulus, bgv.params.logn);
  
  CompactedCiphertext c1 = bgv.compacted_encrypt_with_sk(pt1);  
  auto c2_null  = bgv.tf.ToEval(pt2, bgv.params.ciphertext_modulus, bgv.params.logn);
  auto c3_null  = bgv.tf.ToEval(pt3, bgv.params.ciphertext_modulus, bgv.params.logn);
  
  Ciphertext c2 = bgv.EvalMultPlain(c1, c2_null);
  for (int idx = 0; idx < bgv.params.n; idx++) {
    c2.a[idx] = AddMod(c2.a[idx], c3_null[idx], bgv.params.ciphertext_modulus);
  }
  
  vector<ZZ> d = bgv.decrypt(c2);
  
  d = bgv.tf.packed_decode(d, bgv.params.plaintext_modulus, bgv.params.logn);
  
  for(int idx = 0; idx < bgv.params.n; idx++) {
    rem(d[idx], d[idx], bgv.params.plaintext_modulus);
    auto res = MulMod(v1[idx], v2[idx], bgv.params.plaintext_modulus);
    res = AddMod(res, v3[idx], bgv.params.plaintext_modulus);
    assert(d[idx] == res);
  }
}

void testSHEwithPK(ZZ plaintext_modulus, ZZ ciphertext_modulus, int n) {
  BGV bgv;
  bgv.init(n, ciphertext_modulus, plaintext_modulus);
  bgv.keygen();
  
  SetSeed(ZZ(0));
  
  vector<ZZ> v1(bgv.params.n);
  vector<ZZ> v2(bgv.params.n);
  vector<ZZ> v3(bgv.params.n);
  
  for(int idx = 0; idx < bgv.params.n; idx++) {
    v1[idx] = RandomBnd(ZZ(3000));
    v2[idx] = RandomBnd(ZZ(3000));
    v3[idx] = RandomBnd(bgv.params.plaintext_modulus);
  }
  
  auto pt1 = bgv.tf.packed_encode(v1, bgv.params.plaintext_modulus, bgv.params.logn);
  auto pt2 = bgv.tf.packed_encode(v2, bgv.params.plaintext_modulus, bgv.params.logn);
  auto pt3 = bgv.tf.packed_encode(v3, bgv.params.plaintext_modulus, bgv.params.logn);
  
  Ciphertext c1 = bgv.encrypt_with_pk(pt1, RandomBnd(ZZ(ciphertext_modulus)));  
  auto c2_null  = bgv.tf.ToEval(pt2, bgv.params.ciphertext_modulus, bgv.params.logn);
  auto c3_null  = bgv.tf.ToEval(pt3, bgv.params.ciphertext_modulus, bgv.params.logn);
  
  bgv.EvalMultPlain(c1, c2_null);
  for (int idx = 0; idx < bgv.params.n; idx++) {
    c1.a[idx] = AddMod(c1.a[idx], c3_null[idx], bgv.params.ciphertext_modulus);
  }
  
  vector<ZZ> d = bgv.decrypt(c1);
  
  d = bgv.tf.packed_decode(d, bgv.params.plaintext_modulus, bgv.params.logn);
  
  for(int idx = 0; idx < bgv.params.n; idx++) {
    rem(d[idx], d[idx], bgv.params.plaintext_modulus);
    auto res = MulMod(v1[idx], v2[idx], bgv.params.plaintext_modulus);
    res = AddMod(res, v3[idx], bgv.params.plaintext_modulus);
    assert(d[idx] == res);
  }
}

void testPolynomialEval(ZZ plaintext_modulus, ZZ ciphertext_modulus, int n) {
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

void testSHEPackedEncode(ZZ plaintext_modulus, ZZ ciphertext_modulus, int n) {
  BGV bgv;
  bgv.init(n, ciphertext_modulus, plaintext_modulus);
  bgv.keygen();
  
  SetSeed(ZZ(0));
  
  vector<ZZ> v1(bgv.params.n);
  vector<ZZ> v2(bgv.params.n);
  vector<ZZ> v3(bgv.params.n);
  
  for(int idx = 0; idx < bgv.params.n; idx++) {
    v1[idx] = RandomBnd(ZZ(3000));
    v2[idx] = RandomBnd(ZZ(3000));
    v3[idx] = MulMod(v1[idx], v2[idx], bgv.params.plaintext_modulus);
  }
  
  auto pt1 = bgv.tf.packed_encode(v1, bgv.params.plaintext_modulus, bgv.params.logn);
  auto pt2 = bgv.tf.packed_encode(v2, bgv.params.plaintext_modulus, bgv.params.logn);
  auto pt3 = bgv.tf.packed_encode(v3, bgv.params.plaintext_modulus, bgv.params.logn);
  
  for (int idx = 0; idx < bgv.params.n; idx++) {
    pt3[idx] = MulMod(pt1[idx], pt2[idx], plaintext_modulus);
  }
  
  auto d = bgv.tf.packed_decode(pt3, bgv.params.plaintext_modulus, bgv.params.logn);
  
  for(int idx = 0; idx < bgv.params.n; idx++) {
    assert(d[idx] == v3[idx]);
  }
}