
#ifndef __PACKED_ARRAY_HPP__
#define __PACKED_ARRAY_HPP__

#include <utility>

using namespace std;

class PackedArray {
public:
    size_t _size;      // size in bytes of useful content
    size_t _capacity;  // total size in bytes of dynamic array
    char *_array;      // array of raw blob data

public:
    /* Constructors */
    // default constructor
    PackedArray(size_t size=0, char *array=nullptr) :
            _size(size), _capacity(size), _array(array) { }
    // move constructor
    PackedArray(PackedArray &&other) : _size(other._size),
            _capacity(other._capacity), _array(other._array) {
        other._size = 0; other._capacity = 0; other._array = nullptr;
    }
    /* Destructor */
    ~PackedArray() { if (_array != nullptr) { delete[] _array; } }

    /* Methods */
    template <typename T>
    void append(T &&newValue) {
        // compute total size required to store this new element
        size_t newTop = _size + sizeof(newValue);
        // double length of the dynamic array until it fits the new element
        while (newTop > _capacity) { doubleCapacity(); }
        // useful typedefs (there's some c++ complication going on here)
        typedef typename remove_reference<T>::type NoRefT;
        typedef typename add_pointer<NoRefT>::type NoRefTPtr;
        // get a reference to the first array slot available
        T &slot = *((NoRefTPtr) this->operator[](_size));
        // move (rvalue) or copy (lvalue) assign new value to the slot
        slot = std::forward<T>(newValue);
        // update the useful content pointer to new size
        _size = newTop;
    }

    void appendArray(char* valueArray, const size_t count) {
        for (int i = 0; i < count; ++i) {
            append(valueArray[i]);
        }
    }

    template <typename T>
    const T& get(const size_t index) {
        // assert we can read a value of type from the useful content area
        assert(index + sizeof(T) <= _size);
        // access the value at position index as being of type T
        // be warned that no guarantee is made that this is actually of type T
        T *value = (T*) this->operator[](index);
        // return the value as being of type T
        return *value;
    }

    char* getArray(const size_t index) {
        // assert the first element of the array is in the useful content area
        assert(index < _size);
        // it's up to the caller to manage array size and not go out of bounds
        return (char*) this->operator[](index);
    }

    // size of the useful portion of the array
    size_t size() const { return _size; }

    // total capacity of the array
    size_t capacity() const { return _capacity; }

    // copy array to an array of exactly the size of useful content
    void shrink() { copy_to_size(_size); }

    PackedArray& operator=(PackedArray &&other) {
        // Move-assign operator
        if (this != &other) {
            if (_array != nullptr) { delete[] _array; }

            _size = other._size;
            _capacity = other._capacity;
            _array = other._array;

            other._size = 0;
            other._capacity = 0;
            other._array = nullptr;
        }
        return *this;
    }

    bool operator==(const PackedArray &r) const {
        return this == &r ||
                (_size == r._size && memcmp(_array, r._array, _size) == 0);
    }

    void* operator[](const int index) const {
        assert(index <= _size); // index == size when we append new element
        return _array + index;
    }

private:
    static const uint INITIAL_CAPACITY = 200;

    void doubleCapacity() {
        // initialize if this is the first time double is being called
        if (_capacity == 0) { init(); return; }

        // double previous capacity
        size_t newCapacity = 2 * _capacity;
        // copy array to an array twice the capacity
        copy_to_size(newCapacity);
    }

    void copy_to_size(const size_t newCapacity) {
        assert(newCapacity >= _size); // ensure enough space for the content
        // do nothing if the dynamic array is already the desired capacity
        if (newCapacity == _capacity) { return; }

        // allocate new array
        char *newarray = new char[newCapacity];
        // copy useful contents of old array
        memcpy(newarray, _array, _size);
        // deallocate old array
        delete[] _array;
        // assign the newly allocate array
        _array = newarray;
        _capacity = newCapacity;
    }

    void init() {
        _capacity = INITIAL_CAPACITY;
        _array = new char[_capacity];
    }

};

#endif
