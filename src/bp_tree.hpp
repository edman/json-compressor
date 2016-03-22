
#ifndef __BPTREE_HPP__
#define __BPTREE_HPP__

#include <sdsl/bit_vectors.hpp>
#include <sdsl/bp_support_sada.hpp>
#include "rapidjson/document.h"

using namespace std;
using namespace rapidjson;
using namespace sdsl;

// Balanced parentheses implementation of a succinct tree
class BpTree {
    public:
        int N;          // number of nodes in the JSON file
        bit_vector bv;  // bit vector that describes the DOM tree structure

    public:
        BpTree() { N=0; }
        BpTree(int n, bit_vector b): N(n), bv(b) {}
        BpTree(Value& v, int doc_size);
        BpTree(char *p, int size_in_bits);

        char* to_char_array() const;
        int size_in_bytes() const;
        int size() const;

        bool operator==(const BpTree &rhs) const;
        bool operator!=(const BpTree &rhs) const { return !(*this == rhs); }

    private:
        void documentDfs(Value&);
};

ostream& operator<<(ostream &o, const BpTree &t);

#endif

