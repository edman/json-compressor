
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
    byte_size += get_size(elem);
}

template <class T>
void BitmapIndex<T>::loadBitvector() {
    // Initialize a bit vector of size byte_size with zeroes
    bv = bit_vector(byte_size, 0);

    int c = 0, n = values.size();
    for (int i = 0; i < n - 1; i++) {
        c += get_size(values[i]);
        bv[c] = 1;
    }
}

template <class T>
bool BitmapIndex<T>::operator==(const BitmapIndex<T> &rhs) const {
    if (size() != rhs.size()) return false;
    for (int i = 0; i < size(); ++i) if (values[i] != rhs.values[i])
        return false;
    return bv == rhs.bv;
}

template <class T>
ostream& operator<<(ostream &o, const BitmapIndex<T> &b) {
    o<<"("<<b.byte_size<<",";
    o<<b.bv<<",";
    o<<"{";
    for (auto it : b.values) o<<"'"<<it<<"',";
    o<<"}";
    return o<<endl;
}


/* Explicit instantiation of needed template classes and methods */
template class BitmapIndex<string>;
template class BitmapIndex<Jvalue>;

template ostream& operator<< (ostream &o, const BitmapIndex<string> &b);
template ostream& operator<< (ostream &o, const BitmapIndex<Jvalue> &b);

#endif
