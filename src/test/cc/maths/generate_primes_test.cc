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

#include "maths/generate_primes.h"

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "common_cpp/testing/status_macros.h"
#include "common_cpp/testing/status_matchers.h"
#include "gtest/gtest.h"

namespace maths {
namespace {

TEST(GeneratePrime, Succeeds) {
  auto p = maths::genMaxPrime(12, 64);

  EXPECT_LE(p, NTL::ZZ(1) << 64);
  auto q = (p - 1) >> 12;
  EXPECT_EQ((q << 12), p - 1);
}

}  // namespace
}  // namespace maths
