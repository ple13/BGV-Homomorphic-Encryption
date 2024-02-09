#ifndef __UTILS_H__
#define __UTILS_H__

#include <cstdint>
#include <ostream>

std::ostream& operator<<(std::ostream& dest, __int128_t value);

// p1 = 2^62 + 106497
__int128_t mod_slow_p1(__int128_t x);

// p2 = 2^62 + 622593
__int128_t mod_slow_p2(__int128_t x);

__int128_t mod_slow_p0(__int128_t x);

#endif  // __UTILS_H__