
#ifndef __UTIL_HPP__
#define __UTIL_HPP__

#include <sdsl/bit_vectors.hpp>

using namespace std;
using namespace sdsl;

inline size_t bit_size_to_bytes(const size_t bit_size){ return (bit_size + 7) / 8; }
inline size_t bitvector_size_in_bytes(const bit_vector &bv){ return (bv.size() + 7) / 8; }
void bitvector_to_char_array(char *p, const bit_vector &bv);
bit_vector char_array_to_bitvector(char *p, int size_in_bits);

#endif
