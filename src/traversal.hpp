#ifndef __TRAVERSAL_HPP__
#define __TRAVERSAL_HPP__

#include "cjson.hpp"
#include "traversal_node.hpp"
#include "rapidjson/document.h"
#include <sdsl/bp_support_sada.hpp>
#include <vector>

using namespace std;
using namespace rapidjson;


class Traversal {
public:
    Cjson cjson;
    bp_support_sada<> bp;
    int nodeIndex;
    int treeIndex;

public:
    Traversal(Cjson &c, int i=0, int t=0): cjson(c), nodeIndex(i), treeIndex(t) { util::init_support(bp, &cjson.tree.bv); }
    Traversal(Value &d): cjson(d), nodeIndex(0), treeIndex(0) { util::init_support(bp, &cjson.tree.bv); }

    TraversalNode getCurrentNode();
    bool hasParent();
    bool goToParent();
    // bool backToParent();
    bool hasChild();
    bool goToChild();
    bool hasNextSibling();
    bool goToNextSibling();
};

ostream& operator<<(ostream &o, const Traversal &t);

#endif
