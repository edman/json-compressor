
#ifndef __BITMAP_INDEX_CPP__
#define __BITMAP_INDEX_CPP__

#include "bitmap_index.hpp"
#include "serialize.hpp"
#include "jvalue.hpp"

using namespace std;
using namespace sdsl;

template <class T>
void BitmapIndex<T>::insert(T elem) {
    values.push_back(elem);
    byte_size += get_size(elem) + 1;
}

template <class T>
void BitmapIndex<T>::loadBitvector() {
    // Initialize a bit vector of size byte_size with zeroes
    bv = bit_vector(byte_size, 0);

    int c = 0;
    for (T elem : values) {
        c += get_size(elem);
        bv[c++] = 1;
    }
}


/* Explicit instantiation of needed template classes */
template class BitmapIndex<string>;
template class BitmapIndex<Jvalue>;

#endif

