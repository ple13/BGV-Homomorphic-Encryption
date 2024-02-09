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

#ifndef _RANDOM_GENERATOR_H__
#define _RANDOM_GENERATOR_H__

#include <NTL/RR.h>
#include <math.h>

#include <map>
#include <memory>
#include <random>
#include <vector>

using namespace std;

class NoiseGenerator {
 public:
  // Generate the look up table for the Discrete Gaussian noise with
  // the statistical security parameter delta.
  NoiseGenerator(double sigma, double delta = 1e-15);

  vector<vector<uint64_t>> GenerateGaussianVector(int size);
  vector<vector<uint64_t>> GenerateFloodingNoiseVector(int size);
  vector<vector<uint64_t>> GenerateUniformVector(int size);
  vector<vector<uint64_t>> GenerateUniformVector(vector<unsigned char> seed,
                                                 int size);
  vector<vector<uint64_t>> GenerateBinaryVector(int size);
  vector<vector<uint64_t>> GenerateZOVector(int size);

 private:
  // Defines the discrete Gaussian distribution N(0, sigma^2).
  double sigma_;

  // Stores the CDF of the discrete Gaussian distribution.
  std::vector<double> cummulative_density_probability_;

  // Lookup table to map uniformly random value to the discrete Gaussian
  // distribution.
  map<int, std::vector<uint64_t>> lookUpTable_;

  NTL::RR PI_;
};

#endif
