
#ifndef __CJSON_CPP__
#define __CJSON_CPP__

#include "cjson.hpp"
#include "bp_tree.hpp"
#include "df_tree.hpp"
#include "jvalue.hpp"
#include "util.hpp"
#include "rapidjson/document.h"
#include <iostream>
#include <unordered_map>
#include <map>

using namespace std;
using namespace rapidjson;

template <class SuccinctTree>
Cjson<SuccinctTree>::Cjson(Value &d, bool debug) {
    if (debug) cout << ".. start new cjson" << endl;

    // Load names and values arrays
    unordered_map<string, int> nameTable;
    loadInfo(d, nameTable);
    // Load the values bitmap
    values.loadBitmap();

    size = values.size();
    if (debug) cout << ".. load info" << endl;
    if (debug) cout << "names size " << names.size() << " values size " << values.size() << endl;

    // Reduce excess vector capacity
    names.shrink_to_fit();

    // Construct a tree from the JSON sctructure
    tree = SuccinctTree(d, size);
    if (debug) cout << ".. construct tree" << endl;
}

template <class SuccinctTree>
void Cjson<SuccinctTree>::loadInfo(Value &d, unordered_map<string, int> &nameTable) {
    if (d.IsObject()) {
        for (auto it = d.MemberBegin(); it != d.MemberEnd(); ++it) {
            uint nameId = resolveNameId(it->name.GetString(), nameTable);
            values.append<Jvalue>(Jvalue::factory(it->value, nameId));

            loadInfo(it->value, nameTable);
        }
    }
    else if (d.IsArray())
        for (auto it = d.Begin(); it != d.End(); ++it) {
            uint nameId = resolveNameId("", nameTable);
            values.append<Jvalue>(Jvalue::factory(*it, nameId));

            loadInfo(*it, nameTable);
        }
}

template <class SuccinctTree>
uint Cjson<SuccinctTree>::resolveNameId(const string &name, unordered_map<string, int> &nameTable) {
    auto it = nameTable.find(name);
    if (it == nameTable.end()) {
        uint nameId = names.size();
        nameTable[name] = nameId;
        names.push_back(string_to_cstr(name));
        return nameId;
    }
    return it->second;
}

// int resolveName(const string &name, map<string, int> &nameMap, int &nn) {
//     auto nameit = nameMap.find(name);
//     if (nameit == nameMap.end()) {
//         nameMap[name] = nn++;
//         return nn - 1;
//     }
//     return nameit->second;
// }

// int resolveValue(Value &d, map<Jvalue, int> &valueMap, int &vn) {
//     Jvalue value = Jvalue::factory(d);
//     auto valueit = valueMap.find(value);
//     if (valueit == valueMap.end()) {
//         valueMap[value] = vn++;
//         return vn - 1;
//     }
//     return valueit->second;
// }

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

template <class SuccinctTree>
bool Cjson<SuccinctTree>::operator==(const Cjson &rhs) const {
    if (names.size() != rhs.names.size()) return false;
    for (int i = 0; i < names.size(); ++i)
        if (strcmp(names[i], rhs.names[i])) return false;
    return size == rhs.size
            && tree == rhs.tree
            // && names == rhs.names
            && values == rhs.values;
}

template <class SuccinctTree>
ostream& operator<<(ostream &o, const Cjson<SuccinctTree> &p) {
    o<<"Tree "<<p.tree<<endl;
    o<<"Names "; for (int i = 0; i < p.names.size(); i++) o<<i<<":'"<<p.names[i]<<"'"; o<<endl;
    return o<<"Values "<<p.values<<endl;
}


/* Explicit instantiation of needed template classes and methods */
template class Cjson<BpTree>;
template class Cjson<DfTree>;

template ostream& operator<< (ostream&, const Cjson<BpTree>&);
template ostream& operator<< (ostream&, const Cjson<DfTree>&);


#endif
