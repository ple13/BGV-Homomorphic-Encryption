#include "HashInput.h"
#include <NTL/ZZ.h>

using namespace std;

map<int, vector<uint64_t>> hashInput(const vector<uint64_t>& input, int nBuckets, const string seed) {
  hash<string> ptr_hash;
  map<int, vector<uint64_t>> ret;
  for (auto x : input) {
    auto str = seed + to_string(x);
    size_t index = ptr_hash(str);
    index %= nBuckets;
    ret[index].push_back(x);
  }
  
  return ret;
}
