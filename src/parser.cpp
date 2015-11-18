
#ifndef __PARSER_CPP__
#define __PARSER_CPP__

#include "parser.hpp"
#include "succinct_tree.hpp"
#include "jvalue.hpp"
#include "rapidjson/document.h"
#include <iostream>
#include <unordered_map>
#include <map>

using namespace std;
using namespace rapidjson;

Parser::Parser(Value &d, bool debug) {
    if (debug) cout << ".. start new parser" << endl;

    // Load names and values arrays
    unordered_map<string, int> nameTable;
    loadInfo(d, nameTable);
    valuess.loadBitvector();

    size = valuess.size();
    if (debug) cout << ".. load info" << endl;

    if (debug) cout << "array size " << namess.size() << " " << valuess.size() << endl;

    // Construct a tree from the JSON sctructure
    tree = SuccinctTree(d, size);
    if (debug) cout << ".. construct tree" << endl;
}

void Parser::loadInfo(Value &d, unordered_map<string, int> &nameTable) {
    if (d.IsObject()) {
        for (auto it = d.MemberBegin(); it != d.MemberEnd(); ++it) {
            int nameId = resolveNameId(it->name.GetString(), nameTable);
            nameList.push_back(nameId);
            valuess.insert(Jvalue(it->value));

            loadInfo(it->value, nameTable);
        }
    }
    else if (d.IsArray())
        for (auto it = d.Begin(); it != d.End(); ++it) {
            nameList.push_back(resolveNameId("", nameTable));
            valuess.insert(Jvalue(*it));

            loadInfo(*it, nameTable);
        }
}

int Parser::resolveNameId(const string &name, unordered_map<string, int> &nameTable) {
    auto it = nameTable.find(name);
    if (it == nameTable.end()) {
        int nameId = namess.size();
        nameTable[name] = nameId;
        namess.push_back(name);
        return nameId;
    }
    return it->second;
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
    o<<"Names "; for (int i = 0; i < p.namess.size(); i++) o<<i<<":'"<<p.namess[i]<<"'"; o<<endl;
    return o<<"Values "<<p.valuess<<endl;
}

#endif

