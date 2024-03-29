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

#ifndef __PAHE_H__
#define __PAHE_H__

#include <NTL/ZZ.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>
#include <vector>

#include "../utils/Timer.h"
#include "PAHEHelper.h"
#include "RandomGenerator.h"

#define SEED_SIZE 16

using namespace std;

#define BYTES_PER_CIPHERTEXT_MODULUS 3

struct Ciphertext {
  vector<vector<uint64_t>> a;
  vector<vector<uint64_t>> b;
  Ciphertext(int n = 4096) {
    a.resize(n, vector<uint64_t>(3));
    b.resize(n, vector<uint64_t>(3));
  }
};

struct CompactedCiphertext {
  vector<vector<uint64_t>> a;
  vector<unsigned char> b;
  CompactedCiphertext(int n = 4096) {
    a.resize(n, vector<uint64_t>(3));
    b.resize(2 * SEED_SIZE);
  }
};

struct Plaintext {
  vector<uint64_t> pt;
  Plaintext() {}
  Plaintext(int n) { pt.resize(n, 0); }
};

class BGV {
 public:
  void init(int n = 4096, bool use_packing = true);
  void keygen();

  Ciphertext encrypt_with_sk(vector<uint64_t>& pt);
  Ciphertext encrypt_with_pk(vector<uint64_t>& pt);

  Ciphertext encrypt_noise_flooding_with_pk();

  CompactedCiphertext compact_encrypt_with_sk(vector<uint64_t>& pt);
  Ciphertext toCiphertext(CompactedCiphertext compactedCiphertext);

  void EvalAdd(Ciphertext& ct1, Ciphertext& ct2);
  void EvalAddPlain(Ciphertext& ct, vector<uint64_t>& pt);
  void EvalAddPlain(Ciphertext& ct, vector<vector<uint64_t>>& eval_pt);
  void EvalMultPlain(Ciphertext& ct, vector<uint64_t>& pt);
  void EvalMultPlain(Ciphertext& ct, vector<vector<uint64_t>>& eval_pt);
  void EvalMultPlain(Ciphertext& dst, Ciphertext& ct, vector<uint64_t>& pt);
  void EvalMultPlain(Ciphertext& dst, Ciphertext& ct,
                     vector<vector<uint64_t>>& eval_pt);

  vector<uint64_t> decrypt(Ciphertext& c);
  unsigned int n;    /* 4096 */
  unsigned int logn; /* 12 */

  uint64_t root_of_unity_plaintext;
  vector<uint64_t> root_of_unity_ciphertext;

  Ciphertext pk;
  vector<vector<uint64_t>> sk;

  PAHEHelper* helper;
  NoiseGenerator* prng;
};

#endif
