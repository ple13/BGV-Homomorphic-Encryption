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

#include "polynomial_interpolation.h"

#include <NTL/ZZ.h>

#include <iostream>

namespace maths {

std::vector<uint64_t> mulMod(std::vector<uint64_t> u, uint64_t x,
                             uint64_t plaintext_modulus) {
  std::vector<uint64_t> ret(u.size() + 1);
  ret[0] = ((unsigned __int128)x * (unsigned __int128)u[0]) % plaintext_modulus;
  for (int i = 1; i < ret.size() - 1; i++) {
    ret[i] =
        ((unsigned __int128)x * (unsigned __int128)u[i]) % plaintext_modulus;
    ret[i] = ((unsigned __int128)ret[i] + (unsigned __int128)u[i - 1]) %
             plaintext_modulus;
  }
  ret[ret.size() - 1] = 1;
  return ret;
}

// Example modulus: 18446744073709436929ULL
// P(x) = a_0 + ... + a_n*x^n
std::vector<uint64_t> interpolate(std::vector<uint64_t> roots,
                                  uint64_t plaintext_modulus) {
  if (roots.empty()) {
    return std::vector<uint64_t>();
  }
  std::vector<uint64_t> c(2);
  // P(x) = -r_0 + x
  c[0] = roots[0] > 0 ? plaintext_modulus - roots[0] : 0;
  c[1] = 1;

  // (c_0, ..., c_i)*(-r_i, 1)
  for (int i = 1; i < roots.size(); i++) {
    unsigned __int128 x = roots[i] > 0 ? plaintext_modulus - roots[i] : 0;
    c = mulMod(c, x, plaintext_modulus);
  }
  return c;
}

uint64_t invModP(uint64_t val) {
  NTL::ZZ plaintext_modulus = (NTL::ZZ(1) << 64) - NTL::ZZ(114687);
  NTL::ZZ x = NTL::ZZFromBytes((unsigned char *)&val, 8);
  NTL::ZZ inv = InvMod(x, plaintext_modulus);
  uint64_t ret = 0;
  NTL::BytesFromZZ((unsigned char *)&ret, inv, 8);
  return ret;
}

}  // namespace maths
