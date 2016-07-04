#ifndef __DFTRAVERSAL_HPP__
#define __DFTRAVERSAL_HPP__

#include "cjson.hpp"
#include "df_tree.hpp"
#include "df_traverser.hpp"
#include "traversal_node.hpp"
#include "rapidjson/document.h"
#include <sdsl/bp_support_sada.hpp>
#include <sdsl/rank_support_v.hpp>
#include <sdsl/select_support_mcl.hpp>

using namespace std;
using namespace rapidjson;

class DfTraverser;

class DfTraversal {
public:
    Cjson<DfTree> *cjson;
    bp_support_sada<> bp;
    rank_support_v<0> rank_close;
    select_support_mcl<0> select_close;
    int nodeIndex;
    int treeIndex; // first index of current node description

public:
    DfTraversal(Cjson<DfTree> *c, int i=0, int t=1): cjson(c), nodeIndex(i), treeIndex(t) {
        util::init_support(bp, &c->tree.bv);
        util::init_support(rank_close, &c->tree.bv);
        util::init_support(select_close, &c->tree.bv);
    }

    // DfTraversal(Value &d): cjson(d), nodeIndex(0), treeIndex(1) {
    //     util::init_support(bp, &cjson.tree.bv);
    //     util::init_support(rank_close, &cjson.tree.bv);
    //     util::init_support(select_close, &cjson.tree.bv);
    // }

    DfTraverser getTraverser();
    TraversalNode getCurrentNode();
    TraversalNode getNode(int ni, int ti);
    const Jval& getValue(int ni, int ti);
    bool hasParent();
    bool hasParent(int ti);
    bool goToParent();
    DfTraverser getParent(int ni, int ti);
    int degree();
    int degree(int ni, int ti);
    bool hasChild();
    bool hasChild(int ni, int ti);
    bool goToChild(int i=0);
    DfTraverser getChild(int ni, int ti, int i=0);
    vector<DfTraverser> getChildren(int ni, int ti);
    bool hasNextSibling();
    bool hasNextSibling(int ni, int ti);
    int nextSiblingIndex();
    int nextSiblingIndex(int ni, int ti);
    bool goToNextSibling();
    DfTraverser getNextSibling(int ni, int ti);

private:
    void updateNodeIndexFromTreeIndex();
    void updateTreeIndexFromNodeIndex();
    int nodeIndexFromTreeIndex(int treeIndex);
    int treeIndexFromNodeIndex(int ni);
};

ostream& operator<<(ostream &o, const DfTraversal &t);

#endif
