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

#include "maths/polynomial_interpolation.h"

#include "absl/status/status.h"
#include "absl/status/statusor.h"
#include "common_cpp/testing/status_macros.h"
#include "common_cpp/testing/status_matchers.h"
#include "gtest/gtest.h"

namespace maths {
namespace {

using ::wfa::StatusIs;

TEST(MulMod, InvalidModulusFails) {
  auto res = maths::mulMod(/*x=*/1, /*y=*/2, /*modulus=*/1);
  EXPECT_THAT(res.status(), StatusIs(absl::StatusCode::kInvalidArgument,
                                     "Modulus must be greater than 1"));
}

TEST(MulMod, MultiplicandNotLessThanModulusFails) {
  auto res = maths::mulMod(/*x=*/4, /*y=*/2, /*modulus=*/4);
  EXPECT_THAT(res.status(),
              StatusIs(absl::StatusCode::kInvalidArgument,
                       "The multiplicand x must be less than modulus."));
}

TEST(MulMod, MultiplierNotLessThanModulusFails) {
  auto res = maths::mulMod(/*x=*/4, /*y=*/6, /*modulus=*/5);
  EXPECT_THAT(res.status(),
              StatusIs(absl::StatusCode::kInvalidArgument,
                       "The multiplier y must be less than modulus."));
}

TEST(MulMod, BothMultiplierMultiplicandNotLessThanModulusFails) {
  auto res = maths::mulMod(/*x=*/4, /*y=*/6, /*modulus=*/3);
  EXPECT_THAT(
      res.status(),
      StatusIs(absl::StatusCode::kInvalidArgument,
               "Both multiplicand and multiplier must be less than modulus."));
}

TEST(MulMod, ValidInputSucceeds) {
  ASSERT_OK_AND_ASSIGN(uint64_t ret,
                       maths::mulMod(/*x=*/4, /*y=*/6, /*modulus=*/11));
  ASSERT_EQ(ret, 2);
  ASSERT_OK_AND_ASSIGN(ret, maths::mulMod(/*x=*/4, /*y=*/0, /*modulus=*/11));
  ASSERT_EQ(ret, 0);
}

}  // namespace
}  // namespace maths
