#ifndef __PAHE_HELPER_H__
#define __PAHE_HELPER_H__

#include <cstdint>
#include <map>
#include <vector>

// Ciphertext mul modulo
extern "C" void asmMulModQ(uint64_t* Z, uint64_t* X, uint64_t* Y);

// Ciphertext add modulo
extern "C" void asmAddModQ(uint64_t* Z, uint64_t* X, uint64_t* Y);

// Ciphertext sub modulo
extern "C" void asmSubModQ(uint64_t* Z, uint64_t* X, uint64_t* Y);

// Plaintext mul modulo
extern "C" void asmMulModP(uint64_t* Z, uint64_t* X, uint64_t* Y);

// Ciphertext add modulo
extern "C" void asmAddModP(uint64_t* Z, uint64_t* X, uint64_t* Y);

// Ciphertext sub modulo
extern "C" void asmSubModP(uint64_t* Z, uint64_t* X, uint64_t* Y);

extern "C" void asmMulWithPModQ(uint64_t* Z, uint64_t* X);

extern "C" void asmModPInQ(uint64_t* Z, uint64_t* X);

extern "C" void asmCorrect(uint64_t* X);

using namespace std;

inline uint64_t ones(int n) { return ((uint64_t)1 << n) - 1; }

// Only works for positive num and den. (num cannot be zero)
inline uint32_t div_ceil(uint32_t num, uint32_t den) {
  return 1 + ((num - 1) / den);
}

/**
 * Method to reverse bits of num and return an unsigned int, for all bits up to
 * an including the designated most significant bit.
 *
 * @param input an unsigned int
 * @param msb the most significant bit.  All larger bits are disregarded.
 *
 * @return an unsigned integer that represents the reversed bits.
 */
uint32_t ReverseBits(uint32_t input, uint32_t msb);

/**
 * Get MSB of an unsigned 64 bit integer.
 *
 * @param x the input to find MSB of.
 *
 * @return the index of the MSB bit location.
 */
inline uint32_t GetMSB64(uint64_t x) {
  if (x == 0) return 0;

    // hardware instructions for finding MSB are used are used;
#if defined(_MSC_VER)
  // a wrapper for VC++
  unsigned long msb;
  _BitScanReverse64(&msb, x);
  return msb + 1;
#else
  // a wrapper for GCC
  return 64 -
         (sizeof(unsigned long) == 8 ? __builtin_clzl(x) : __builtin_clzll(x));
#endif
}

uint32_t lopow2(uint32_t v);
uint32_t nxt_pow2(uint32_t x);
uint32_t num_ones(uint32_t x);

class PAHEHelper {
 public:
  PAHEHelper() {
    n = 4096;
    logn = 12;
    phim = n;

    plaintextModulus = 18446744073709436929ULL;
    RoUPlaintext = 7972528404776578548ULL;

    RoUCiphertext.resize(3);
    RoUCiphertext[0] = 10904640868020604749ULL;
    RoUCiphertext[1] = 4064029909171694920ULL;
    RoUCiphertext[2] = 14460608192583649110ULL;

    nInvPlaintext = 18442240474082066461ULL;

    nInvCiphertext.resize(3);
    nInvCiphertext[0] = 18446744073708617957ULL;
    nInvCiphertext[1] = 18446744073709551615ULL;
    nInvCiphertext[2] = 18442240474082181119ULL;

    RoUInvPlaintext = 441056466821571575ULL;

    RoUInvCiphertext.resize(3);
    RoUInvCiphertext[0] = 1757593741805203102ULL;
    RoUInvCiphertext[1] = 14853335128251300877ULL;
    RoUInvCiphertext[2] = 5141496555031112569ULL;
  }

  vector<uint64_t> ntt_forward_plaintext(vector<uint64_t>& data,
                                         vector<uint64_t> RoUTable);
  vector<uint64_t> ntt_inverse_plaintext(vector<uint64_t>& data);
  vector<uint64_t> ftt_forward_plaintext(vector<uint64_t>& data);
  vector<uint64_t> ftt_inverse_plaintext(vector<uint64_t>& data);

  vector<vector<uint64_t>> ntt_forward_ciphertext(
      vector<vector<uint64_t>>& data, std::vector<vector<uint64_t>> RoUTable);
  vector<vector<uint64_t>> ntt_inverse_ciphertext(
      vector<vector<uint64_t>>& data);
  vector<vector<uint64_t>> ftt_forward_ciphertext(
      vector<vector<uint64_t>>& data);
  vector<vector<uint64_t>> ftt_inverse_ciphertext(
      vector<vector<uint64_t>>& data);

  void ftt_precompute_plaintext();
  void ftt_precompute_ciphertext();

  vector<uint64_t> packed_encode(vector<uint64_t>& input);
  vector<uint64_t> packed_decode(vector<uint64_t>& input);

  vector<vector<uint64_t>> ToCoeff(vector<vector<uint64_t>>& eval);
  vector<vector<uint64_t>> ToEval(vector<uint64_t>& pt);
  vector<vector<uint64_t>> ToEval(vector<vector<uint64_t>>& coeff);
  vector<vector<uint64_t>> NullEncrypt(vector<uint64_t>& pt);

  bool parallelized;
  int n;
  int logn;
  int phim;

  uint64_t plaintextModulus;
  uint64_t RoUPlaintext;
  uint64_t nInvPlaintext;
  uint64_t RoUInvPlaintext;

  vector<uint64_t> ciphertextModulus;
  vector<uint64_t> RoUCiphertext;
  vector<uint64_t> nInvCiphertext;
  vector<uint64_t> RoUInvCiphertext;

  vector<uint64_t> RoUTablePlaintext;
  vector<uint64_t> RoUInverseTablePlaintext;
  vector<uint64_t> scaledRoUInverseTablePlaintext;
  vector<uint32_t> reverseTablePlaintext;

  vector<vector<uint64_t>> RoUTableCiphertext;
  vector<vector<uint64_t>> RoUInverseTableCiphertext;
  vector<vector<uint64_t>> scaledInverseTableCiphertext;
  vector<uint32_t> reverseTableCiphertext;
};

#endif
