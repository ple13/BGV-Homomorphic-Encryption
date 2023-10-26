#include <iostream>
#include <vector>
#include <numeric>

#include "../maths/utils.h"

using namespace std;

int main() {
  vector<uint64_t> roots(4);
  iota(roots.begin(), roots.end(), 0);

  auto p = interpolate(roots);

  for (int i = 0; i < p.size(); i++) {
    cout << p[i] << "x^" << i << " + ";
  }
  cout << endl;
  return 0;
}
