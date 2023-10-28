#include "RandomGenerator.h"
#include <openssl/rand.h>

// Ciphertext sub modulo
extern "C" void asmSubModQ(uint64_t* Z, uint64_t * X, uint64_t * Y);

NoiseGenerator::NoiseGenerator(double std) {
  m_std = std;
  
  m_vals.clear();

  //weightDiscreteGaussian
  double acc = 1e-15;
  double variance = m_std * m_std;

  int max_val = (int)ceil(m_std * sqrt(-2 * log(acc)));

  for(int i = 0; i <= max_val; i++) {
    std::vector<uint64_t> val(3, 0);
    val[0] = i;
    lookUpTable[i] = val;
  }

  for(int i = -1; i >= -max_val; i--) {
    std::vector<uint64_t> val(3, 0);
    asmSubModQ(val.data(), lookUpTable[0].data(), lookUpTable[-i].data());
    lookUpTable[i] = val;
  }

  double sum = 1.0;

  for (int x = 1; x <= max_val; x++) {
    sum = sum + 2 * exp(-x * x / (variance * 2));
  }

  m_a = 1 / sum;

  double temp;

  for (int i = 1; i <= max_val; i++) {
    temp = m_a * exp(-((double)(i * i) / (2 * variance)));
    m_vals.push_back(temp);
  }

  for (int i = 1; i < m_vals.size(); i++) {
    m_vals[i] += m_vals[i - 1];
  }
}

int NoiseGenerator::FindInVector(
   std::vector<double> &S, double search) {
  //STL binary search implementation
  auto lower = std::lower_bound(S.begin(), S.end(), search);
  if (lower != S.end())
    return lower - S.begin();
  else
    return 0;
}

vector<vector<uint64_t>> NoiseGenerator::GenerateGaussianVector(int size) {
  vector<vector<uint64_t>> ret(size);
  vector<int> randomness(size);
  RAND_bytes((unsigned char *)randomness.data(), sizeof(int)*size);
  
  double max_uint_32 = pow(2.0, 32) - 1.0;
  
  for (int i = 0; i < size; i++) {
    double val = (double)(randomness[i])/max_uint_32 - 0.5;
    if (std::abs(val) <= m_a / 2) {
      ret[i] = lookUpTable[0];
    } else {
      int index = FindInVector(m_vals, (std::abs(val) - m_a / 2));
      if (val > 0) {
        ret[i] = lookUpTable[index+1];
      } else {
        ret[i] = lookUpTable[-(index+1)];
      }
    }
  }
  return ret;
}

vector<vector<uint64_t>> NoiseGenerator::GenerateUniformVector(int size) {
  vector<vector<uint64_t>> ret(size, std::vector<uint64_t>(3));
  for (int i = 0; i < size; i++) {
    ret[i].resize(3);
    RAND_bytes((unsigned char *)ret[i].data(), sizeof(uint64_t)*ret[i].size());
  }
  return ret;
}

vector<vector<uint64_t>> NoiseGenerator::GenerateBinaryVector (int size)  {
  vector<vector<uint64_t>> ret(size);
  vector<unsigned char> randomness(size);
  RAND_bytes(randomness.data(), sizeof(char)*size);
  for (int i = 0; i < size; i++) {
    ret[i] = lookUpTable[randomness[i] % 2];
  }
  return ret;
}

vector<vector<uint64_t>> NoiseGenerator::GenerateZOVector(int size) {
  vector<vector<uint64_t>> ret(size);
  vector<unsigned char> randomness(size);
  RAND_bytes(randomness.data(), sizeof(char)*size);
  for (int i = 0; i < size; i++) {
    int val = randomness[i] % 4;
    if (val == 0) {
      ret[i] = lookUpTable[1];
    } else if (val == 1) {
      ret[i] = lookUpTable[-1];
    } else {
      ret[i] = lookUpTable[0];
    }
  }

  return ret;
}
