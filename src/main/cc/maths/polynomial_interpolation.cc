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

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "common_cpp/macros/macros.h"

namespace maths {

absl::StatusOr<uint64_t> mulMod(uint64_t x, uint64_t y, uint64_t modulus) {
  if (modulus < 2) {
    return absl::InvalidArgumentError("Modulus must be greater than 1.");
  }
  if (x >= modulus && y >= modulus) {
    return absl::InvalidArgumentError(
        "Both multiplicand and multiplier must be less than modulus.");
  } else if (x >= modulus) {
    return absl::InvalidArgumentError(
        "The multiplicand x must be less than modulus.");
  } else if (y >= modulus) {
    return absl::InvalidArgumentError(
        "The multiplier y must be less than modulus.");
  }
  return ((unsigned __int128)x * (unsigned __int128)y) % modulus;
}

absl::StatusOr<std::vector<uint64_t>> mulMod(std::vector<uint64_t> u,
                                             uint64_t x,
                                             uint64_t plaintext_modulus) {
  std::vector<uint64_t> ret(u.size() + 1);
  ASSIGN_OR_RETURN(ret[0], mulMod(u[0], x, plaintext_modulus));
  for (int i = 1; i < ret.size() - 1; i++) {
    ASSIGN_OR_RETURN(ret[i], mulMod(u[i], x, plaintext_modulus));
    ASSIGN_OR_RETURN(ret[i], mulMod(ret[i], u[i - 1], plaintext_modulus));
  }
  ret[ret.size() - 1] = 1;
  return ret;
}

// Example modulus: 18446744073709436929ULL
// P(x) = a_0 + ... + a_n*x^n
absl::StatusOr<std::vector<uint64_t>> interpolate(std::vector<uint64_t> roots,
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
    ASSIGN_OR_RETURN(c, mulMod(c, x, plaintext_modulus));
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
