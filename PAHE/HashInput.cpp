#include "HashInput.h"

using namespace std;


void padInput(vector<vector<NTL::ZZ>>& input, int maxSize) {
  NTL::ZZ upper_bound = NTL::ZZ(1) << 64;
  for (int idx = 0; idx < input.size(); idx++) {
    int current_size = input[idx].size();
    assert(maxSize >= current_size);
    for (int pdx = 0; pdx < maxSize - current_size; pdx++) {
      input[idx].push_back(RandomBnd(upper_bound));
    }
    assert(input[idx].size() == maxSize);
  }
}

vector<vector<NTL::ZZ>> hashInput(const vector<uint64_t>& input, int nBuckets, const string seed) {
  hash<string> ptr_hash;
  vector<vector<NTL::ZZ>> ret(nBuckets);
  for (auto x : input) {
    auto str = seed + to_string(x);
    size_t index = ptr_hash(str);
    index %= nBuckets;
    ret[index].push_back(NTL::ZZ(x));
  }
  
  int maxSize = 0;
  for (auto x : ret) {
    if (maxSize < x.size()) {
      maxSize = x.size();
    }
  }
  
  cout << maxSize << endl;
  
  return ret;
}

vector<vector<NTL::ZZ>> hashInput(const vector<uint64_t>& input, int nBuckets, const string seed, int& maxSize) {
  hash<string> ptr_hash;
  vector<vector<NTL::ZZ>> ret(nBuckets);
  for (auto x : input) {
    auto str = seed + to_string(x);
    size_t index = ptr_hash(str);
    index %= nBuckets;
    ret[index].push_back(NTL::ZZ(x));
  }
  
  maxSize = 0;
  for (auto x : ret) {
    if (maxSize < x.size()) {
      maxSize = x.size();
    }
  }
  
//   cout << "Max bucket size: " << maxSize << endl;
  
  return ret;
}

vector<vector<NTL::ZZ>> hashInput(const vector<uint64_t>& input, int nBuckets, const string seed, NTL::ZZ plaintext_modulus) {
  hash<string> ptr_hash;
  vector<vector<NTL::ZZ>> ret(nBuckets);
  for (auto x : input) {
    auto str = seed + to_string(x);
    size_t index = ptr_hash(str);
    index %= nBuckets;
    ret[index].push_back(NTL::ZZ(x) % plaintext_modulus);
  }
  
  int maxSize = 0;
  for (auto x : ret) {
    if (maxSize < x.size()) {
      maxSize = x.size();
    }
  }
  
  cout << maxSize << endl;
  
  return ret;
}

vector<vector<NTL::ZZ>> hashInput(const vector<uint64_t>& input, int nBuckets, int maxSize, const string seed) {
  hash<string> ptr_hash;
  vector<vector<NTL::ZZ>> ret(nBuckets);
  for (auto x : input) {
    auto str = seed + to_string(x);
    size_t index = ptr_hash(str);
    index %= nBuckets;
    ret[index].push_back(NTL::ZZ(x));
  }
  padInput(ret, maxSize);
  
  return ret;
}