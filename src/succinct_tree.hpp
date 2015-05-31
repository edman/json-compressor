
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
        SuccinctTree() {}
        SuccinctTree(Value&, int size);

    private:
        void documentDfs(Value&);
};

#endif

