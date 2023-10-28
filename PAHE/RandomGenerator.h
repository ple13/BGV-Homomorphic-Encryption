#ifndef _RANDOM_GENERATOR_H__
#define _RANDOM_GENERATOR_H__

#include <map>
#include <vector>
#include <math.h>
#include <random>
#include <memory>

using namespace std;

class NoiseGenerator {
public:
  /**
  * @brief         Basic ructor for specifying distribution parameter and modulus.
  * @param modulus The modulus to use to generate discrete values.
  * @param std     The standard deviation for this Gaussian Distribution.
  */
  NoiseGenerator(double std);
  
  /**
  * @brief           Generates a vector of random values within this Discrete Gaussian Distribution. Uses Peikert's inversion method.
  *
  * @param  size     The number of values to return.
  * @param  modulus  modulus of the polynomial ring.
  * @return          The vector of values within this Discrete Gaussian Distribution.
  */
  vector<vector<uint64_t>> GenerateGaussianVector(int size);
  vector<vector<uint64_t>> GenerateUniformVector(int size);
  vector<vector<uint64_t>> GenerateBinaryVector(int size);
  vector<vector<uint64_t>> GenerateZOVector(int size);
private:
  int FindInVector ( std::vector<double> &S, double search) ;

  // Gyana to add precomputation methods and data members
  // all parameters are set as int because it is assumed that they are used for generating "small" polynomials only
  double m_a;

  std::vector<double> m_vals;
  map<int, std::vector<uint64_t>> lookUpTable;
  /**
  * The standard deviation of the distribution.
  */
  double m_std;
};

#endif
