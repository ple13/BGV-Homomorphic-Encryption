#include "utils.h"
#include <NTL/ZZ.h>
#include <iostream>

std::vector<uint64_t> mul(std::vector<uint64_t> u, uint64_t x) {
  uint64_t plaintext_modulus = 18446744073709436929ULL;
  std::vector<uint64_t> ret(u.size() + 1);
  ret[0] = ((unsigned __int128)x * (unsigned __int128)u[0]) % plaintext_modulus;
  for (int i = 1; i < ret.size()-1; i++) {
    ret[i] = ((unsigned __int128)x * (unsigned __int128)u[i]) % plaintext_modulus;
    ret[i] = ((unsigned __int128)ret[i] + (unsigned __int128)u[i-1]) % plaintext_modulus;
  }
  ret[ret.size()-1] = 1;
  return ret;
}

// P(x) = a_0 + ... + a_n*x^n
std::vector<uint64_t> interpolate(std::vector<uint64_t> roots) {
  if (roots.empty()) {
    return std::vector<uint64_t>();
  }
  uint64_t plaintext_modulus = 18446744073709436929ULL;
  std::vector<uint64_t> c(2);
  // P(x) = -r_0 + x
  c[0] = roots[0] > 0 ? plaintext_modulus - roots[0] : 0;
  c[1] = 1;

  // (c_0, ..., c_i)*(-r_i, 1)
  for (int i = 1; i < roots.size(); i++) {
    unsigned __int128 x = roots[i] > 0 ? plaintext_modulus - roots[i] : 0;
    c = mul(c, x);
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
