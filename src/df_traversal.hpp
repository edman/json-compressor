#ifndef __DFTRAVERSAL_HPP__
#define __DFTRAVERSAL_HPP__

#include "cjson.hpp"
#include "df_tree.hpp"
#include "traversal_node.hpp"
#include "rapidjson/document.h"
#include <sdsl/bp_support_sada.hpp>
#include <sdsl/rank_support_v.hpp>
#include <sdsl/select_support_mcl.hpp>

using namespace std;
using namespace rapidjson;


class DfTraversal {
public:
    Cjson<DfTree> cjson;
    bp_support_sada<> bp;
    rank_support_v<0> rank_close;
    select_support_mcl<0> select_close;
    int nodeIndex;
    int treeIndex;

public:
    DfTraversal(Cjson<DfTree> &c, int i=0, int t=1): cjson(c), nodeIndex(i), treeIndex(t) {
        util::init_support(bp, &cjson.tree.bv);
        util::init_support(rank_close, &cjson.tree.bv);
        util::init_support(select_close, &cjson.tree.bv);
    }

    DfTraversal(Value &d): cjson(d), nodeIndex(0), treeIndex(1) {
        util::init_support(bp, &cjson.tree.bv);
        util::init_support(rank_close, &cjson.tree.bv);
        util::init_support(select_close, &cjson.tree.bv);
    }

    TraversalNode getCurrentNode();
    bool hasParent();
    bool goToParent();
    int degree();
    bool hasChild();
    bool goToChild(int i=0);
    bool hasNextSibling();
    bool goToNextSibling();

private:
    void updateNodeIndexFromTreeIndex();
    void updateTreeIndexFromNodeIndex();
};

ostream& operator<<(ostream &o, const DfTraversal &t);

#endif
