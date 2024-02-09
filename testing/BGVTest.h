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

#ifndef BGV_TEST_H__
#define BGV_TEST_H__

#include <cassert>
#include <random>
#include <vector>

#include "../PAHE/PAHE.h"

class BGVTest {
 public:
  BGVTest() {
    bgv.init();
    bgv.keygen();
  }

  void testPublicKey();
  void testEncryption();
  void testEncryptionWithPk();
  void testPlainMult();
  void testCompactedPlainMult();
  void testSHE();
  void testSHEwithPK();
  void testPolynomialEval();
  void testSHEPackedEncode();
  void benchmark();
  void compacted_ciphertext_to_ciphertext();

  BGV bgv;
};

#endif
