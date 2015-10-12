
#ifndef __PARSER_CPP__
#define __PARSER_CPP__

#include "parser.hpp"
#include "succinct_tree.hpp"
#include "jvalue.hpp"
#include "rapidjson/document.h"
#include <iostream>
#include <map>

using namespace std;
using namespace rapidjson;

Parser::Parser(Value &d, bool debug) {
    if (debug) cout << ".. start new parser" << endl;

    // Load names and values arrays
    loadInfo(d);
    namess.loadBitvector();
    valuess.loadBitvector();

    size = namess.size();
    if (debug) cout << ".. load info" << endl;

    if (debug) cout << "array size " << namess.size() << " " << valuess.size() << endl;

    // Construct a tree from the JSON sctructure
    tree = SuccinctTree(d, size);
    if (debug) cout << ".. construct tree" << endl;
}

void Parser::loadInfo(Value &d) {
    if (d.IsObject()) {
        for (auto it = d.MemberBegin(); it != d.MemberEnd(); ++it) {
            namess.insert(it->name.GetString());
            valuess.insert(Jvalue(it->value));

            loadInfo(it->value);
        }
    }
    else if (d.IsArray())
        for (auto it = d.Begin(); it != d.End(); ++it) {
            namess.insert("");
            valuess.insert(Jvalue(*it));

            loadInfo(*it);
        }
}

int resolveName(const string &name, map<string, int> &nameMap, int &nn) {
    auto nameit = nameMap.find(name);
    if (nameit == nameMap.end()) {
        nameMap[name] = nn++;
        return nn - 1;
    }
    return nameit->second;
}

int resolveValue(Value &d, map<Jvalue, int> &valueMap, int &vn) {
    Jvalue value(d);
    auto valueit = valueMap.find(value);
    if (valueit == valueMap.end()) {
        valueMap[value] = vn++;
        return vn - 1;
    }
    return valueit->second;
}

int type_of(Value &d) {
    if (d.GetType() != 6) return d.GetType();
    return d.IsInt() ? 6 : 7;
}

encode* vectorToArray(vector<encode> &cn) {
    encode *c = new encode[cn.size()];
    for (int i = 0; i < cn.size(); ++i)
        c[i] = cn[i];
    return c;
}

template <typename T> T* mapToArray(map<T, int> &mmap) {
    // Allocate space for array a
    T *a = new T[mmap.size()];
    // Copy the elements from map to array
    for (auto it = mmap.begin(); it != mmap.end(); ++it)
        a[it->second] = it->first;
    return a;
}

bool Parser::operator==(const Parser &rhs) const {
    return size == rhs.size
            && tree == rhs.tree
            && namess == rhs.namess
            && valuess == rhs.valuess;
}

ostream& operator<<(ostream &o, const Parser &p) {
    o<<"Tree "<<p.tree<<endl;
    o<<"Names "<<p.namess<<endl;
    return o<<"Values "<<p.valuess<<endl;
}

ostream& operator<<(ostream &o, const encode &e) {
    o<<"("<<e.name<<","<<e.type;
    return (e.type >= 5 ? o<<","<<e.value : o)<<")";
}

#endif

