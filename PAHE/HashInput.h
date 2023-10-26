#ifndef _HASH_INPUT__
#define _HASH_INPUT__

#include <iostream>
#include <iomanip>
#include <functional>
#include <string>
#include <vector>
#include <cassert>
#include <NTL/ZZ.h>

using namespace std;


void padInput(vector<vector<NTL::ZZ>>& input, int maxSize);
vector<vector<NTL::ZZ>> hashInput(const vector<uint64_t>& input, int nBuckets, const string seed);
vector<vector<NTL::ZZ>> hashInput(const vector<uint64_t>& input, int nBuckets, int maxSize, const string seed);
vector<vector<NTL::ZZ>> hashInput(const vector<uint64_t>& input, int nBuckets, const string seed, int& maxiSize);
vector<vector<NTL::ZZ>> hashInput(const vector<uint64_t>& input, int nBuckets, const string seed, NTL::ZZ plaintext_modulus);

#endif