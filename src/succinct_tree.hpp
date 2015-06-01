
#ifndef __SUCCINCTTREE_HPP__
#define __SUCCINCTTREE_HPP__

#include <sdsl/bit_vectors.hpp>
#include <sdsl/bp_support_sada.hpp>
#include "rapidjson/document.h"

using namespace std;
using namespace rapidjson;
using namespace sdsl;

class SuccinctTree {
    public:
        int N;
        bit_vector bv;

    public:
        SuccinctTree() { N=0; }
        SuccinctTree(Value&, int doc_size);
        SuccinctTree(char *p, int size_in_bits);

        char* to_char_array() const;
        int size_in_bytes() const;
        int size() const;

        bool operator==(const SuccinctTree &rhs) const;
        bool operator!=(const SuccinctTree &rhs) const { return !(*this == rhs); }

    private:
        void documentDfs(Value&);
};

ostream& operator<<(ostream &o, const SuccinctTree &t);

#endif

