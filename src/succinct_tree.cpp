
#ifndef __SUCCINCTTREE_CPP__
#define __SUCCINCTTREE_CPP__

#include "succinct_tree.hpp"
#include "rapidjson/document.h"

using namespace std;
using namespace rapidjson;
using namespace sdsl;

int counter = 0;

SuccinctTree::SuccinctTree(Value &document) {
    bv = bit_vector(100, 0);

    counter = 0;
    documentDfs(document);
    N = (counter - 1) >> 1;
}

void SuccinctTree::documentDfs(Value &d) {
    bv[counter++] = 1;

    if (d.IsObject())
        for (auto it = d.MemberBegin(); it != d.MemberEnd(); ++it)
            documentDfs(it->value);
    else if (d.IsArray())
        for (auto it = d.Begin(); it != d.End(); ++it)
            documentDfs(*it);

    counter++;
}

#endif

