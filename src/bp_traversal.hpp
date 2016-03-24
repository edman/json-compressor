#ifndef __BPTRAVERSAL_HPP__
#define __BPTRAVERSAL_HPP__

#include "cjson.hpp"
#include "bp_tree.hpp"
#include "traversal_node.hpp"
#include "rapidjson/document.h"
#include <sdsl/bp_support_sada.hpp>

using namespace std;
using namespace rapidjson;


class BpTraversal {
public:
    Cjson<BpTree> cjson;
    bp_support_sada<> bp;
    int nodeIndex;
    int treeIndex;

public:
    BpTraversal(Cjson<BpTree> &c, int i=0, int t=0): cjson(c), nodeIndex(i), treeIndex(t) { util::init_support(bp, &cjson.tree.bv); }
    BpTraversal(Value &d): cjson(d), nodeIndex(0), treeIndex(0) { util::init_support(bp, &cjson.tree.bv); }

    TraversalNode getCurrentNode();
    bool hasParent();
    bool goToParent();
    // bool backToParent();
    int degree();
    bool hasChild();
    bool goToChild();
    bool hasNextSibling();
    bool goToNextSibling();
};

ostream& operator<<(ostream &o, const BpTraversal &t);

#endif
