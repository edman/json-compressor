
#ifndef __BITMAP_INDEX_HPP__
#define __BITMAP_INDEX_HPP__

#include "util.hpp"
#include "packed_array.hpp"
#include <sdsl/bit_vectors.hpp>
#include <vector>

using namespace std;
using namespace sdsl;

class BitmapIndex {
public:
    uint _size;
    bit_vector _bitmap;
    PackedArray _array;
    select_support_mcl<1> select;

public:
    /* Constructors */
    // default constructor
    BitmapIndex() : _size(0) {}
    // move constructor
    BitmapIndex(BitmapIndex&&);
    // deserialization constructor
    BitmapIndex(PackedArray &v) : _array(move(v)) {}
    /* Destructor */
    ~BitmapIndex() { if (boolBitmap != nullptr) { delete boolBitmap; } }

    /* Methods */
    void loadBitmap();
    // initialize the select support structure
    void loadSelect() { util::init_support(select, &_bitmap); }
    uint size() const { return _size; }

    template <typename T> void append(T &&elem) {
        // append new value to the packed array
        _array.append(std::forward<T>(elem));
        // increment element counter
        _size++;
        // mark the new elemnt position in boolBitmap
        markNewElementOfSize(sizeof(elem));
    }

    template <typename T> T& get(const uint index) const {
        // get a reference to the position in the packed array corresponding to
        // the index-th element
        T* value = (T*) this->operator[](index);
        return *value;
    }

    /* Operators */
    BitmapIndex& operator=(BitmapIndex&&); // move-assignment operator
    bool operator==(const BitmapIndex&) const;
    void* operator[](const uint) const;
    // void* operator [](int i) const { return _array[i]; }
    // T & operator [](int i) { return _array[i]; }

private:
    vector<bool> *boolBitmap = nullptr;
    void markNewElementOfSize(size_t); // mark a new element in boolBitmap
};

ostream& operator<<(ostream &o, const BitmapIndex &b);

#endif
