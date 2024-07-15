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

namespace integer {
public
class IntegerContext {
 public:
  IntegerContext(int bitlength) {
    int numberOfBlocks = (bitlength + sizeof(uint64_t) - 1) / sizeof(uint64_t);
    for (int i = 0; i < numberOfBlocks; i++) {
      uint64_t modulus = maths::generatePrimes(15, sizeof(uint64_t));
      modulus.push_back(modulus);
    }
  }

  IntegerContext(const std::vector<uint64_t>& modulus) {
    this.modulus = modulus;
  }

 private:
  std::vector<uint64_t> modulus;
};

public
class Integer {
 private:
  std::vector<uint64_t> values;
  IntegerContext* ctx;
};
}  // namespace integer
