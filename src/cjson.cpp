
#ifndef __CJSON_CPP__
#define __CJSON_CPP__

#include "cjson.hpp"
#include "bp_tree.hpp"
#include "df_tree.hpp"
#include "jval.hpp"
#include "util.hpp"
#include "rapidjson/document.h"
#include <iostream>
#include <unordered_map>
#include <map>

using namespace std;
using namespace rapidjson;

template <class SuccinctTree>
Cjson<SuccinctTree>::Cjson(Value &d) {
    // Load names table and values packed array
    unordered_map<string, int> nameTable;
    loadInfo(d, nameTable);
    // Load values bitmap index
    values.loadBitmap();

    size = values.size();

    // Reduce excess vector capacity
    names.shrink_to_fit();
    stringValues.shrink_to_fit();

    // Construct a tree from the JSON sctructure
    tree = SuccinctTree(d, size);
}

template <class SuccinctTree>
void Cjson<SuccinctTree>::loadInfo(Value &d, unordered_map<string,
        int> &nameTable, uint nameId) {
    resolveValue(d, nameId);
    if (d.IsObject())
        for (auto it = d.MemberBegin(); it != d.MemberEnd(); ++it) {
            uint nameId = resolveNameId(it->name.GetString(), nameTable);
            loadInfo(it->value, nameTable, nameId);
        }
    else if (d.IsArray())
        for (auto it = d.Begin(); it != d.End(); ++it) {
            loadInfo(*it, nameTable);
        }
}

template <class SuccinctTree>
uint Cjson<SuccinctTree>::resolveNameId(const string &name,
        unordered_map<string, int> &nameTable) {
    auto it = nameTable.find(name);
    if (it == nameTable.end()) {
        uint nameId = names.size();
        nameTable[name] = nameId;
        names.push_back(string_to_cstr(name));
        return nameId;
    }
    return it->second;
}

template <class SuccinctTree>
void Cjson<SuccinctTree>::resolveValue(Value &d, uint nameId) {
    // no name
    if (isMaxUint(nameId)) {
        // no data
        if (d.IsNull())
            values.append<Jval>(Jval(Jval::TYPE_NULL));
        else if (d.IsObject())
            values.append<Jval>(Jval(Jval::TYPE_OBJECT));
        else if (d.IsArray())
            values.append<Jval>(Jval(Jval::TYPE_ARRAY));
        else if (d.IsFalse())
            values.append<Jval>(Jval(Jval::TYPE_FALSE));
        else if (d.IsTrue())
            values.append<Jval>(Jval(Jval::TYPE_TRUE));
        // with data (string)
        else if (d.IsString()) {
            uint strId = stringValues.size();
            stringValues.push_back(string_to_cstr(d.GetString()));
            values.append<StringJval>(StringJval(strId));
        }
        // with data (double)
        else if (d.IsDouble()) {
            values.append<DoubleJval>(DoubleJval(d.GetDouble()));
        }
        // with data (integer)
        else if (d.IsNumber()) { // IsNumber returns true for doubles as well
            int num = d.GetInt64();
            if (intInChar(num))
                values.append<CharJval>(CharJval(num));
            else if (intInShort(num))
                values.append<ShortJval>(ShortJval(num));
            else
                values.append<IntJval>(IntJval(num));
        }
        else { // assert we don't fall into an unknown type
            assert(false);
        }
    }
    // with name
    else {
        // no data
        if (d.IsNull())
            values.append<NamedJval>(NamedJval(Jval::TYPE_NULL, nameId));
        else if (d.IsObject())
            values.append<NamedJval>(NamedJval(Jval::TYPE_OBJECT, nameId));
        else if (d.IsArray())
            values.append<NamedJval>(NamedJval(Jval::TYPE_ARRAY, nameId));
        else if (d.IsFalse())
            values.append<NamedJval>(NamedJval(Jval::TYPE_FALSE, nameId));
        else if (d.IsTrue())
            values.append<NamedJval>(NamedJval(Jval::TYPE_TRUE, nameId));
        // with data (string)
        else if (d.IsString()) {
            uint strId = stringValues.size();
            stringValues.push_back(string_to_cstr(d.GetString()));
            values.append<NamedStringJval>(NamedStringJval(nameId, strId));
        }
        // with data (double)
        else if (d.IsDouble()) {
            values.append<NamedDoubleJval>(NamedDoubleJval(nameId,
                    d.GetDouble()));
        }
        // with data (integer)
        else if (d.IsNumber()) { // IsNumber returns true for doubles as well
            int num = d.GetInt64();
            if (intInChar(num))
                values.append<NamedCharJval>(NamedCharJval(nameId, num));
            else if (intInShort(num))
                values.append<NamedShortJval>(NamedShortJval(nameId, num));
            else
                values.append<NamedIntJval>(NamedIntJval(nameId, num));
        }
        else { // assert we don't fall into an unknown type
            assert(false);
        }
    }
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
const Jval& Cjson<SuccinctTree>::operator[](const uint index) const {
    return values.get<Jval>(index);
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
    o<<"Tree: "<<p.tree<<endl;
    o<<"Names ";
    for (int i = 0; i < p.names.size(); i++)
        o<<i<<":'"<<p.names[i]<<"'"; o<<endl;
    o<<"Values ";
    for (int i = 0; i < p.values.size(); i++) {
        const Jval &v = p[i];
        o<<":"<<v; if (v.isString()) o<<p.stringValues[v.getStringIndex()];
    }
    return o<<endl;
}


/* Explicit instantiation of needed template classes and methods */
template class Cjson<BpTree>;
template class Cjson<DfTree>;

template ostream& operator<< (ostream&, const Cjson<BpTree>&);
template ostream& operator<< (ostream&, const Cjson<DfTree>&);


#endif
