#include "RandomGenerator.h"

#include <NTL/RR.h>
#include <NTL/ZZ.h>
#include <openssl/rand.h>

#include <unordered_map>

#include "../crypto/prng.h"

// Ciphertext sub modulo
extern "C" void asmSubModQ(uint64_t *Z, uint64_t *X, uint64_t *Y);

namespace {
void ZZToU3(NTL::ZZ val, uint64_t *u) {
  NTL::ZZ temp;
  NTL::ZZ one = NTL::ZZ(1);
  temp = val % (one << 64);
  NTL::BytesFromZZ((unsigned char *)&u[0], val, 8);
  val = val >> 64;
  temp = val % (one << 64);
  NTL::BytesFromZZ((unsigned char *)&u[1], val, 8);
  val = val >> 64;
  temp = val % (one << 64);
  NTL::BytesFromZZ((unsigned char *)&u[2], val, 8);
}
}  // namespace

// We find the range [-max; max] such that
// Pr[-max <= u <= max] | u <-- N(0, sigma^2)] > 1 - delta
NoiseGenerator::NoiseGenerator(double sigma, double delta) {
  sigma_ = sigma;

  // See Theorem A.1 in
  // https://www.cis.upenn.edu/~aaroth/Papers/privacybook.pdf.
  int max = (int)ceil(sigma_ * sqrt(2 * log(1.25 / delta)));

  for (int i = 0; i <= max; i++) {
    std::vector<uint64_t> val(3, 0);
    val[0] = i;
    lookUpTable_[i] = val;
  }

  for (int i = -1; i >= -max; i--) {
    std::vector<uint64_t> val(3, 0);
    asmSubModQ(val.data(), lookUpTable_[0].data(), lookUpTable_[-i].data());
    lookUpTable_[i] = val;
  }

  // Computes the sum = \sum_{i=-max}^max exp(-i^2/[2*sigma^2]).
  double variance = sigma_ * sigma_;

  // The probability density function: pdf[i] ~ exp(-i^2/[2*sigma^2]).
  // We omit the term 1/[sqrt(2*pi)*sigma].
  unordered_map<int, double> pdf;
  double sum = 1.0;
  pdf[0] = 1.0;
  for (int i = 1; i <= max; i++) {
    pdf[i] = exp(-i * i / (2.0 * variance));
    pdf[-i] = pdf[i];
    sum = sum + 2 * pdf[i];
  }

  double invert_sum = 1.0 / sum;

  for (int i = -max; i <= max; i++) {
    auto temp = invert_sum * pdf[i];
    cummulative_density_probability_.push_back(temp);
  }

  for (int i = 1; i < cummulative_density_probability_.size(); i++) {
    cummulative_density_probability_[i] +=
        cummulative_density_probability_[i - 1];
  }

  NTL::RR::SetPrecision(110);
  PI_ = NTL::ComputePi_RR();
}

int NoiseGenerator::FindInVector(std::vector<double> &S, double search) {
  // STL binary search implementation
  auto lower = std::lower_bound(S.begin(), S.end(), search);
  if (lower != S.end()) {
    return lower - S.begin();
  } else {
    return 0;
  }
}

vector<vector<uint64_t>> NoiseGenerator::GenerateGaussianVector(int size) {
  vector<vector<uint64_t>> ret(size);
  vector<uint64_t> randomness(size);
  RAND_bytes((unsigned char *)randomness.data(), sizeof(uint64_t) * size);

  double max_uint_64 = pow(2.0, 64) - 1.0;
  int mid = lookUpTable_.size() / 2;
  for (int i = 0; i < size; i++) {
    double val = (double)(randomness[i]) / max_uint_64;
    auto lower = lower_bound(cummulative_density_probability_.begin(),
                             cummulative_density_probability_.end(), val);
    int index = 0;
    if (lower != cummulative_density_probability_.end()) {
      index = lower - cummulative_density_probability_.begin();
    }
    ret[i] = lookUpTable_[index - mid];
  }
  return ret;
}

vector<vector<uint64_t>> NoiseGenerator::GenerateFloodingNoiseVector(int size) {
  vector<vector<uint64_t>> ret(size, std::vector<uint64_t>(3));
  for (int i = 0; i < size; i++) {
    ret[i].resize(3, 0);
    RAND_bytes((unsigned char *)ret[i].data(), 128 / 8);
    ret[i][1] >>= 16;
  }
  return ret;

  //   NTL::RR::SetPrecision(110);
  //   NTL::RR scale;
  //   NTL::MakeRR(scale, NTL::ZZ(1), 110);
  //   vector<vector<uint64_t>> ret(size, vector<uint64_t>(3));
  //   for (int i = 0; i < size; i++) {
  //     NTL::RR u = -2*NTL::log(NTL::random_RR());
  //     NTL::RR sqr_u = NTL::SqrRoot(u);
  //     NTL::RR v = sin(2*PI*NTL::random_RR());
  //     NTL::ZZ val = NTL::RoundToZZ(sqr_u*v*scale);
  //     ZZToU3(val, ret[i].data());
  //   }
  //   return ret;
}

vector<vector<uint64_t>> NoiseGenerator::GenerateUniformVector(int size) {
  vector<vector<uint64_t>> ret(size, std::vector<uint64_t>(3));
  for (int i = 0; i < size; i++) {
    ret[i].resize(3);
    RAND_bytes((unsigned char *)ret[i].data(),
               sizeof(uint64_t) * ret[i].size());
  }
  return ret;
}

vector<vector<uint64_t>> NoiseGenerator::GenerateUniformVector(
    std::vector<unsigned char> seed, int size) {
  std::vector<unsigned char> IV(seed.data() + 16, seed.data() + seed.size());
  std::vector<unsigned char> s(seed.data(), seed.data() + seed.size());

  PRNG *prng = new PRNG((const unsigned char *)s.data(),
                        (const unsigned char *)IV.data());
  std::vector<unsigned char> str(size * sizeof(uint64_t) * 3, 0);
  prng->sampleBytes(str, str.size());

  vector<vector<uint64_t>> ret(size, std::vector<uint64_t>(3));
  uint64_t *ptr = (uint64_t *)str.data();
  for (int i = 0; i < size; i++) {
    ret[i].resize(3);
    ret[i][0] = *ptr;
    ptr++;
    ret[i][1] = *ptr;
    ptr++;
    ret[i][2] = *ptr;
    ptr++;
  }

  delete prng;

  return ret;
}

vector<vector<uint64_t>> NoiseGenerator::GenerateBinaryVector(int size) {
  vector<vector<uint64_t>> ret(size);
  vector<unsigned char> randomness(size);
  RAND_bytes(randomness.data(), sizeof(char) * size);
  for (int i = 0; i < size; i++) {
    ret[i] = lookUpTable_[randomness[i] % 2];
  }
  return ret;
}

vector<vector<uint64_t>> NoiseGenerator::GenerateZOVector(int size) {
  vector<vector<uint64_t>> ret(size);
  vector<unsigned char> randomness(size);
  RAND_bytes(randomness.data(), sizeof(char) * size);
  for (int i = 0; i < size; i++) {
    int val = randomness[i] % 4;
    if (val == 0) {
      ret[i] = lookUpTable_[1];
    } else if (val == 1) {
      ret[i] = lookUpTable_[-1];
    } else {
      ret[i] = lookUpTable_[0];
    }
  }

  return ret;
}
