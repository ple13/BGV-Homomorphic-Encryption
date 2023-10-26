// g++ -O3 -ftree-vectorize -march=native -msse4.1 -mavx PAHE.cpp RandomGenerator.cpp PAHEHelper.cpp ../fastMod.a -lntl -lgmp -lpthread -std=c++11 -maes -pthread

#include <stdio.h>
#include <vector>
#include <string.h>
#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>
#include <cassert>

#include "PAHE.h"

#define USE_PACKING 1

void BGV::init(int n, bool use_packing) {
  this->n = n;
  this->logn = log2(n);

  root_of_unity_ciphertext.resize(3);
  root_of_unity_ciphertext[0] = 10904640868020604749ULL;
  root_of_unity_ciphertext[1] =  4064029909171694920ULL;
  root_of_unity_ciphertext[2] = 14460608192583649110ULL;

  root_of_unity_plaintext = 7972528404776578548;
  
  helper = new PAHEHelper();
  helper->ftt_precompute_ciphertext();
  helper->ftt_precompute_plaintext();

  prng = new NoiseGenerator(4.0);
}

void BGV::keygen() {
  // Generate secket key
  sk = prng->GenerateBinaryVector(n);
  
  // Convert sk to FTT form
  sk = helper->ToEval(sk);
  
  pk.b = prng->GenerateUniformVector(n);
  pk.a.resize(n, vector<uint64_t>(3));
  // Sample Gaussian noise
  vector<vector<uint64_t>> e = prng->GenerateGaussianVector(n);
  
  // Compute te
  for (int i = 0; i < n; i++) {
    asmMulWithPModQ(e[i].data(), e[i].data());
  }
  
  // te -> FTT(te)
  e = helper->ToEval(e);
  
  // pk = (-bs + te, b)
  for (int i = 0; i < n; i++) {
    // a1 = b*sk
    asmMulModQ(pk.a[i].data(), pk.b[i].data(), sk[i].data());
    // a = te - b*sk
    asmSubModQ(pk.a[i].data(), e[i].data(), pk.a[i].data());
  }
}

Ciphertext BGV::encrypt_with_sk(vector<uint64_t>& pt) {
  Ciphertext c(n);
  
  // pt -> NTT(pt)
  auto eval_pt = helper->ToEval(pt);
  
  // Sample randomness (b) for ciphertext (-bs + te + m, b)
  c.b = prng->GenerateUniformVector(n);
  
  vector<vector<uint64_t>> e = prng->GenerateGaussianVector(n);
  
  // Compute te
  for (int i = 0; i < n; i++) {
    asmMulWithPModQ(e[i].data(), e[i].data());
  }
  
  // te -> NTT(te)
  e = helper->ToEval(e);
  
  // pk = (-bs + te, b)
  for (int i = 0; i < n; i++) {
    asmMulModQ(c.a[i].data(), c.b[i].data(), sk[i].data());
    asmSubModQ(c.a[i].data(), e[i].data(), c.a[i].data());
    asmAddModQ(c.a[i].data(), c.a[i].data(), eval_pt[i].data());
  }
  
  return c;
}

Ciphertext BGV::encrypt_with_pk(vector<uint64_t>& pt) {
  Ciphertext c(n);

  // Generate u, e1, and d2
  vector<vector<uint64_t>> u = prng->GenerateZOVector(n);
  vector<vector<uint64_t>> e1 = prng->GenerateGaussianVector(n);
  vector<vector<uint64_t>> e2 = prng->GenerateGaussianVector(n);

  // Compute te
  vector<uint64_t> val(3, 0);
  for (int i = 0; i < n; i++) {
    val[0] = pt[i];
    asmMulWithPModQ(e1[i].data(), e1[i].data());
    asmAddModQ(e1[i].data(), e1[i].data(), val.data());
    asmMulWithPModQ(e2[i].data(), e2[i].data());
  }

  // te -> NTT(te)
  u  = helper->ToEval(u);
  e1 = helper->ToEval(e1);
  e2 = helper->ToEval(e2);

  // pk = (pk0, pk1)
  // ct = (pk0*u + te1 + m, pk1*u + te2)
  for (int i = 0; i < n; i++) {
    asmMulModQ(c.a[i].data(), pk.a[i].data(), u[i].data());
    asmAddModQ(c.a[i].data(), c.a[i].data(), e1[i].data());
    asmMulModQ(c.b[i].data(), pk.b[i].data(), u[i].data());
    asmAddModQ(c.b[i].data(), c.b[i].data(), e2[i].data());
  }
  
  return c;
}

void BGV::EvalAdd(Ciphertext& dst, Ciphertext& src) {
  for(int i = 0; i < n; i++){
    asmAddModQ(dst.a[i].data(), dst.a[i].data(), src.a[i].data());
    asmAddModQ(dst.b[i].data(), dst.b[i].data(), src.b[i].data());
  }
}

void BGV::EvalAddPlain(Ciphertext& ct, vector<uint64_t>& pt) {
  auto eval_pt = helper->ToEval(pt);
  for(int i = 0; i < n; i++){
    asmAddModQ(ct.a[i].data(), ct.a[i].data(), eval_pt[i].data());
  }
}

void BGV::EvalAddPlain(Ciphertext& ct, vector<vector<uint64_t>>& eval_pt) {
  for(int i = 0; i < n; i++){
    asmAddModQ(ct.a[i].data(), ct.a[i].data(), eval_pt[i].data());
  }
}

void BGV::EvalMultPlain(Ciphertext& ct, vector<uint64_t>& pt) {
  auto eval_pt = helper->ToEval(pt);
  for(int i = 0; i < n; i++){
    asmMulModQ(ct.a[i].data(), ct.a[i].data(), eval_pt[i].data());
    asmMulModQ(ct.b[i].data(), ct.b[i].data(), eval_pt[i].data());
  }
}

void BGV::EvalMultPlain(Ciphertext& ct, vector<vector<uint64_t>>& eval_pt) {
  for(int i = 0; i < n; i++){
    asmMulModQ(ct.a[i].data(), ct.a[i].data(), eval_pt[i].data());
    asmMulModQ(ct.b[i].data(), ct.b[i].data(), eval_pt[i].data());
  }
}

void BGV::EvalMultPlain(Ciphertext& dst, Ciphertext& ct, vector<uint64_t>& pt) {
  auto eval_pt = helper->ToEval(pt);
  for(int i = 0; i < n; i++){
    asmMulModQ(dst.a[i].data(), ct.a[i].data(), eval_pt[i].data());
    asmMulModQ(dst.b[i].data(), ct.b[i].data(), eval_pt[i].data());
  }
}

void BGV::EvalMultPlain(Ciphertext& dst, Ciphertext& ct, vector<vector<uint64_t>>& eval_pt) {
  for(int i = 0; i < n; i++){
    asmMulModQ(dst.a[i].data(), ct.a[i].data(), eval_pt[i].data());
    asmMulModQ(dst.b[i].data(), ct.b[i].data(), eval_pt[i].data());
  }
}

NTL::ZZ U3ToZZ(vector<uint64_t> u) {
  NTL::ZZ ret = NTL::ZZ(0);
  ret += NTL::ZZFromBytes((unsigned char *)&u[2], 8);
  ret = (ret << 64);
  ret += NTL::ZZFromBytes((unsigned char *)&u[1], 8);
  ret = (ret << 64);
  ret += NTL::ZZFromBytes((unsigned char *)&u[0], 8);
  return ret;
}

vector<uint64_t> BGV::decrypt(Ciphertext& c) {
  vector<vector<uint64_t>> ret(n, vector<uint64_t>(3));
  for(int i = 0; i < n; i++) {
    asmMulModQ(ret[i].data(), c.b[i].data(), sk[i].data());
    asmAddModQ(ret[i].data(), ret[i].data(), c.a[i].data());
  }
  ret = helper->ToCoeff(ret);
  vector<uint64_t> output(n);
  unsigned __int128 temp;
  for(int i = 0; i < n; i++) {
    // If (ret[i] > q/2) --> ret[i] -= q
    asmCorrect(ret[i].data());
    temp = ret[i][0];
    temp += (unsigned __int128)ret[i][1]*((unsigned __int128)114687ULL);
    temp += (unsigned __int128)ret[i][2]*((unsigned __int128)13153107969ULL);
    output[i] = temp % 18446744073709436929ULL;
//     asmModPInQ(&output[i], ret[i].data());
  }
  return output;
}
