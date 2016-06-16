#ifndef __DFTRAVERSER_HPP__
#define __DFTRAVERSER_HPP__

#include "cjson.hpp"
#include "df_traversal.hpp"
#include "traversal_node.hpp"

using namespace std;

class DfTraversal;

class DfTraverser {
public:
    DfTraversal *traversal;
    int nodeIndex;
    int treeIndex; // first index of current node description

public:
    DfTraverser(DfTraversal *trav=nullptr, int ni=0, int ti=1) :
            traversal(trav), nodeIndex(ni), treeIndex(ti) {}

    TraversalNode getNode();
    bool hasParent();
    DfTraverser getParent();
    int degree();
    bool hasChild();
    DfTraverser getChild(int i=0);
    vector<DfTraverser> getChildren();
    bool hasNextSibling();
    DfTraverser getNextSibling();
};

ostream& operator<<(ostream &o, const DfTraverser &t);

#endif
