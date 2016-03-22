
#ifndef __DFTREE_HPP__
#define __DFTREE_HPP__

#include <sdsl/bit_vectors.hpp>
#include <sdsl/bp_support_sada.hpp>
#include "rapidjson/document.h"

using namespace std;
using namespace rapidjson;
using namespace sdsl;

// Depth-First Unary Degree Sequence (DFUDS) implementation of a succinct tree
class DfTree {
    public:
        int N;          // number of nodes in the JSON file
        bit_vector bv;  // bit vector that describes the DOM tree structure

    public:
        DfTree() { N=0; }
        DfTree(int n, bit_vector b): N(n), bv(b) {}
        DfTree(Value& v, int doc_size);
        DfTree(char *p, int size_in_bits);

        char* to_char_array() const;
        int size_in_bytes() const;
        int size() const;

        bool operator==(const DfTree &rhs) const;
        bool operator!=(const DfTree &rhs) const { return !(*this == rhs); }

    private:
        void documentDfs(Value&);
};

ostream& operator<<(ostream &o, const DfTree &t);

#endif
