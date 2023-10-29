#ifndef BGV_TEST_H__
#define BGV_TEST_H__

#include "../PAHE/PAHE.h"
#include <cassert>
#include <vector>
#include <random>

class BGVTest {
public:
  BGVTest() {
    bgv.init();
    bgv.keygen();
  }

  void testPublicKey();
  void testEncryption();
  void testEncryptionWithPk();
  void testPlainMult();
  void testCompactedPlainMult();
  void testSHE();
  void testSHEwithPK();
  void testPolynomialEval();
  void testSHEPackedEncode();
  void benchmark();
  void compacted_ciphertext_to_ciphertext();

  BGV bgv;
};

#endif
