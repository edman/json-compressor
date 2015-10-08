
#ifndef __BITMAP_INDEX_HPP__
#define __BITMAP_INDEX_HPP__

#include <sdsl/bit_vectors.hpp>
#include <vector>

using namespace std;
using namespace sdsl;

template <class T>
class BitmapIndex {
private:
    int byte_size = 0;
    vector<T> values;
    bit_vector bv;

public:
    void insert(T elem);
    void loadBitvector();

    size_t size() { return values.size(); }
};

#endif
