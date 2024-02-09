// Copyright 2024 Phi Hung Le
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "BGVTest.h"

#include <iostream>

void BGVTest::testPublicKey() {
  Ciphertext c;
  c.a = bgv.pk.a;
  c.b = bgv.pk.b;

  vector<uint64_t> d = bgv.decrypt(c);

  for (int idx = 0; idx < bgv.n; idx++) {
    assert(d[idx] == 0);
  }
}

void BGVTest::testEncryption() {
  vector<uint64_t> v(bgv.n);

  for (int idx = 0; idx < bgv.n; idx++) {
    v[idx] = idx;
  }

  auto pt = bgv.helper->packed_encode(v);

  Ciphertext c = bgv.encrypt_with_sk(pt);
  vector<uint64_t> d = bgv.decrypt(c);

  d = bgv.helper->packed_decode(d);

  for (int idx = 0; idx < bgv.n; idx++) {
    assert((d[idx] % bgv.helper->plaintextModulus) == v[idx]);
  }
}

void BGVTest::testEncryptionWithPk() {
  vector<uint64_t> v(bgv.n, 10);
  auto pt = bgv.helper->packed_encode(v);

  Ciphertext c = bgv.encrypt_with_pk(pt);

  vector<uint64_t> d = bgv.decrypt(c);

  d = bgv.helper->packed_decode(d);

  for (int idx = 0; idx < bgv.n; idx++) {
    assert(d[idx] == v[idx]);
  }
}

void BGVTest::testPlainMult() {
  vector<uint64_t> v1(bgv.n);
  vector<uint64_t> v2(bgv.n);

  for (int idx = 0; idx < bgv.n; idx++) {
    v1[idx] = idx;
    v2[idx] = idx;
  }

  auto pt1 = bgv.helper->packed_encode(v1);
  auto pt2 = bgv.helper->packed_encode(v2);

  Ciphertext c1 = bgv.encrypt_with_sk(pt1);
  auto c2_null = bgv.helper->ToEval(pt2);

  bgv.EvalMultPlain(c1, c2_null);
  vector<uint64_t> d = bgv.decrypt(c1);

  d = bgv.helper->packed_decode(d);

  for (int idx = 0; idx < bgv.n; idx++) {
    assert((d[idx] % bgv.helper->plaintextModulus) == v1[idx] * v2[idx]);
  }
}

void BGVTest::testSHE() {
  vector<uint64_t> v1(bgv.n);
  vector<uint64_t> v2(bgv.n);
  vector<uint64_t> v3(bgv.n);

  for (int idx = 0; idx < bgv.n; idx++) {
    v1[idx] = idx;
    v2[idx] = idx + 1;
    v3[idx] = idx + 2;
  }

  auto pt1 = bgv.helper->packed_encode(v1);
  auto pt2 = bgv.helper->packed_encode(v2);
  auto pt3 = bgv.helper->packed_encode(v3);

  Ciphertext c1 = bgv.encrypt_with_sk(pt1);
  auto c2_null = bgv.helper->ToEval(pt2);
  auto c3_null = bgv.helper->ToEval(pt3);

  bgv.EvalMultPlain(c1, c2_null);
  bgv.EvalAddPlain(c1, c3_null);

  vector<uint64_t> d = bgv.decrypt(c1);

  d = bgv.helper->packed_decode(d);

  for (int idx = 0; idx < bgv.n; idx++) {
    assert((d[idx] % bgv.helper->plaintextModulus) ==
           (v1[idx] * v2[idx] + v3[idx]));
  }
}

void BGVTest::testSHEwithPK() {
  vector<uint64_t> v1(bgv.n);
  vector<uint64_t> v2(bgv.n);
  vector<uint64_t> v3(bgv.n);

  for (int idx = 0; idx < bgv.n; idx++) {
    v1[idx] = idx;
    v2[idx] = idx + 1;
    v3[idx] = idx + 2;
  }

  auto pt1 = bgv.helper->packed_encode(v1);
  auto pt2 = bgv.helper->packed_encode(v2);
  auto pt3 = bgv.helper->packed_encode(v3);

  Ciphertext c1 = bgv.encrypt_with_pk(pt1);
  auto c2_null = bgv.helper->ToEval(pt2);
  auto c3_null = bgv.helper->ToEval(pt3);

  bgv.EvalMultPlain(c1, c2_null);
  bgv.EvalAddPlain(c1, c3_null);

  vector<uint64_t> d = bgv.decrypt(c1);

  d = bgv.helper->packed_decode(d);

  for (int idx = 0; idx < bgv.n; idx++) {
    assert((d[idx] % bgv.helper->plaintextModulus) ==
           (v1[idx] * v2[idx] + v3[idx]));
  }
}

void BGVTest::benchmark() {
  vector<uint64_t> v1(bgv.n);
  vector<uint64_t> v2(bgv.n);
  vector<uint64_t> v3(bgv.n);

  for (int idx = 0; idx < bgv.n; idx++) {
    v1[idx] = idx;
    v2[idx] = idx + 1;
    v3[idx] = idx + 2;
  }

  auto pt1 = bgv.helper->packed_encode(v1);
  auto pt2 = bgv.helper->packed_encode(v2);
  auto pt3 = bgv.helper->packed_encode(v3);

  Ciphertext c1 = bgv.encrypt_with_pk(pt1);
  auto c2_null = bgv.helper->ToEval(pt2);
  auto c3_null = bgv.helper->ToEval(pt3);
  vector<uint64_t> d = bgv.decrypt(c1);

  Timer t;
  for (int i = 0; i < 100; i++) {
    pt1 = bgv.helper->packed_encode(v1);
  }
  t.Tick("Encode");
  for (int i = 0; i < 100; i++) {
    c1 = bgv.encrypt_with_pk(pt1);
  }
  t.Tick("Encrypt");
  for (int i = 0; i < 100; i++) {
    c2_null = bgv.helper->ToEval(pt2);
  }
  t.Tick("ToEval");
  for (int i = 0; i < 100; i++) {
    bgv.EvalMultPlain(c1, c2_null);
  }
  t.Tick("Mult Plain");
  for (int i = 0; i < 100; i++) {
    bgv.EvalAddPlain(c1, c3_null);
  }
  t.Tick("Add Plain");
  for (int i = 0; i < 100; i++) {
    d = bgv.decrypt(c1);
  }
  t.Tick("Decrypt");
  for (int i = 0; i < 100; i++) {
    bgv.helper->packed_decode(d);
  }
  t.Tick("Decode");
}

void BGVTest::compacted_ciphertext_to_ciphertext() {
  vector<uint64_t> v(bgv.n);

  for (int idx = 0; idx < bgv.n; idx++) {
    v[idx] = idx;
  }

  auto pt = bgv.helper->packed_encode(v);

  CompactedCiphertext cc = bgv.compact_encrypt_with_sk(pt);
  Ciphertext c = bgv.toCiphertext(cc);

  vector<uint64_t> d = bgv.decrypt(c);

  d = bgv.helper->packed_decode(d);

  for (int idx = 0; idx < bgv.n; idx++) {
    assert((d[idx] % bgv.helper->plaintextModulus) == v[idx]);
  }
}
