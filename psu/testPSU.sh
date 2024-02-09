cd ../amd64.gmu/
sh script.sh
cd ../psu

g++ -std=c++11  -O3 -mavx2 -msse2 -maes -o PSU main.cc ../maths/utils.cpp ../PAHE/PAHE.cpp ../PAHE/RandomGenerator.cpp ../PAHE/PAHEHelper.cpp ../maths/HashInput.cpp -lssl -lntl -lgmp -lpthread -lcrypto ../fastMod.a

rm ../amd64.gmu/mult.o
rm ../fastMod.a

# 2 round PSU
# ./PSU 2 0 1048576 65536 60 0&
# ./PSU 2 1 1048576 65536 60 0

# ./PSU 2 0 262144 16384 59 0
# ./PSU 2 1 262144 16384 59 0

# 4 round PSU
# ./PSU 2 0 1048576 65536 60 1
# ./PSU 2 1 1048576 65536 60 1

# ./PSU 2 0 262144 16384 59 1
# ./PSU 2 1 262144 16384 59 1
