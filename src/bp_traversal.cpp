#ifndef __BPTRAVERSAL_CPP__
#define __BPTRAVERSAL_CPP__

#include "bp_traversal.hpp"
#include "bp_tree.hpp"
#include "jvalue.hpp"
#include <iostream>

using namespace std;


TraversalNode BpTraversal::getCurrentNode() {
  if (nodeIndex == 0) return TraversalNode("", Jvalue::factory(types::kObject, -1));
  Jvalue &value = cjson->values.get<Jvalue>(nodeIndex - 1);
  return TraversalNode(cjson->names[value.nameId], value);
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
  // update node index
  updateNodeIndexFromTreeIndex();
  // successfully moved to parent, return true
  return true;
}

int BpTraversal::degree() {
  int index = treeIndex + 1;
  int count = 0;

  while (index < cjson->tree.size() && cjson->tree.bv[index + 1] == 1) {
    count++;
    index = bp.find_close(index) + 1;
  }

  return count;
}

bool BpTraversal::hasChild() {
  // true if next bit is an open parenthesis
  return treeIndex + 1 < cjson->tree.size() && cjson->tree.bv[treeIndex + 1] == 1;
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

// Return next sibling index if existent, otherwise return 0
int BpTraversal::nextSiblingIndex() {
  // true if next bit after closing is an open parenthesis
  int siblingIndex = bp.find_close(treeIndex) + 1;
  return siblingIndex < cjson->tree.size() && cjson->tree.bv[siblingIndex] == 1 ? siblingIndex : 0;
}

bool BpTraversal::hasNextSibling() {
  return nextSiblingIndex();
}

bool BpTraversal::goToNextSibling() {
  int siblingIndex = nextSiblingIndex();
  // no sibling to go to, return false
  if (!siblingIndex) return false;
  // tree index becomes the open parenthesis after the current closes
  treeIndex = siblingIndex;
  // update node index
  updateNodeIndexFromTreeIndex();
  // successfully moved to sibling, return true
  return true;
}

void BpTraversal::updateNodeIndexFromTreeIndex() {
  // node index is the number of open parenthesis up to the current tree index
  // (indexed from [0,n-1], so we decrement that value by 1)
  nodeIndex = bp.rank(treeIndex) - 1;
}



ostream& operator<<(ostream &o, const BpTraversal &t) {
  return o << "(" << t.nodeIndex << "," << t.treeIndex << ")";
}

#endif