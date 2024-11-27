#include "sha512.h"

#define S(x,y) (((x)>>(y))|(x)<<(32-(y)))
#define swap64(x) ((x >> 56) | ((x >> 40) & 0xff00ull) | ((x >> 24) & 0xff0000ull) | ((x >> 8) & 0xff000000ull) | ((x << 8) & 0xff00000000ull) | ((x << 24) & 0xff0000000000ull) | ((x << 40) & 0xff000000000000ull) | (x << 56))
#define Ch(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
#define Ma(x,y,z) (((x)&(y))^((x)&(z))^((y)&(z)))
#define S0(x) (S((x),2)^S((x),13)^S((x),22))
#define S1(x) (S((x),6)^S((x),11)^S((x),25))
#define o0(x) (S((x),7)^S((x),18)^((x)>>3))
#define o1(x) (S((x),17)^S((x),19)^((x)>>10))