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
  void testMultiplePlainMult();
  void benchmark();

  BGV bgv;
};

#endif
