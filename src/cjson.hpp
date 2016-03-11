
#ifndef __CJSON_HPP__
#define __CJSON_HPP__

#include "succinct_tree.hpp"
#include "jvalue.hpp"
#include "bitmap_index.hpp"
#include "rapidjson/document.h"
#include <iostream>
#include <unordered_map>
#include <string>

using namespace std;
using namespace rapidjson;

class Cjson {
public:
    int size;
    SuccinctTree tree;
    vector<string> names;
    vector<int> nameList;
    BitmapIndex<Jvalue> values;

public:
    Cjson(Value &d, bool debug=false);
    Cjson(int s, SuccinctTree st, vector<string> nm, vector<int> nl, BitmapIndex<Jvalue> vl): size(s), tree(st), names(nm), nameList(nl), values(vl) {}

    void loadInfo(Value &d, unordered_map<string, int> &nt);
    int resolveNameId(const string &n, unordered_map<string, int> &nt);

    bool operator==(const Cjson &rhs) const;
};

int resolveName(const string&, map<string, int>&, int&);
int resolveValue(Value&, map<Jvalue, int>&, int&);
int type_of(Value &d);
template <typename T> T* mapToArray(map<T, int> &mmap);

#endif
