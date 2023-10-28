#include "PAHEHelper.h"
#include <future>
#include <cassert>
#include <iostream>
#include <NTL/ZZ.h>


#define THREADING true

// precomputed reverse of a byte

inline static unsigned char reverse_byte(unsigned char x)
{
  static  unsigned char table[] = {
    0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
    0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
    0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
    0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
    0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
    0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
    0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
    0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
    0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
    0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
    0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
    0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
    0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
    0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
    0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
    0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
    0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
    0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
    0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
    0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
    0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
    0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
    0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
    0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
    0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
    0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
    0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
    0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
    0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
    0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
    0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
    0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff,
  };
  return table[x];
}

/* Function to reverse bits of num */
uint32_t ReverseBits(uint32_t num, uint32_t msb)
{
  uint32_t result;
  unsigned char * p = (unsigned char *) &num;
  unsigned char * q = (unsigned char *) &result;
  q[3] = reverse_byte(p[0]);
  q[2] = reverse_byte(p[1]);
  q[1] = reverse_byte(p[2]);
  q[0] = reverse_byte(p[3]);
  return (result) >> (32-msb);
}

uint32_t lopow2(uint32_t v){
  // Find log2 when v is a known power of two
   uint32_t b[] = {0xAAAAAAAA, 0xCCCCCCCC, 0xF0F0F0F0, 0xFF00FF00, 0xFFFF0000};
  uint32_t r = (v & b[0]) != 0;
  r |= ((v & b[1]) != 0) << 1;
  r |= ((v & b[2]) != 0) << 2;
  r |= ((v & b[3]) != 0) << 3;
  r |= ((v & b[4]) != 0) << 4;

  return r;
}

uint32_t nxt_pow2(uint32_t x){
  x -= 1;

  // Fill LSB with ones
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;

  return x+1;
}

uint32_t num_ones(uint32_t x){
  uint32_t bits_set = 0;
  while(x != 0){
    x = x & (x-1);
    bits_set++;
  }
  return bits_set;
}

vector<uint64_t> PAHEHelper::ntt_forward_plaintext(
   vector<uint64_t>& data, vector<uint64_t> RoUTable) {
  vector<uint64_t> result(phim);

  //reverse coefficients (bit reversal)
  for (uint32_t i = 0; i < phim; i++) {
      result[i] = data[reverseTablePlaintext[i]];
  }

  uint64_t omegaFactor;
  uint64_t butterflyPlus;
  uint64_t butterflyMinus;

  for (uint32_t logm = 1; logm <= logn; logm++) {
    for (uint32_t j = 0; j < phim; j = j + (1 << logm)) {
      for (uint32_t i = 0; i < (uint32_t)(1 << (logm-1)); i++) {
        uint32_t x = (i << (1+logn-logm));

        uint64_t omega = RoUTable[x];

        uint32_t indexEven = j + i;
        uint32_t indexOdd = j + i + (1 << (logm-1));

        if (result[indexOdd] != 0) {
          asmMulModP(&omegaFactor, &result[indexOdd], &omega);
          asmAddModP(&butterflyPlus, &result[indexEven], &omegaFactor);
          asmSubModP(&butterflyMinus, &result[indexEven], &omegaFactor);

          result[indexEven] = butterflyPlus;
          result[indexOdd] = butterflyMinus;
        } else {
          result[indexOdd] = result[indexEven];
        }
      }
    }
  }

  return result;
}

vector<vector<uint64_t>> PAHEHelper::ntt_forward_ciphertext(
   vector<vector<uint64_t>>& data, vector<vector<uint64_t>> RoUTable) {
  vector<vector<uint64_t>> result(phim);
  //reverse coefficients (bit reversal)
  for (uint32_t i = 0; i < phim; i++) {
      result[i] = data[reverseTableCiphertext[i]];
  }

  vector<uint64_t> omegaFactor(3);
  vector<uint64_t> butterflyPlus(3);
  vector<uint64_t> butterflyMinus(3);

  for (uint32_t logm = 1; logm <= logn; logm++) {
    for (uint32_t j = 0; j < phim; j = j + (1 << logm)) {
      for (uint32_t i = 0; i < (uint32_t)(1 << (logm-1)); i++) {
        uint32_t x = (i << (1+logn-logm));

        vector<uint64_t> omega = RoUTable[x];

        uint32_t indexEven = j + i;
        uint32_t indexOdd = j + i + (1 << (logm-1));

        if (result[indexOdd][0] != 0 || result[indexOdd][1] != 0 || result[indexOdd][2] != 0) {
          asmMulModQ(omegaFactor.data(), result[indexOdd].data(), omega.data());
          asmAddModQ(butterflyPlus.data(), result[indexEven].data(), omegaFactor.data());
          asmSubModQ(butterflyMinus.data(), result[indexEven].data(), omegaFactor.data());

          result[indexEven] = butterflyPlus;
          result[indexOdd] = butterflyMinus;
        } else {
          result[indexOdd] = result[indexEven];
        }
      }
    }
  }

  return result;
}

vector<uint64_t> PAHEHelper::ntt_inverse_plaintext(
   vector<uint64_t>& data) {
  auto ret = ntt_forward_plaintext(data, RoUInverseTablePlaintext);
  for (int idx = 0; idx < ret.size(); idx++) {
    asmMulModP(&ret[idx], &nInvPlaintext, &ret[idx]);
  }
  return ret;
}

vector<vector<uint64_t>> PAHEHelper::ntt_inverse_ciphertext(
   vector<vector<uint64_t>>& data) {
  auto ret = ntt_forward_ciphertext(data, RoUInverseTableCiphertext);
  for (int idx = 0; idx < ret.size(); idx++) {
    asmMulModQ(ret[idx].data(), nInvCiphertext.data(), ret[idx].data());
  }
  return ret;
}

vector<uint64_t> PAHEHelper::ftt_forward_plaintext(
   vector<uint64_t>& data) {
  vector<uint64_t> ret(phim);
  for (uint32_t i = 0; i<phim; i++) {
    asmMulModP(&ret[i], &data[i], &RoUTablePlaintext[i]);
  }

  return ntt_forward_plaintext(ret, RoUTablePlaintext);
}

vector<uint64_t> PAHEHelper::ftt_inverse_plaintext(
   vector<uint64_t>& data) {
  auto ret = ntt_forward_plaintext(data, RoUInverseTablePlaintext);

  for (uint32_t i = 0; i < phim; i++){
    asmMulModP(&ret[i], &ret[i], &scaledRoUInverseTablePlaintext[i]);
  }

  return ret;
}

vector<vector<uint64_t>> PAHEHelper::ftt_forward_ciphertext(
   vector<vector<uint64_t>>& data) {
  vector<vector<uint64_t>> ret(phim, vector<uint64_t>(3));
  for (uint32_t i = 0; i<phim; i++) {
    asmMulModQ(ret[i].data(), data[i].data(), RoUTableCiphertext[i].data());
  }

  return ntt_forward_ciphertext(ret, RoUTableCiphertext);
}

vector<vector<uint64_t>> PAHEHelper::ftt_inverse_ciphertext(
   vector<vector<uint64_t>>& data) {
  auto ret = ntt_forward_ciphertext(data, RoUInverseTableCiphertext);

  for (uint32_t i = 0; i < phim; i++){
    asmMulModQ(ret[i].data(), ret[i].data(), scaledInverseTableCiphertext[i].data());
  }

  return ret;
}

void PAHEHelper::ftt_precompute_plaintext() {
  //Precomputes twiddle factor omega and FTT parameter phi for Forward Transform
  uint64_t x = 1;
  vector<uint64_t> table(phim);
  for (int i = 0; i < phim; i++) {
    table[i] = x;
    asmMulModP(&x, &x, &RoUPlaintext);
  }
  RoUTablePlaintext = table;

  //Precomputes twiddle factor omega and FTT parameter phi for Inverse Transform
  x = 1;
  vector<uint64_t>  table_inverse(phim);
  for (int i = 0; i < phim; i++) {
    table_inverse[i] = x;
    asmMulModP(&x, &x, &RoUInvPlaintext);
  }
  RoUInverseTablePlaintext = table_inverse;

  vector<uint64_t>  table_scaled_inverse(phim);
  for (int i = 0; i < phim; i++) {
    asmMulModP(&table_scaled_inverse[i], &nInvPlaintext, &RoUInverseTablePlaintext[i]);
  }
  scaledRoUInverseTablePlaintext = table_scaled_inverse;

  vector<uint32_t>  reverse_id(phim);
  for (int i = 0; i < phim; i++){
    reverse_id[i] = ReverseBits(i, logn);
  }
  reverseTablePlaintext = reverse_id;
}

namespace {
NTL::ZZ U3ToZZ(vector<uint64_t> u) {
  NTL::ZZ ret = NTL::ZZ(0);
  ret += NTL::ZZFromBytes((unsigned char *)&u[2], 8);
  ret = (ret << 64);
  ret += NTL::ZZFromBytes((unsigned char *)&u[1], 8);
  ret = (ret << 64);
  ret += NTL::ZZFromBytes((unsigned char *)&u[0], 8);
  return ret;
}
}

void PAHEHelper::ftt_precompute_ciphertext() {
  //Precomputes twiddle factor omega and FTT parameter phi for Forward Transform
  vector<uint64_t> x(3, 0);
  x[0] = 1;
  vector<vector<uint64_t>> table(phim, vector<uint64_t>(3));
  for (int i = 0; i < phim; i++) {
    table[i] = x;
    auto y = RoUCiphertext;
    asmMulModQ(x.data(), y.data(), x.data());
  }
  RoUTableCiphertext = table;

  //Precomputes twiddle factor omega and FTT parameter phi for Inverse Transform
  x[0] = 1; x[1] = 0; x[2] = 0;
  vector<vector<uint64_t>>  table_inverse(phim, vector<uint64_t>(3));
  for (int i = 0; i < phim; i++) {
    table_inverse[i] = x;
    asmMulModQ(x.data(), RoUInvCiphertext.data(), x.data());
  }
  RoUInverseTableCiphertext = table_inverse;

  vector<vector<uint64_t>>  table_scaled_inverse(phim, vector<uint64_t>(3));
  for (int i = 0; i < phim; i++) {
    asmMulModQ(table_scaled_inverse[i].data(), nInvCiphertext.data(), RoUInverseTableCiphertext[i].data());
  }
  scaledInverseTableCiphertext = table_scaled_inverse;

  vector<uint32_t>  reverse_id(phim);
  for (int i = 0; i < phim; i++){
    reverse_id[i] = ReverseBits(i, logn);
  }
  reverseTableCiphertext = reverse_id;
}

vector<uint64_t> PAHEHelper::packed_encode(vector<uint64_t>& input) {
  return ftt_inverse_plaintext(input);
}

vector<uint64_t> PAHEHelper::packed_decode(vector<uint64_t>& input) {
  return ftt_forward_plaintext(input);
}

vector<vector<uint64_t>> PAHEHelper::ToCoeff(vector<vector<uint64_t>>& eval) {
  return ftt_inverse_ciphertext(eval);
}

vector<vector<uint64_t>> PAHEHelper::ToEval(vector<vector<uint64_t>>& coeff) {
  return ftt_forward_ciphertext(coeff);
}

vector<vector<uint64_t>> PAHEHelper::ToEval(vector<uint64_t>& pt) {
  vector<vector<uint64_t>> coeff(pt.size(), vector<uint64_t>(3, 0));
  for(int i = 0; i < pt.size(); i++) {
    coeff[i][0] = pt[i];
  }
  return ftt_forward_ciphertext(coeff);
}

vector<vector<uint64_t>> PAHEHelper::NullEncrypt(vector<uint64_t>& pt) {
  vector<vector<uint64_t>> temp(pt.size(), vector<uint64_t>(3));
  for (int i = 0; i < pt.size(); i++) {
    temp[i][0] = pt[i];
    temp[i][1] = 0;
    temp[i][2] = 0;
  }
  return ToEval(temp);
}

