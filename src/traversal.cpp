#ifndef __TRAVERSAL_CPP__
#define __TRAVERSAL_CPP__

#include "traversal.hpp"
#include "jvalue.hpp"
#include <vector>

using namespace std;
using namespace rapidjson;


template <class SuccinctTree>
TraversalNode Traversal<SuccinctTree>::getCurrentNode() {
  if (nodeIndex == 0) return TraversalNode("", Jvalue(kObject));
  return TraversalNode(cjson.names[cjson.nameList[nodeIndex - 1]],
      cjson.values[nodeIndex - 1]);
}

template <class SuccinctTree>
bool Traversal<SuccinctTree>::hasParent() {
  // true unless tree index is 0
  return treeIndex > 0;
}

template <class SuccinctTree>
bool Traversal<SuccinctTree>::goToParent() {
  // no parent to go to
  if (!hasParent()) return false;
  // tree index becomes the open parenthesis that encloses the current
  treeIndex = bp.enclose(treeIndex);
  // node index is the number of open parenthesis up to the current tree index
  nodeIndex = bp.rank(treeIndex) - 1;
  // successfully moved to parent, return true
  return true;
}

template <class SuccinctTree>
bool Traversal<SuccinctTree>::hasChild() {
  // true if next bit is an open parenthesis
  return treeIndex + 1 < cjson.tree.size() && cjson.tree.bv[treeIndex + 1] == 1;
}

template <class SuccinctTree>
bool Traversal<SuccinctTree>::goToChild() {
  // no child to go to
  if (!hasChild()) return false;
  // increment tree index
  treeIndex++;
  // increment node index
  nodeIndex++;
  // successfully moved to child, return true
  return true;
}

template <class SuccinctTree>
bool Traversal<SuccinctTree>::hasNextSibling() {
  // true if next bit after closing is an open parenthesis
  int closing = bp.find_close(treeIndex);
  return closing + 1 < cjson.tree.size() && cjson.tree.bv[closing + 1] == 1;
}

template <class SuccinctTree>
bool Traversal<SuccinctTree>::goToNextSibling() {
  // no sibling to go to, return false
  if (!hasNextSibling()) return false;
  // tree index becomes the open parenthesis after the current closes
  treeIndex = bp.find_close(treeIndex) + 1;
  // increment node index
  nodeIndex++;
  // successfully moved to sibling, return true
  return true;
}

template <class SuccinctTree>
ostream& operator<<(ostream &o, const Traversal<SuccinctTree> &t) {
  return o << "(" << t.nodeIndex << "," << t.treeIndex << ")";
}


/* Explicit instantiation of needed template classes and methods */
template class Traversal<BpTree>;

template ostream& operator<< (ostream&, const Traversal<BpTree>&);

#endif