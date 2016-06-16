
#ifndef __CJSON_HPP__
#define __CJSON_HPP__

#include "util.hpp"
#include "jvalue.hpp"
#include "bitmap_index.hpp"
#include "rapidjson/document.h"
#include <iostream>
#include <unordered_map>
#include <string>

using namespace std;
using namespace rapidjson;

template <class SuccinctTree>
class Cjson {
public:
    int size;
    SuccinctTree tree;
    vector<char*> names;
    BitmapIndex<Jvalue> values;

public:
    /* Constructors and destructor */
    Cjson(Value &d, bool debug=false);
    Cjson(int s, SuccinctTree st, vector<char*> nm, BitmapIndex<Jvalue> vl): size(s), tree(st), names(nm), values(vl) {}

    /* Methods */
    void loadInfo(Value &d, unordered_map<string, int> &nt);
    uint resolveNameId(const string &n, unordered_map<string, int> &nt);

    void removeValues();

    /* Operators */
    bool operator==(const Cjson &rhs) const;
};

int resolveName(const string&, map<string, int>&, int&);
int resolveValue(Value&, map<Jvalue, int>&, int&);
int type_of(Value &d);
template <typename T> T* mapToArray(map<T, int> &mmap);

#endif
