#ifndef __TRAVERSAL_NODE_HPP__
#define __TRAVERSAL_NODE_HPP__

#include "cjson.hpp"
#include "jvalue.hpp"
#include "rapidjson/document.h"
#include <sdsl/bp_support_sada.hpp>
#include <string>
#include <vector>

using namespace std;
using namespace rapidjson;


class TraversalNode {
public:
    string name;  // name might be empty if it's the root or an array node
    Jvalue value;
public:
    TraversalNode(string n, Jvalue v) : name(n), value(v) {}
};

ostream& operator<<(ostream &o, const TraversalNode &n);

#endif
