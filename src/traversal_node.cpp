#ifndef __TRAVERSAL_NODE_CPP__
#define __TRAVERSAL_NODE_CPP__

#include "traversal_node.hpp"

using namespace std;


ostream& operator<<(ostream &o, const TraversalNode &n) {
    o<<"\""<<n.name<<"\":";
    if (n.value.isObject()) o<<"object";
    else if (n.value.isArray()) o<<"array";
    else if (n.value.isNull()) o<<"null";
    else if (n.value.isTrue()) o<<"True";
    else if (n.value.isFalse()) o<<"False";
    else if (n.value.isString()) o<<"\""<<n.value.getString()<<"\"";
    else if (n.value.isInt()) o<<n.value.getInt();
    else if (n.value.isDouble()) o<<n.value.getDouble();
    return o;
}

#endif
