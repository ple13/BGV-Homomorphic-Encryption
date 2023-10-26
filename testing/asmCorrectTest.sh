g++ -O3 -msse -msse2 -mavx assemblyModulo.cpp -lssl -lntl -lgmp -lpthread -std=c++11 -lcrypto ../fastMod.a -o assemblyModulo
./assemblyModulo
