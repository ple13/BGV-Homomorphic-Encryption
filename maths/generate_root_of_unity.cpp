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

// g++ -O3 -msse -mavx generate_root_of_unity.cpp -lssl -lntl -lgmp -lpthread
// -std=c++11 -lcrypto -o generate_root_of_unity

#include <NTL/ZZ.h>

#include <iostream>

using namespace NTL;
using namespace std;

void ZZToU3(NTL::ZZ v) {
  NTL::ZZ one = ZZ(1);
  std::cout << v << std::endl;
  std::cout << v % (one << 64) << std::endl;
  v = (v >> 64);
  std::cout << v % (one << 64) << std::endl;
  v = (v >> 64);
  std::cout << v % (one << 64) << std::endl;
}

NTL::ZZ RootOfUnityPlaintext(uint32_t two_phi_m) {
  NTL::ZZ zero = NTL::ZZ(0);
  NTL::ZZ one = NTL::ZZ(1);
  SetSeed(one);
  NTL::ZZ ret;
  NTL::ZZ modulo = (one << 64) - ZZ(114687);
  NTL::ZZ temp = modulo - one;
  NTL::ZZ exponent = RightShift(temp, log2(two_phi_m));

  while (1) {
    NTL::RandomBnd(ret, modulo);
    if (ret == zero || ret == one) {
      continue;
    }

    ret = NTL::PowerMod(ret, exponent, modulo);

    if (NTL::PowerMod(ret, two_phi_m / 2, modulo) != one &&
        NTL::PowerMod(ret, two_phi_m, modulo) == one) {
      break;
    }
  }

  std::cout << ret << " " << PowerMod(ret, 4096, modulo) << " "
            << PowerMod(ret, 4096 * 2, modulo) << std::endl;
  return ret;
}

NTL::ZZ RootOfUnityCiphertext(uint32_t two_phi_m) {
  NTL::ZZ zero = NTL::ZZ(0);
  NTL::ZZ one = NTL::ZZ(1);
  SetSeed(one);
  NTL::ZZ ret;
  NTL::ZZ modulo = (one << 192) - ZZ(933887);
  NTL::ZZ temp = modulo - one;
  NTL::ZZ exponent = RightShift(temp, log2(two_phi_m));

  while (1) {
    NTL::RandomBnd(ret, modulo);
    if (ret == zero || ret == one) {
      continue;
    }

    ret = NTL::PowerMod(ret, exponent, modulo);

    if (NTL::PowerMod(ret, two_phi_m / 2, modulo) != one &&
        NTL::PowerMod(ret, two_phi_m, modulo) == one) {
      break;
    }
  }

  return ret;
}

int main() {
  auto p = RootOfUnityPlaintext(4096 * 2);
  auto q = RootOfUnityCiphertext(4096 * 2);
  ZZToU3(q);
  cout << "RootOfUnityPlaintext : " << p << endl;
  cout << "RootOfUnityCiphertext: " << q << endl;

  NTL::ZZ one = NTL::ZZ(1);
  NTL::ZZ n = NTL::ZZ(4096);

  NTL::ZZ ciphertextModulus = (one << 192) - ZZ(933887);
  NTL::ZZ plaintextModulus = (one << 64) - ZZ(114687);

  auto pinv = NTL::InvMod(p, plaintextModulus);
  auto qinv = NTL::InvMod(q, ciphertextModulus);

  cout << "InvRootOfUnityPlaintext : " << pinv << endl;
  cout << "InvRootOfUnityCiphertext: " << endl;
  ZZToU3(qinv);

  cout << "n: " << n << endl;
  auto ninvp = NTL::InvMod(n, plaintextModulus);
  auto ninvq = NTL::InvMod(n, ciphertextModulus);

  cout << "nInvModPlaintext : " << ninvp << endl;
  cout << "nInvModCiphertext: " << endl;
  ZZToU3(ninvq);

  return 0;
}
