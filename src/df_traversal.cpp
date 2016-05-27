#ifndef __DFTRAVERSAL_CPP__
#define __DFTRAVERSAL_CPP__

#include "df_traversal.hpp"
#include "df_tree.hpp"
#include "jvalue.hpp"
#include <iostream>

using namespace std;


TraversalNode DfTraversal::getCurrentNode() {
  if (nodeIndex == 0) return TraversalNode("", Jvalue::OBJECT_VAL);
  return TraversalNode(cjson->names[cjson->nameList[nodeIndex - 1]],
      cjson->values[nodeIndex - 1]);
}

inline bool DfTraversal::hasParent() {
  // true unless tree index is 1
  return treeIndex > 1;
}

bool DfTraversal::goToParent() {
  // no parent to go to
  if (!hasParent()) return false;

  // the open of the previous position lies within the parent description
  int index = bp.find_open(treeIndex - 1);
  // number of 0s up to here is the node index
  nodeIndex = rank_close(index + 1);
  updateTreeIndexFromNodeIndex();

  return true;
}

inline int DfTraversal::degree() {
  // close of nodeIndex + 1 corresponds to the 0 bit of the current node
  // the difference between that 0 bit and treeIndex is the degree
  return select_close(nodeIndex + 1) - treeIndex;
}

bool DfTraversal::hasChild() {
  return degree() > 0;
}

bool DfTraversal::goToChild(int i) {
  // no child to go to, return false
  int d = degree();
  if (d == 0 || i >= d) return false;

  // jump forward (d - i) posistions, find the close, the child is after that
  treeIndex = bp.find_close(treeIndex + d - (i + 1)) + 1;
  // update node index
  updateNodeIndexFromTreeIndex();

  // cout << "ni " << nodeIndex << " ti " << treeIndex << endl;
  // cout << "rank " << rank_close(treeIndex - 1) << endl;
  // for (int i = 0; i <= cjson.tree.bv.size(); ++i)
  //   cout << "i " << i << " rank " << rank_close(i) << endl;
  // for (int i = 1; i <= rank_close(cjson.tree.bv.size()); ++i)
  //   cout << "i " << i << " sel " << select_close(i) << endl;

  // successfully moved to child, return true
  return true;
}

// Return next sibling index if existent, otherwise return 0
int DfTraversal::nextSiblingIndex() {
  // no sibling to go to, return false
  if (!hasParent()) return 0;

  // the open of the previous position corresponds to this node in the parent description
  int siblingIndex = bp.find_open(treeIndex - 1);
  // the position before that would correspond to the next sibling
  siblingIndex = siblingIndex - 1;
  // as long as we're not at the first artificial bit and it's an open
  return siblingIndex > 0 && cjson->tree.bv[siblingIndex] == 1 ? siblingIndex : 0;
}

bool DfTraversal::hasNextSibling() {
  return nextSiblingIndex();
}

bool DfTraversal::goToNextSibling() {
  int siblingIndex = nextSiblingIndex();
  // no sibling to go to, return false
  if (!siblingIndex) return false;

  // sibling index is one position after the close of this open parenthesis
  treeIndex = bp.find_close(siblingIndex) + 1;
  // update node index
  updateNodeIndexFromTreeIndex();

  return true;
}

void DfTraversal::updateNodeIndexFromTreeIndex() {
  // node index is the number of 0s up to the previous position
  nodeIndex = rank_close(treeIndex);
}

void DfTraversal::updateTreeIndexFromNodeIndex() {
  // tree index is one position after the nodeIndex-th 0, or 1 if nodeIndex is 0
  treeIndex = nodeIndex == 0 ? 1 : select_close(nodeIndex) + 1;
}


ostream& operator<<(ostream &o, const DfTraversal &t) {
  return o << "(" << t.nodeIndex << "," << t.treeIndex << ")";
}

#endif