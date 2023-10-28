as --64 --defsym GCC=1 -o mult.o mult.s
rm ../fastMod.a
ar cr ../fastMod.a mult.o
