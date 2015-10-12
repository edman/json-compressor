
#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include "succinct_tree.hpp"
#include "jvalue.hpp"
#include "bitmap_index.hpp"
#include "rapidjson/document.h"
#include <iostream>
#include <map>
#include <string>

using namespace std;
using namespace rapidjson;

struct encode {
    int name, type, value;
    encode(int n=-1, int t=-1, int v=-1): name(n), type(t), value(v) {}
    void print() { cout<<"("<<name<<","<<type<<","<<value<<")"; }
    bool operator==(const encode &rhs) const { return name == rhs.name &&
        type == rhs.type && (type < 5 || value == rhs.value); }
    bool operator!=(const encode &rhs) const { return !(*this == rhs); }
};

class Parser {
public:
    int size;
    SuccinctTree tree;
    BitmapIndex<string> namess;
    BitmapIndex<Jvalue> valuess;

public:
    Parser(Value &d, bool debug=false);
    Parser(int s, SuccinctTree st, BitmapIndex<string> nm, BitmapIndex<Jvalue> vl): size(s), tree(st), namess(nm), valuess(vl) {}

    void loadInfo(Value &d);

    bool operator==(const Parser &rhs) const;
};

int resolveName(const string&, map<string, int>&, int&);
int resolveValue(Value&, map<Jvalue, int>&, int&);
int type_of(Value &d);
encode* vectorToArray(vector<encode>&);
template <typename T> T* mapToArray(map<T, int> &mmap);

ostream& operator<<(ostream &o, const encode &e);

#endif
