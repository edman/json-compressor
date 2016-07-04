
#ifndef __CJSON_HPP__
#define __CJSON_HPP__

#include "util.hpp"
#include "jval.hpp"
#include "bitmap_index.hpp"
#include "bp_tree.hpp"
#include "df_tree.hpp"
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
    BitmapIndex values;
    vector<char*> stringValues;

public:
    /* Constructors and destructor */
    Cjson(Value &d);
    Cjson(int s, SuccinctTree &st, vector<char*> &nm, BitmapIndex &vl,
            vector<char*> &sv) : size(s), tree(st), names(nm),
                                 values(std::move(vl)), stringValues(sv) {}
    // TODO: free all names and string values on destructor

    /* Operators */
    const Jval& operator[](const uint index) const;
    bool operator==(const Cjson &rhs) const;

private:
    /* Private helper methods */
    void loadInfo(Value &d, unordered_map<string, int> &nt, uint nId=MAX_UINT);
    uint resolveNameId(const string &n, unordered_map<string, int> &nt);
    void resolveValue(Value &d, uint nameId=MAX_UINT);
};

template <typename T> T* mapToArray(map<T, int> &mmap);

/* Typedefs and template instantiations */
typedef Cjson<BpTree> CjsonBp;
typedef Cjson<DfTree> CjsonDf;

#endif
