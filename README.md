# BGV-Homomorphic-Encryption

# Compile asm code to object file
as --64 --defsym GCC=1 -o [out].o [in].s

# Build library
ar cr mylib.a [in].o

