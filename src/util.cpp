
#ifndef __UTIL_CPP__
#define __UTIL_CPP__

#include "util.hpp"
#include "jval.hpp"
#include "bitmap_index.hpp"
#include "util.hpp"
#include <climits>
#include <cstdlib>
#include <cstring>

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

char* string_to_cstr(const string &s) {
    char *p = new char[s.size() + 1];
    strcpy(p, s.c_str());
    return p;
}

char* cstr_copy(const char *const cstr) {
    if (cstr == nullptr) return nullptr;
    char *p = new char[strlen(cstr) + 1];
    strcpy(p, cstr);
    return p;
}

bool intInChar(int num) { return abs(num) <= SCHAR_MAX; }

bool intInShort(int num) { return num <= SHRT_MAX; }

bool isMaxUint(uint num) { return num == MAX_UINT; }

uint jvalByteSize(const Jval &val) {
    if (!val.hasName()) {
        // val does not have name
        if (!val.hasValue()) return sizeof(Jval);
        if (val.isString()) return sizeof(StringJval);
        if (val.isChar()) return sizeof(CharJval);
        if (val.isShort()) return sizeof(ShortJval);
        if (val.isInt()) return sizeof(IntJval);
        if (val.isDouble()) return sizeof(DoubleJval);
        // unknown type (shouldn't happen)
        assert(false);
    }
    // val has name
    if (!val.hasValue()) return sizeof(NamedJval);
    if (val.isString()) return sizeof(NamedStringJval);
    if (val.isChar()) return sizeof(NamedCharJval);
    if (val.isShort()) return sizeof(NamedShortJval);
    if (val.isInt()) return sizeof(NamedIntJval);
    if (val.isDouble()) return sizeof(NamedDoubleJval);
    // unknown type (shouldn't happen)
    assert(false);
    return 0;
}

void loadJvalBitmapIndex(BitmapIndex &index) {
    // total size of bitmap corresponds to the size of packed array
    uint byte_size = index._array.size();
    // allocate a bit vector with the appropriate size
    bit_vector bitmap(byte_size, 0);

    // initialize counter for number of elements in the bitmap index
    uint size = 0;
    for (uint cur = 0; cur < byte_size;) {
        // set a bit to 1 in the beginning of every element
        bitmap[cur] = 1;
        // skip the remanining bits of the current element
        cur += jvalByteSize(index._array.get<Jval>(cur));
        // count the current element
        size++;
    }

    // assign member attributes and load the select structure
    index._size = size;
    index._bitmap = move(bitmap);
    index.loadSelect();
}

#endif
