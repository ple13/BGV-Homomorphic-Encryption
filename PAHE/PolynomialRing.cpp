#include "PolynomialRing.h"

vector<ZZ> MulPolynomialRing(const vector<ZZ>& a, const vector<ZZ>& b, ZZ modulus) {
  int n = a.size();
  vector<ZZ> output(n);
  
  for (int idx = 0; idx < n; idx++) {
    ZZ temp = ZZ(0);
    for (int kdx = 0; kdx <= idx; kdx++) {
      AddMod(temp, temp, MulMod(a[kdx], b[idx-kdx], modulus), modulus);
    }
    
    for (int kdx = idx + 1; kdx < n; kdx++) {
      SubMod(temp, temp, MulMod(a[kdx], b[n - kdx + idx], modulus), modulus);
    }
    output[idx] = temp;
  }
  return output;
}