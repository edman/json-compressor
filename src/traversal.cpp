#ifndef __TRAVERSAL_CPP__
#define __TRAVERSAL_CPP__

#include "traversal.hpp"
#include "bp_tree.hpp"
#include "df_tree.hpp"

using namespace std;

template <class SuccinctTree>
ostream& operator<<(ostream &o, const Traversal<SuccinctTree> &t) {
  return o << "(" << t.nodeIndex << "," << t.treeIndex << ")";
}

/* Explicit instantiation of needed template classes and methods */
// template class Traversal<BpTree>;

template ostream& operator<< (ostream&, const Traversal<BpTree>&);
template ostream& operator<< (ostream&, const Traversal<DfTree>&);

#endif
