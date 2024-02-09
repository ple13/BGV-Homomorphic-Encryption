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

// g++ -O3 -msse -mavx generate_primes.cpp -lssl -lntl -lgmp -lpthread
// -std=c++11 -lcrypto -o generate_primes

#include <NTL/ZZ.h>

#include <iostream>

using namespace NTL;
using namespace std;

long witness(const ZZ& n, const ZZ& x) {
  ZZ m, y, z;
  long j, k;

  if (x == 0) return 0;

  // compute m, k such that n-1 = 2^k * m, m odd:
  k = 1;
  m = n / 2;
  while (m % 2 == 0) {
    k++;
    m /= 2;
  }

  z = PowerMod(x, m, n);  // z = x^m % n
  if (z == 1) return 0;

  j = 0;
  do {
    y = z;
    z = (y * y) % n;
    j++;
  } while (j < k && z != 1);

  return z != 1 || y != n - 1;
}

long PrimeTest(const ZZ& n, long t) {
  if (n <= 1) return 0;

  // first, perform trial division by primes up to 2000
  PrimeSeq s;  // a class for quickly generating primes in sequence
  long p;
  p = s.next();  // first prime is always 2
  while (p && p < 2000) {
    if ((n % p) == 0) return (n == p);
    p = s.next();
  }

  // second, perform t Miller-Rabin tests
  ZZ x;
  long i;

  for (i = 0; i < t; i++) {
    x = RandomBnd(n);  // random number between 0 and n-1
    if (witness(n, x)) return 0;
  }

  return 1;
}

// Generate a random prime p such that
// p = 2^bitlength - k*2^(1+logn) + 1
ZZ genPrime(int logn, int bitlength) {
  ZZ one = ZZ(1);
  ZZ n = ZZ(1 << (1 + logn));

  int count = 0;
  while (1) {
    auto ret = RandomPrime_ZZ(bitlength);
    ret -= one;
    auto temp = RightShift(ret, logn + 1);

    if (ret == temp * n) {
      return (ret + one);
    }

    count++;
  }

  return ZZ(0);
}

// Generate the max prime p such that
// p = 2^bitlength - k*2^(1+logn) + 1 (k >= 0)
ZZ genMaxPrime(int logn, int bitlength) {
  ZZ one = ZZ(1);
  ZZ n = ZZ(1 << (logn + 1));

  ZZ current = (ZZ(1) << bitlength) + one;
  ZZ count = ZZ(0);
  while (1) {
    if (PrimeTest(current, 100)) {
      cout << "k = " << count << endl;
      return current;
    } else {
      current -= n;
      count += one;
    }
  }

  return current;
}

int main() {
  auto p = genMaxPrime(12, 64);
  auto q = genMaxPrime(12, 192);

  cout << "plaintext modulus 64: " << p << endl;
  cout << "ciphertext modulus 192: " << q << endl;
  cout << "q % p: " << (q % p) << endl;

  auto halfq = (q >> 1);
  cout << "q/2: " << halfq << endl;
  cout << hex << halfq % (ZZ(1) << 64) << endl;
  halfq = (halfq >> 64);
  cout << hex << halfq % (ZZ(1) << 64) << endl;
  halfq = (halfq >> 64);
  cout << hex << halfq % (ZZ(1) << 64) << endl;
  return 0;
}
