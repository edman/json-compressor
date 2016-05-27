#ifndef __TRAVERSAL_NODE_CPP__
#define __TRAVERSAL_NODE_CPP__

#include "traversal_node.hpp"

using namespace std;


ostream& operator<<(ostream &o, const TraversalNode &n) {
    return o<<"\""<<n.name<<"\":"<<n.value;
}

#endif
