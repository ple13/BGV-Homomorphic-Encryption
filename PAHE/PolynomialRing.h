#ifndef _POLYNOMIAL_RING_H__
#define _POLYNOMIAL_RING_H__

#include <vector>
#include <NTL/ZZ.h>

using namespace std;

vector<NTL::ZZ> MulPolynomialRing(const vector<NTL::ZZ>& a, const vector<NTL::ZZ>& b, NTL::ZZ modulus);

#endif