
#ifndef __UTIL_CPP__
#define __UTIL_CPP__

#include "util.hpp"

using namespace std;
using namespace sdsl;

void bitvector_to_char_array(char *p, const bit_vector &bv) {
    // parameter array p should be at least of size bitvector_size_in_bytes
    for (int i = 0; i < bitvector_size_in_bytes(bv); ++i)
        p[i] = 0;
    for (int i = 0; i < bv.size(); ++i)
        p[i / 8] |= bv[i] << (7 - (i % 8));
}

bit_vector char_array_to_bitvector(char *p, int size_in_bits) {
    // parameter size_in_bits should be at most 8 * lenght of p
    bit_vector bv = bit_vector(size_in_bits, 0);
    for (int i = 0; i < size_in_bits; ++i)
        bv[i] = p[i / 8] & (1 << (7 - (i % 8)));
    return bv;
}

#endif