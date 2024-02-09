#ifndef TRIPLE_H__
#define TRIPLE_H__

#pragma once
#include <cstddef>
#include <iterator>
#include <sstream>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <cstring>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <algorithm>
#include <future>

#include <unistd.h>
#include <NTL/ZZ.h>
#include <emmintrin.h>

#include "Machine.hpp"

#include "../PAHE/PAHE.h"
#include "../PAHE/RandomGenerator.h"
#include "../maths/HashInput.h"
#include "../maths/utils.h"

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

class Triple {
public:
  Triple(n) {
    share_a.resize(n);
    share_b.resize(n);
    share_c.resize(n);
    share_mac_a.resize(n);
    share_mac_b.resize(n);
    share_mac_c.resize(n);
  }

  vector<uint64_t> share_a;
  vector<uint64_t> share_b;
  vector<uint64_t> share_c;
  vector<uint64_t> share_mac_a;
  vector<uint64_t> share_mac_b;
  vector<uint64_t> share_mac_c;

  uint46_t share_mac_key;
};

class MpcOffline {
public:
  int machineId;
  int party;
  int partner;
  int totalItems;
  int totalParties;

  BGV bgv;

  Machine *machine;

  Ciphertext encryptedMacKey;

  MpcOffline (int machineId, int party, int totalParties, int totalItems, Machine *machine) {
    this->totalParties = totalParties;
    this->machineId = machineId;
    this->party = party;
    this->totalItems = totalItems;
    this->machine = machine;
    bgv.init();
    bgv.keygen();
  }

  ~Triple(){
    delete machine;
  }

  void TripleGeneration(int test) {
    int nItems = totalItems;
    Timer t;

    PSU_AHE(party, nItems, machine);
    if (party == 0) {
        cout << "Communication (MBytes): " << machine->num_bytes_send/1024.0/1024.0
             << "\t" << machine->num_bytes_recv/1024.0/1024.0 << "\t"
             << (machine->num_bytes_send + machine->num_bytes_recv)/1024.0/1024.0 << endl;
    }
    t.Tick("PSU************************************");
  }

  void GetEncryptedMacKey(int party, Machine *machie)
    // Each party sample a mac key and encrypts it
    vector<uint64_t> partial_mac(bgv.n);
    RAND_bytes((unsigned char *)partial_mac.data(), sizeof(uint46_t));
    Ciphertext partiallyEncryptedMacKey = bgv.encrypt_with_pk(partial_mac);

    if (party == 0) {
        Ciphertext encryptedMacKey = partiallyEncryptedMacKey;
        for (int i = 1; i < totalParties; i++) {
            machine->receiveFromParty(0, i, partiallyEncryptedMacKey);
            bgv.EvalAdd(encryptedMacKey, partiallyEncryptedMacKey);
        }
        for (int i = 1; i < totalParties; i++) {
            machine->sendToParty(0, i, encryptedMacKey);
        }
    } else {
        machine->sendToParty(party, 0, partiallyEncryptedMacKey);
        machine->receiveFromParty(party, 0, encryptedMacKey);
    }
  }

  int PSU_AHE(int party, int nItems, Machine *machine) {
    vector<uint64_t> input;

    int n = bgv.n;
    int nrows = nItems/n;

    Timer t;

    if (party == 0) {
      machine->sendToParty(0, 1, bgv.pk);

      for (int i = 0; i < nItems; i++) {
        input.push_back(i+1);
      }

      vector<Ciphertext> ciphertexts(nrows);

      vector<uint64_t> pt(n);
      for (int r = 0; r < nrows; r++) {
          for (int i = 0; i < n; i++) {
            pt[i] = input[r*n + i];
          }
          pt = bgv.helper->packed_encode(pt);
          ciphertexts[r] = bgv.encrypt_with_pk(pt);
      }

      t.Tick("Encryption");

      machine->sendToParty(0, 1, ciphertexts);

      t.Tick("Sending data");
      vector<Ciphertext> output_ciphertexts(nrows*ncols, Ciphertext(n));
      vector<uint64_t> Px;
      vector<uint64_t> xPx;

      for (int r = 0; r < nrows; r++) {
        for (int c = 0; c < ncols; c++) {
          machine->receiveFromParty(0, 1, output_ciphertexts[r*ncols + c]);

          auto d = bgv.decrypt(output_ciphertexts[r*ncols + c]);
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
            machine->receiveFromParty(0, 1, output_ciphertexts[r*ncols + c]);

            auto d = bgv.decrypt(output_ciphertexts[r*ncols + c]);
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
        for(int i = 0; i < Px.size(); i++) {
          if (Px[i] != 0 && xPx[i] != 0) {
            uint64_t inv = invModP(Px[i]);
            uint64_t val = (unsigned __int128)inv*(unsigned __int128)xPx[i] % 18446744073709436929ULL;
            union_set.push_back(val);
          }
        }
        cout << "Diff size: " << union_set.size() << endl;
        t.Tick("Compute the diff set");
      } else {
        // Emulate OT
        vector<uint64_t> m0(nBuckets*max_size), mb(nBuckets*max_size);
        machine->receiveFromParty(0, 1, (unsigned char *)m0.data(), 8*nBuckets*max_size);
        for (int r = 0; r < nBuckets; r++) {
          for (int c = 0; c < max_size; c++) {
            m0[r*max_size + c] -= mb[r*max_size + c];
          }
        }
        // End emulate OT
      }
    }

    if (party == 1) {
      machine->receiveFromParty(1, 0, bgv.pk);

      for (int i = 0; i < nItems; i++) {
        input.push_back(nItems/4 + i+1);
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
            plaintext[i] = buckets[r*n + i][c];
          }
          plaintexts[r*ncols + c] = plaintext;
        }
      }
      for (int r = 0; r < nrows; r++) {
        for (int c = 0; c < ncols; c++) {
          auto pt = plaintexts[r*ncols + c];
          plaintexts_power[r*ncols + c].push_back(pt);
          for (int k = 1; k < max_size; k++) {
            vector<uint64_t> temp(n);
            for (int i = 0; i < n; i++) {
              temp[i] =
                (unsigned __int128)pt[i] *
                (unsigned __int128)plaintexts_power[r*ncols + c][k-1][i] %
                bgv.helper->plaintextModulus;
            }
            plaintexts_power[r*ncols + c].push_back(temp);
          }
        }
      }
      t.Tick("Prepare plaintexts");

      vector<CompactedCiphertext> compacted_ciphertexts(nrows*max_size, CompactedCiphertext(n));
      machine->receiveFromParty(1, 0, compacted_ciphertexts);

      double tenc = 0;
      double teva = 0;

      map<int, vector<vector<vector<uint64_t>>>> ptNulls;
      for (int r = 0; r < nrows; r++) {
        cout << r << endl;
        Timer t;
        for (int c = 0; c < ncols; c++) {
          for (int k = 0; k < max_size; k++) {
            Timer t;
            vector<uint64_t> pt = bgv.helper->packed_encode(plaintexts_power[r*ncols + c][k]);
            tenc += t.Tick("");
            ptNulls[r*ncols + c].push_back(bgv.helper->ToEval(pt));
            teva += t.Tick("");
          }
        }
        t.Tick("time");
      }

      cout << "Encode time: " << tenc << endl;
      cout << "ToEval time: " << teva << endl;

      t.Tick("Transform plaintext");

      vector<Ciphertext> ciphertexts(nrows*max_size, Ciphertext(n));
      vector<Ciphertext> output_ciphertexts(nrows*ncols, Ciphertext(n));
      for (int i = 0; i < compacted_ciphertexts.size(); i++) {
        ciphertexts[i] = bgv.toCiphertext(compacted_ciphertexts[i]);
      }
      for (int r = 0; r < nrows; r++) {
        for (int c = 0; c < ncols; c++) {
          Ciphertext temp_ciphertext(n);
          output_ciphertexts[r*ncols + c] = ciphertexts[r*ncols];
          for (int k = 1; k < max_size; k++) {
            bgv.EvalMultPlain(temp_ciphertext,
              ciphertexts[r*ncols + k], ptNulls[r*ncols + c][k-1]);
            bgv.EvalAdd(output_ciphertexts[r*ncols + c], temp_ciphertext);
          }
          // Noise flooding
          auto noise_ciphertext = bgv.encrypt_noise_flooding_with_pk();
          bgv.EvalAdd(output_ciphertexts[r*ncols + c], noise_ciphertext);
          machine->sendToParty(1, 0, output_ciphertexts[r*ncols + c]);
        }
      }
      t.Tick("Evaluate the polynomials");
      if (!use_ot) {
        vector<Ciphertext> output_ciphertexts(nrows*max_size, Ciphertext(n));
        for (int r = 0; r < nrows; r++) {
          for (int c = 0; c < ncols; c++) {
            Ciphertext temp_ciphertext(n);
            for (int k = 0; k < max_size; k++) {
              bgv.EvalMultPlain(temp_ciphertext,
                ciphertexts[r*ncols + k], ptNulls[r*ncols + c][k]);
              bgv.EvalAdd(output_ciphertexts[r*ncols + c], temp_ciphertext);
            }
            // Noise flooding
            auto noise_ciphertext = bgv.encrypt_noise_flooding_with_pk();
            bgv.EvalAdd(output_ciphertexts[r*ncols + c], noise_ciphertext);
            machine->sendToParty(1, 0, output_ciphertexts[r*ncols + c]);
          }
        }
        t.Tick("Evaluate the polynomials");
      } else {
        // Emulate OT
        vector<uint64_t> m0(nBuckets*max_size), m1(nBuckets*max_size);

        for (int r = 0; r < nBuckets; r++) {
          buckets[r].resize(max_size, 0);
          for (int c = 0; c < max_size; c++) {
            if (rand() % 2) {
              m0[r*max_size + c] = buckets[r][c] - m1[r*max_size + c];
            } else {
              m0[r*max_size + c] = buckets[r][c] - m0[r*max_size + c];
            }
          }
        }

        machine->sendToParty(1, 0, (unsigned char *)m0.data(), 8*nBuckets*max_size);
      }
    }

    return 0;
  }

  void GenerateInput(int party, vector<uint64_t>& data, int nItems,
		     float rate = 0.5) {
    if(party == Alice) {
      data.resize(nItems);

      for(int i = 0; i < nItems; i++) {
        data[i] = i + 1;
      }
    } else if(party == Bob) {
      int intersectionSize;

      if(rate <= 1) intersectionSize = nItems*rate; //rand() % (nItems);
      else intersectionSize = rate;

      cout << "intersection size: " << intersectionSize << endl;

      data.resize(nItems);

      data[0] = 1;
      for(int i = 1; i < nItems; i++) {
        data[i] = (i + 1 + nItems - intersectionSize);
      }
    }
  }

  void GenerateInput(int party, vector<uint64_t>& data, int nItems) {
    data.resize(nItems);
    if(party == 0) {
      for(int i = 0; i < nItems; i++) {
	data[i] = uint64_t(i + 1);
      }
    } else {
      data[0] = uint64_t(1);
      for(int i = 1; i < nItems; i++) {
	data[i] = uint64_t(i + 1 + nItems);
      }
    }
  }
};

#endif









