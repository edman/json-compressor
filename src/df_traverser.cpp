#ifndef __DFTRAVERSER_CPP__
#define __DFTRAVERSER_CPP__

#include "df_traverser.hpp"
#include "df_traversal.hpp"
#include "df_tree.hpp"
#include "jvalue.hpp"
#include <iostream>

using namespace std;

TraversalNode DfTraverser::getNode() {
  return traversal->getNode(nodeIndex, treeIndex);
}

const Jval& DfTraverser::getValue() const {
  return traversal->getValue(nodeIndex, treeIndex);
}

bool DfTraverser::hasParent() {
  return traversal->hasParent(treeIndex);
}

DfTraverser DfTraverser::getParent() {
  return traversal->getParent(nodeIndex, treeIndex);
}

int DfTraverser::degree() {
  return traversal->degree(nodeIndex, treeIndex);
}

bool DfTraverser::hasChild() {
  return traversal->hasChild(nodeIndex, treeIndex);
}

DfTraverser DfTraverser::getChild(int i) const {
  return traversal->getChild(nodeIndex, treeIndex, i);
}

vector<DfTraverser> DfTraverser::getChildren() const {
  return traversal->getChildren(nodeIndex, treeIndex);
}

bool DfTraverser::hasNextSibling() {
  return traversal->hasNextSibling(nodeIndex, treeIndex);
}

DfTraverser DfTraverser::getNextSibling() {
  return traversal->getNextSibling(nodeIndex, treeIndex);
}

ostream& operator<<(ostream &o, const DfTraverser &t) {
  return o << "(" << t.nodeIndex << "," << t.treeIndex << ")";
}

#endif