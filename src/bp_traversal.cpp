#ifndef __BPTRAVERSAL_CPP__
#define __BPTRAVERSAL_CPP__

#include "bp_traversal.hpp"
#include "bp_tree.hpp"
#include "jvalue.hpp"
#include <iostream>

using namespace std;


TraversalNode BpTraversal::getCurrentNode() {
  if (nodeIndex == 0) return TraversalNode("", Jvalue::OBJECT_VAL);
  return TraversalNode(cjson.names[cjson.nameList[nodeIndex - 1]],
      cjson.values[nodeIndex - 1]);
}

bool BpTraversal::hasParent() {
  // true unless tree index is 0
  return treeIndex > 0;
}

bool BpTraversal::goToParent() {
  // no parent to go to
  if (!hasParent()) return false;
  // tree index becomes the open parenthesis that encloses the current
  treeIndex = bp.enclose(treeIndex);
  // node index is the number of open parenthesis up to the current tree index
  nodeIndex = bp.rank(treeIndex) - 1;
  // successfully moved to parent, return true
  return true;
}

int BpTraversal::degree() {
  int index = treeIndex + 1;
  int count = 0;

  while (index < cjson.tree.size() && cjson.tree.bv[index + 1] == 1) {
    count++;
    index = bp.find_close(index) + 1;
  }

  return count;
}

bool BpTraversal::hasChild() {
  // true if next bit is an open parenthesis
  return treeIndex + 1 < cjson.tree.size() && cjson.tree.bv[treeIndex + 1] == 1;
}

bool BpTraversal::goToChild() {
  // no child to go to
  if (!hasChild()) return false;
  // increment tree index
  treeIndex++;
  // increment node index
  nodeIndex++;
  // successfully moved to child, return true
  return true;
}

bool BpTraversal::hasNextSibling() {
  // true if next bit after closing is an open parenthesis
  int closing = bp.find_close(treeIndex);
  return closing + 1 < cjson.tree.size() && cjson.tree.bv[closing + 1] == 1;
}

bool BpTraversal::goToNextSibling() {
  // no sibling to go to, return false
  if (!hasNextSibling()) return false;
  // tree index becomes the open parenthesis after the current closes
  treeIndex = bp.find_close(treeIndex) + 1;
  // increment node index
  nodeIndex++;
  // successfully moved to sibling, return true
  return true;
}


ostream& operator<<(ostream &o, const BpTraversal &t) {
  return o << "(" << t.nodeIndex << "," << t.treeIndex << ")";
}

#endif