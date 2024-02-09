#include <cassert>
#include <cstdint>
#include <array>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <random>
#include <NTL/ZZ.h>
#include "testUtils.h"
#include "immintrin.h"
#include "../utils/Timer.h"

void ZZToU3(ZZ val, uint64_t *u);
ZZ U3ToZZ(vector<uint64_t> u);
void printZZ(ZZ val);
void printU3(vector<uint64_t> val);
std::ostream&
operator<<( std::ostream& dest, __int128_t value );
