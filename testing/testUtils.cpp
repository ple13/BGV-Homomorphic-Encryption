#include "testUtils.h"

void ZZToU3(ZZ val, uint64_t *u) {
  ZZ temp;
  ZZ one = ZZ(1);
  temp = val % (one << 64);
  BytesFromZZ((unsigned char *)&u[0], val, 8);
  val = val >> 64;temp = val % (one << 64);
  BytesFromZZ((unsigned char *)&u[1], val, 8);
  val = val >> 64;temp = val % (one << 64);
  BytesFromZZ((unsigned char *)&u[2], val, 8);
}

ZZ U3ToZZ(vector<uint64_t> u) {
  ZZ ret = ZZ(0);
  ret += ZZFromBytes((unsigned char *)&u[2], 8);
  ret = (ret << 64);
  ret += ZZFromBytes((unsigned char *)&u[1], 8);
  ret = (ret << 64);
  ret += ZZFromBytes((unsigned char *)&u[0], 8);
  return ret;
}

void printZZ(ZZ val) {
  ZZ one = ZZ(1);
  cout << val << endl;
  cout << "ZZ[0]: " << (val % (one << 64)) << endl;
  val = val >> 64;
  cout << "ZZ[1]: " << (val % (one << 64)) << endl;
  val = val >> 64;
  cout << "ZZ[2]: " << (val % (one << 64)) << endl;
}

void printU3(vector<uint64_t> val) {
  cout << "v[0]: " << val[0] << endl;
  cout << "v[1]: " << val[1] << endl;
  cout << "v[2]: " << val[2] << endl;
}

std::ostream&
operator<<( std::ostream& dest, __int128_t value ) {
  std::ostream::sentry s( dest );
  if ( s ) {
    __int128_t tmp = value < 0 ? -value : value;
    char buffer[ 128 ];
    char* d = std::end( buffer );
    do
    {
      -- d;
      *d = "0123456789"[ tmp % 10 ];
      tmp /= 10;
    } while ( tmp != 0 );
    if ( value < 0 ) {
      -- d;
      *d = '-';
    }
    int len = std::end( buffer ) - d;
    if ( dest.rdbuf()->sputn( d, len ) != len ) {
      dest.setstate( std::ios_base::badbit );
    }
  }
  return dest;
}
