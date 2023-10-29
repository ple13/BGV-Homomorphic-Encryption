#include "../PAHE/PAHE.h"
#include "BGVTest.h"
#include <cassert>
#include <vector>
#include <random>

using namespace std;


int main() {
  int batch = 100;
  BGVTest bgv;
  Timer t;
  for (int i = 0; i < batch; i++) {
    bgv.testPublicKey();
  }
  t.Tick("Public Key test");

  for (int i = 0; i < batch; i++) {
    bgv.testEncryption();
  }
  t.Tick("Encryption");

  for (int i = 0; i < batch; i++) {
    bgv.testEncryptionWithPk();
  }
  t.Tick("Encryption With PK");
  for (int i = 0; i < batch; i++) {
    bgv.testPlainMult();
  }
  t.Tick("Plain Mult");

  for (int i = 0; i < batch; i++) {
    bgv.testSHE();
  }
  t.Tick("SHE");
  for (int i = 0; i < batch; i++) {
    bgv.testSHEwithPK();
  }
  t.Tick("SHE with PK");

  cout << "Running benchmark test" <<endl;
  bgv.benchmark();

  bgv.compacted_ciphertext_to_ciphertext();
  cout << "Compacted ciphertext to ciphertext....    PASSED" << endl;

  return 0;
}

