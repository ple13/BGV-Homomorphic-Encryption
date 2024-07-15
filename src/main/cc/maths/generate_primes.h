// Copyright 2024 Phi Hung Le
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef _SRC_MAIN_CC_MATHS_GENERATE_PRIMES_H_
#define _SRC_MAIN_CC_MATHS_GENERATE_PRIMES_H_

#include <NTL/ZZ.h>

namespace maths {

NTL::ZZ genPrime(int logn, int bitlength);
NTL::ZZ genMaxPrime(int logn, int bitlength);

}   // namespace maths

#endif  // _SRC_MAIN_CC_MATHS_GENERATE_PRIMES_H_
