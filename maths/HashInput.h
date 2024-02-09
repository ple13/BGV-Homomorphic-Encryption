#ifndef _HASH_INPUT__
#define _HASH_INPUT__

#include <NTL/ZZ.h>

#include <cassert>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

map<int, vector<uint64_t>> hashInput(const vector<uint64_t>& input,
                                     int nBuckets, const string seed);

#endif
