#ifndef __DFTRAVERSAL_CPP__
#define __DFTRAVERSAL_CPP__

#include "df_traversal.hpp"
#include "df_tree.hpp"
#include "jvalue.hpp"
#include <iostream>

using namespace std;


TraversalNode DfTraversal::getCurrentNode() {
  if (nodeIndex == 0) return TraversalNode("", Jvalue::OBJECT);
  return TraversalNode(cjson.names[cjson.nameList[nodeIndex - 1]],
      cjson.values[nodeIndex - 1]);
}

bool DfTraversal::hasParent() {
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

int DfTraversal::degree() {
  // if (nodeIndex + 1 == cjson.tree.N)
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

bool DfTraversal::hasNextSibling() {
  // no sibling to go to, return false
  if (!hasParent()) return false;

  // the open of the previous position corresponds to this node in the parent description
  int index = bp.find_open(treeIndex - 1);
  // the position before that would correspond to the next sibling
  index = index - 1;
  // as long as we're not at the first artificial bit and it's an open
  return index > 0 && cjson.tree.bv[index == 1];
}

bool DfTraversal::goToNextSibling() {
  // if we have no parent then there's no sibling to go to, return false
  if (!hasParent()) return false;

  // the open of the previous position corresponds to this node in the parent description
  int index = bp.find_open(treeIndex - 1);
  // the position before that would correspond to the next sibling
  index = index - 1;
  // as long as we're not at the first artificial bit and it's an open
  if (index > 0 && cjson.tree.bv[index == 1]) return false;

  // sibling index is one position after the close of this open parenthesis
  treeIndex = bp.find_close(index) + 1;
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