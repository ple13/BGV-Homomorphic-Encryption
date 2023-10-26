g++ -O3 -o testPAHE testPAHE.cpp ../PAHE/RandomGenerator.cpp ../PAHE/PAHEHelper.cpp  ../PAHE/PAHE.cpp  BGVTest.cpp -lssl -lntl -lgmp -lpthread -std=c++11 -lcrypto ../fastMod.a
./testPAHE
