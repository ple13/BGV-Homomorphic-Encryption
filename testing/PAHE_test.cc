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

#include "../PAHE/PAHE.h"

#include <cassert>
#include <random>
#include <vector>

#include "BGVTest.h"

using namespace std;

int main() {
  int batch = 100;
  BGVTest bgv;
  Timer t;
  for (int i = 0; i < batch; i++) {
    bgv.testPublicKey();
  }
  t.Tick("Public Key test");

  for (int i = 0; i < batch; i++) {
    bgv.testEncryption();
  }
  t.Tick("Encryption");

  for (int i = 0; i < batch; i++) {
    bgv.testEncryptionWithPk();
  }
  t.Tick("Encryption With PK");
  for (int i = 0; i < batch; i++) {
    bgv.testPlainMult();
  }
  t.Tick("Plain Mult");

  for (int i = 0; i < batch; i++) {
    bgv.testSHE();
  }
  t.Tick("SHE");
  for (int i = 0; i < batch; i++) {
    bgv.testSHEwithPK();
  }
  t.Tick("SHE with PK");

  cout << "Running benchmark test" << endl;
  bgv.benchmark();

  bgv.compacted_ciphertext_to_ciphertext();
  cout << "Compacted ciphertext to ciphertext....    PASSED" << endl;

  return 0;
}
