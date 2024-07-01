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

#ifndef _SRC_MAIN_CC_MATHS_POLYNOMIAL_INTERPOLATION_H_
#define _SRC_MAIN_CC_MATHS_POLYNOMIAL_INTERPOLATION_H_

#include <iostream>
#include <vector>

#include "absl/status/status.h"
#include "absl/status/statusor.h"

namespace maths {

absl::StatusOr<uint64_t> mulMod(uint64_t x, uint64_t y, uint64_t modulus);

std::vector<uint64_t> interpolate(std::vector<uint64_t> roots);
uint64_t invModP(uint64_t val);

}  // namespace maths

#endif  // _SRC_MAIN_CC_MATHS_POLYNOMIAL_INTERPOLATION_H_
