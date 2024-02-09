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

#ifndef PSU_H__
#define PSU_H__

#pragma once
#include <NTL/ZZ.h>
#include <emmintrin.h>
#include <math.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <future>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include "../PAHE/PAHE.h"
#include "../PAHE/RandomGenerator.h"
#include "../maths/HashInput.h"
#include "../maths/utils.h"
#include "../network/Machine.hpp"

using namespace std;
using namespace NTL;

void printPlaintext(vector<uint64_t> pt) {
  cout << "Plaintext: " << endl;
  for (int i = 0; i < 10; i++) {
    cout << pt[i] << " ";
  }
  cout << endl;
}

void printCiphertext(Ciphertext ct) {
  cout << "Ciphertext: " << endl;
  for (int i = 0; i < 10; i++) {
    cout << ct.a[i][0] << " ";
  }
  cout << endl;
  for (int i = 0; i < 10; i++) {
    cout << ct.b[i][0] << " ";
  }
  cout << endl;
}

class PSU {
 public:
  int machineId;
  int party;
  int partner;
  int totalItems;
  int totalParties;
  float rate;

  int nBuckets;
  int max_size;
  int use_ot;

  BGV bgv;

  Machine *machine;

  PSU(int machineId, int party, int totalParties, int totalItems, int nBuckets,
      int max_size, int use_ot, Machine *machine, float rate) {
    this->totalParties = totalParties;
    this->nBuckets = nBuckets;
    this->max_size = max_size;
    this->use_ot = use_ot;
    this->machineId = machineId;
    this->party = party;
    this->totalItems = totalItems;
    this->machine = machine;
    this->rate = rate;
    bgv.init();
    bgv.keygen();
  }

  ~PSU() { delete machine; }

  void PSUComputation(int test) {
    int nItems = totalItems;
    Timer t;

    PSU_AHE(party, nItems, nBuckets, max_size, machine, use_ot);
    if (party == 0)
      cout << "Communication (MBytes): "
           << machine->num_bytes_send / 1024.0 / 1024.0 << "\t"
           << machine->num_bytes_recv / 1024.0 / 1024.0 << "\t"
           << (machine->num_bytes_send + machine->num_bytes_recv) / 1024.0 /
                  1024.0
           << endl;
    t.Tick("PSU************************************");
  }

  int PSU_AHE(int party, int nItems, int nBuckets, int max_size,
              Machine *machine, int use_ot) {
    vector<uint64_t> input;

    int n = bgv.n;
    int nrows = nBuckets / n;
    int ncols = max_size;
    int other_party = 1 - party;

    Timer t;

    if (party == 0) {
      machine->sendToParty(other_party, bgv.pk);

      for (int i = 0; i < nItems; i++) {
        input.push_back(i + 1);
      }
      int maxSize = 0;
      auto buckets = hashInput(input, nBuckets, "0xasdflafdlaf");
      // Padding to max size.
      // P0 also add 1 zeros at the end and shuffle the items in each bucket.
      // P1 just need to pad to max size.
      if (maxSize < max_size) {
        maxSize = max_size;
      }

      cout << "(nrows, ncols, max bin): " << nrows << " " << ncols << " "
           << maxSize << endl;

      // Convert (X - x_i1)...(X - x_ik) to a_i0 + a_i1*X + ... + a_ik*X^k
      map<int, vector<uint64_t>> polynomials;
      for (int r = 0; r < nBuckets; r++) {
        polynomials[r] = interpolate(buckets[r]);
        polynomials[r].resize(maxSize, 0);
      }

      t.Tick("Interpolation");

      vector<CompactedCiphertext> ciphertexts(nrows * ncols);

      vector<uint64_t> pt(n);
      for (int r = 0; r < nrows; r++) {
        for (int c = 0; c < ncols; c++) {
          for (int i = 0; i < n; i++) {
            pt[i] = polynomials[r * n + i][c];
          }
          pt = bgv.helper->packed_encode(pt);
          ciphertexts[r * ncols + c] = bgv.compact_encrypt_with_sk(pt);
        }
      }

      t.Tick("Encryption");

      machine->sendToParty(other_party, ciphertexts);

      t.Tick("Sending data");
      vector<Ciphertext> output_ciphertexts(nrows * ncols, Ciphertext(n));
      vector<uint64_t> Px;
      vector<uint64_t> xPx;

      for (int r = 0; r < nrows; r++) {
        for (int c = 0; c < ncols; c++) {
          machine->receiveFromParty(other_party,
                                    output_ciphertexts[r * ncols + c]);

          auto d = bgv.decrypt(output_ciphertexts[r * ncols + c]);
          d = bgv.helper->packed_decode(d);
          int count = 0;
          for (int k = 0; k < n; k++) {
            uint64_t temp = d[k] % bgv.helper->plaintextModulus;
            if (temp == 0) {
              count++;
            }
            Px.push_back(temp);
          }
        }
      }
      t.Tick("Decryption");

      int intersection_size = 0;
      for (int i = 0; i < Px.size(); i++) {
        if (Px[i] == 0) {
          intersection_size++;
        }
      }
      cout << "Intersection size: " << intersection_size << endl;

      if (!use_ot) {
        for (int r = 0; r < nrows; r++) {
          for (int c = 0; c < ncols; c++) {
            machine->receiveFromParty(other_party,
                                      output_ciphertexts[r * ncols + c]);

            auto d = bgv.decrypt(output_ciphertexts[r * ncols + c]);
            d = bgv.helper->packed_decode(d);
            int count = 0;
            for (int k = 0; k < n; k++) {
              uint64_t temp = d[k] % bgv.helper->plaintextModulus;
              xPx.push_back(temp);
            }
          }
        }
        t.Tick("Decryption");

        // Compute the union set
        std::vector<uint64_t> union_set;
        for (int i = 0; i < Px.size(); i++) {
          if (Px[i] != 0 && xPx[i] != 0) {
            uint64_t inv = invModP(Px[i]);
            uint64_t val = (unsigned __int128)inv * (unsigned __int128)xPx[i] %
                           18446744073709436929ULL;
            union_set.push_back(val);
          }
        }
        cout << "Diff size: " << union_set.size() << endl;
        t.Tick("Compute the diff set");
      } else {
        // Emulate OT
        vector<uint64_t> m0(nBuckets * max_size), mb(nBuckets * max_size);
        machine->receiveFromParty(other_party, (unsigned char *)m0.data(),
                                  8 * nBuckets * max_size);
        for (int r = 0; r < nBuckets; r++) {
          for (int c = 0; c < max_size; c++) {
            m0[r * max_size + c] -= mb[r * max_size + c];
          }
        }
        // End emulate OT
      }
    }

    if (party == 1) {
      machine->receiveFromParty(other_party, bgv.pk);

      for (int i = 0; i < nItems; i++) {
        input.push_back(nItems / 4 + i + 1);
      }

      int maxSize = 0;
      auto buckets = hashInput(input, nBuckets, "0xasdflafdlaf");

      // Padding to max size.
      // P0 also add 1 zeros at the end and shuffle the items in each bucket.
      // P1 just need to pad to max size.
      if (maxSize < max_size) {
        maxSize = max_size;
      }

      for (int r = 0; r < nBuckets; r++) {
        buckets[r].resize(maxSize, 0);
        random_shuffle(buckets[r].begin(), buckets[r].end());
      }
      map<int, vector<uint64_t>> plaintexts;
      map<int, vector<vector<uint64_t>>> plaintexts_power;
      vector<uint64_t> plaintext(n);
      for (int r = 0; r < nrows; r++) {
        for (int c = 0; c < ncols; c++) {
          for (int i = 0; i < n; i++) {
            plaintext[i] = buckets[r * n + i][c];
          }
          plaintexts[r * ncols + c] = plaintext;
        }
      }
      for (int r = 0; r < nrows; r++) {
        for (int c = 0; c < ncols; c++) {
          auto pt = plaintexts[r * ncols + c];
          plaintexts_power[r * ncols + c].push_back(pt);
          for (int k = 1; k < max_size; k++) {
            vector<uint64_t> temp(n);
            for (int i = 0; i < n; i++) {
              temp[i] =
                  (unsigned __int128)pt[i] *
                  (unsigned __int128)plaintexts_power[r * ncols + c][k - 1][i] %
                  bgv.helper->plaintextModulus;
            }
            plaintexts_power[r * ncols + c].push_back(temp);
          }
        }
      }
      t.Tick("Prepare plaintexts");

      vector<CompactedCiphertext> compacted_ciphertexts(nrows * max_size,
                                                        CompactedCiphertext(n));
      machine->receiveFromParty(other_party, compacted_ciphertexts);

      double tenc = 0;
      double teva = 0;

      map<int, vector<vector<vector<uint64_t>>>> ptNulls;
      for (int r = 0; r < nrows; r++) {
        cout << r << endl;
        Timer t;
        for (int c = 0; c < ncols; c++) {
          for (int k = 0; k < max_size; k++) {
            Timer t;
            vector<uint64_t> pt =
                bgv.helper->packed_encode(plaintexts_power[r * ncols + c][k]);
            tenc += t.Tick("");
            ptNulls[r * ncols + c].push_back(bgv.helper->ToEval(pt));
            teva += t.Tick("");
          }
        }
        t.Tick("time");
      }

      cout << "Encode time: " << tenc << endl;
      cout << "ToEval time: " << teva << endl;

      t.Tick("Transform plaintext");

      vector<Ciphertext> ciphertexts(nrows * max_size, Ciphertext(n));
      vector<Ciphertext> output_ciphertexts(nrows * ncols, Ciphertext(n));
      for (int i = 0; i < compacted_ciphertexts.size(); i++) {
        ciphertexts[i] = bgv.toCiphertext(compacted_ciphertexts[i]);
      }
      for (int r = 0; r < nrows; r++) {
        for (int c = 0; c < ncols; c++) {
          Ciphertext temp_ciphertext(n);
          output_ciphertexts[r * ncols + c] = ciphertexts[r * ncols];
          for (int k = 1; k < max_size; k++) {
            bgv.EvalMultPlain(temp_ciphertext, ciphertexts[r * ncols + k],
                              ptNulls[r * ncols + c][k - 1]);
            bgv.EvalAdd(output_ciphertexts[r * ncols + c], temp_ciphertext);
          }
          // Noise flooding
          auto noise_ciphertext = bgv.encrypt_noise_flooding_with_pk();
          bgv.EvalAdd(output_ciphertexts[r * ncols + c], noise_ciphertext);
          machine->sendToParty(other_party, output_ciphertexts[r * ncols + c]);
        }
      }
      t.Tick("Evaluate the polynomials");
      if (!use_ot) {
        vector<Ciphertext> output_ciphertexts(nrows * max_size, Ciphertext(n));
        for (int r = 0; r < nrows; r++) {
          for (int c = 0; c < ncols; c++) {
            Ciphertext temp_ciphertext(n);
            for (int k = 0; k < max_size; k++) {
              bgv.EvalMultPlain(temp_ciphertext, ciphertexts[r * ncols + k],
                                ptNulls[r * ncols + c][k]);
              bgv.EvalAdd(output_ciphertexts[r * ncols + c], temp_ciphertext);
            }
            // Noise flooding
            auto noise_ciphertext = bgv.encrypt_noise_flooding_with_pk();
            bgv.EvalAdd(output_ciphertexts[r * ncols + c], noise_ciphertext);
            machine->sendToParty(other_party,
                                 output_ciphertexts[r * ncols + c]);
          }
        }
        t.Tick("Evaluate the polynomials");
      } else {
        // Emulate OT
        vector<uint64_t> m0(nBuckets * max_size), m1(nBuckets * max_size);

        for (int r = 0; r < nBuckets; r++) {
          buckets[r].resize(max_size, 0);
          for (int c = 0; c < max_size; c++) {
            if (rand() % 2) {
              m0[r * max_size + c] = buckets[r][c] - m1[r * max_size + c];
            } else {
              m0[r * max_size + c] = buckets[r][c] - m0[r * max_size + c];
            }
          }
        }

        machine->sendToParty(other_party, (unsigned char *)m0.data(),
                             8 * nBuckets * max_size);
      }
    }

    return 0;
  }

  void GenerateInput(int party, vector<uint64_t> &data, int nItems,
                     float rate = 0.5) {
    if (party == Alice) {
      data.resize(nItems);

      for (int i = 0; i < nItems; i++) {
        data[i] = i + 1;
      }
    } else if (party == Bob) {
      int intersectionSize;

      if (rate <= 1)
        intersectionSize = nItems * rate;  // rand() % (nItems);
      else
        intersectionSize = rate;

      cout << "intersection size: " << intersectionSize << endl;

      data.resize(nItems);

      data[0] = 1;
      for (int i = 1; i < nItems; i++) {
        data[i] = (i + 1 + nItems - intersectionSize);
      }
    }
  }

  void GenerateInput(int party, vector<uint64_t> &data, int nItems) {
    data.resize(nItems);
    if (party == 0) {
      for (int i = 0; i < nItems; i++) {
        data[i] = uint64_t(i + 1);
      }
    } else {
      data[0] = uint64_t(1);
      for (int i = 1; i < nItems; i++) {
        data[i] = uint64_t(i + 1 + nItems);
      }
    }
  }
};

#endif
