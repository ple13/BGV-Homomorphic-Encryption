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

#ifndef PRNG_H__
#define PRNG_H__

#include <openssl/evp.h>
#include <openssl/rand.h>

#include <cstring>
#include <iostream>
#include <vector>

#define SEED_SIZE 32
#define IV_SIZE 16

class PRNG {
 public:
  PRNG() {
    std::vector<unsigned char> seed(SEED_SIZE);
    std::vector<unsigned char> IV(IV_SIZE);

    RAND_bytes(seed.data(), seed.size());
    RAND_bytes(IV.data(), IV.size());

    ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL) {
      std::cerr << "Error creating encryption context.\n";
      return;
    }

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_ctr(), NULL,
                           (const unsigned char *)seed.data(),
                           (const unsigned char *)IV.data()) != 1) {
      std::cerr << "Error initializing encryption operation.\n";
      return;
    };
  }

  PRNG(const unsigned char *seed, const unsigned char *IV) {
    this->seed =
        std::vector<unsigned char>(seed, seed + strlen((const char *)seed));
    this->IV = std::vector<unsigned char>(IV, IV + strlen((const char *)IV));

    ctx = EVP_CIPHER_CTX_new();
    if (ctx == NULL) {
      std::cerr << "Error creating encryption context.\n";
      return;
    }

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_ctr(), NULL, seed, IV) != 1) {
      std::cerr << "Error initializing encryption operation.\n";
      return;
    }
  }

  void sampleBytes(std::vector<unsigned char> &dst, int size) {
    std::vector<unsigned char> temp(size, 0);
    dst.resize(size);
    int length;
    if (EVP_EncryptUpdate(ctx, dst.data(), &length, temp.data(), temp.size()) !=
        1) {
      std::cerr << "Error updating encryption context.\n";
      return;
    }
    if (EVP_EncryptFinal_ex(ctx, dst.data() + length, &length) != 1) {
      std::cerr << "Error finalizing encryption operation.\n";
      return;
    }
  }

 private:
  EVP_CIPHER_CTX *ctx;
  std::vector<unsigned char> seed;
  std::vector<unsigned char> IV;
};

#endif
