
#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include "succinct_tree.hpp"
#include "jvalue.hpp"
#include "bitmap_index.hpp"
#include "rapidjson/document.h"
#include <iostream>
#include <unordered_map>
#include <string>

using namespace std;
using namespace rapidjson;

class Parser {
public:
    int size;
    SuccinctTree tree;
    vector<string> namess;
    vector<int> nameList;
    BitmapIndex<Jvalue> valuess;

public:
    Parser(Value &d, bool debug=false);
    Parser(int s, SuccinctTree st, vector<string> nm, vector<int> nl, BitmapIndex<Jvalue> vl): size(s), tree(st), namess(nm), nameList(nl), valuess(vl) {}

    void loadInfo(Value &d, unordered_map<string, int> &nt);
    int resolveNameId(const string &n, unordered_map<string, int> &nt);

    bool operator==(const Parser &rhs) const;
};

int resolveName(const string&, map<string, int>&, int&);
int resolveValue(Value&, map<Jvalue, int>&, int&);
int type_of(Value &d);
template <typename T> T* mapToArray(map<T, int> &mmap);

#endif
