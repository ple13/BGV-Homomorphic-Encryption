#ifndef _HASH_INPUT__
#define _HASH_INPUT__

#include <iostream>
#include <iomanip>
#include <functional>
#include <string>
#include <map>
#include <vector>
#include <cassert>
#include <NTL/ZZ.h>

using namespace std;

map<int, vector<uint64_t>> hashInput(const vector<uint64_t>& input, int nBuckets, const string seed);

#endif
