
#ifndef __BITMAP_INDEX_HPP__
#define __BITMAP_INDEX_HPP__

#include <sdsl/bit_vectors.hpp>
#include <vector>

using namespace std;
using namespace sdsl;

template <class T>
class BitmapIndex {
public:
    // int byte_size = 0;
    // bit_vector bv;
    vector<T> values;

public:
    BitmapIndex() {}  // default constructor
    // BitmapIndex(int bs, bit_vector b, vector<T> &v): byte_size(bs), bv(b), values(v) { values.shrink_to_fit(); }
    BitmapIndex(vector<T> &v): values(v) { values.shrink_to_fit(); }
    void insert(T elem);
    void loadBitvector();

    size_t size() const { return values.size(); }

    bool operator==(const BitmapIndex &rhs) const;
    T operator [](int i) const { return values[i]; }
    T & operator [](int i) { return values[i]; }

};

template <class T>
ostream& operator<<(ostream &o, const BitmapIndex<T> &b);

#endif
