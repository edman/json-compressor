
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

static int counter;

Parser::Parser(Value &d) {
    cout << ".. start" << endl;
    // Load JSON names and values in a hash table
    map<string, int> nameMap;
    map<Jvalue, int> valueMap;

    vector<encode> cn;
    namen = 0, valuen = 0;
    loadInfo(d, nameMap, valueMap, cn);
    cout << ".. load info" << endl;

    // int n = 0; counter = 0;
    // loadNames(nameMap, d, n);
    // loadValues(valueMap, d, (n = 0, n));
    // cout << ".. load values" << endl;

    // Copy the vector and maps to arrays
    codes = vectorToArray(cn);
    names = mapToArray(nameMap);
    values = mapToArray(valueMap);
    size = cn.size();
    // namen = nameMap.size();
    // valuen = valueMap.size();
    cout << ".. copy vector and maps to arrays" << endl;

    // Construct a tree from the JSON sctructure
    // size = counter;
    tree = SuccinctTree(d, size);
    cout << ".. construct tree" << endl;

    cout << ".. (size) " << size;
    cout << ".. (namen) " << namen;
    cout << ".. (valuen) " << valuen << endl;;

    // // Create the list of encodings from the JSON file
    // counter = 0;
    // codes = new encode[size];
    // loadCodes(d, nameMap, valueMap);
    // cout << ".. load codes" << endl;
}

Parser::~Parser() {
    delete[] codes;
    delete[] names;
    delete[] values;
}

void Parser::loadInfo(Value &d, map<string, int> &nameMap,
        map<Jvalue, int> &valueMap, vector<encode> &cn) {
    if (d.IsObject()) {
        for (auto it = d.MemberBegin(); it != d.MemberEnd(); ++it) {
            int name = resolveName(it->name.GetString(), nameMap, namen);
            int type = type_of(it->value);
            int value = (type >= 5 ? resolveValue(it->value, valueMap, valuen) : -1);
            cn.push_back(encode(name, type, value));

            loadInfo(it->value, nameMap, valueMap, cn);
        }
    }
    else if (d.IsArray())
        for (auto it = d.Begin(); it != d.End(); ++it) {
            int name = -1;
            int type = type_of(*it);
            int value = (type >= 5 ? resolveValue(*it, valueMap, valuen) : -1);
            cn.push_back(encode(name, type, value));

            loadInfo(*it, nameMap, valueMap, cn);
        }

    // // load names
    // if (d.IsObject())
    //     for (auto it = d.MemberBegin(); it != d.MemberEnd(); ++it) {
    //         counter++;
    //         string name = it->name.GetString();
    //         if (nameMap.find(name) == nameMap.end())
    //             nameMap[name] = n++;
    //         loadNames(nameMap, it->value, n);
    //     }
    // if (d.IsArray()) for (auto it = d.Begin(); it != d.End(); ++it) {
    //     counter++;
    //     loadNames(nameMap, *it, n);
    // }
    //
    // // load values
    // if (d.IsObject())
    //     for (auto it = d.MemberBegin(); it != d.MemberEnd(); ++it)
    //         loadValues(valueMap, it->value, n);
    // else if (d.IsArray()) for (auto it = d.Begin(); it != d.End(); ++it)
    //     loadValues(valueMap, *it, n);
    // else if (d.IsString() || d.IsNumber()) {
    //     Jvalue value(d);
    //     if (valueMap.find(value) == valueMap.end())
    //         valueMap[value] = n++;
    // }
}

void Parser::loadCodes(Value &d, map<string, int> &nameMap, map<Jvalue, int> &valueMap) {
    if (d.IsObject()) {
        for (auto it = d.MemberBegin(); it != d.MemberEnd(); ++it) {
            int name = nameMap[it->name.GetString()];
            int type = type_of(it->value);
            int value = (type >= 5 ? valueMap[Jvalue(it->value)] : -1);
            codes[counter++] = encode(name, type, value);

            loadCodes(it->value, nameMap, valueMap);
        }
    }
    else if (d.IsArray())
        for (auto it = d.Begin(); it != d.End(); ++it) {
            int name = -1;
            int type = type_of(*it);
            int value = (type >= 5 ? valueMap[Jvalue(*it)] : -1);
            codes[counter++] = encode(name, type, value);

            loadCodes(*it, nameMap, valueMap);
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

void loadNames(map<string, int> &nameMap, Value &d, int &n) {
    if (d.IsObject())
        for (auto it = d.MemberBegin(); it != d.MemberEnd(); ++it) {
            counter++;
            string name = it->name.GetString();
            if (nameMap.find(name) == nameMap.end())
                nameMap[name] = n++;
            loadNames(nameMap, it->value, n);
        }
    if (d.IsArray()) for (auto it = d.Begin(); it != d.End(); ++it) {
        counter++;
        loadNames(nameMap, *it, n);
    }
}

void loadValues(map<Jvalue, int> &valueMap, Value &d, int &n) {
    if (d.IsObject())
        for (auto it = d.MemberBegin(); it != d.MemberEnd(); ++it)
            loadValues(valueMap, it->value, n);
    else if (d.IsArray()) for (auto it = d.Begin(); it != d.End(); ++it)
        loadValues(valueMap, *it, n);
    else if (d.IsString() || d.IsNumber()) {
        Jvalue value(d);
        if (valueMap.find(value) == valueMap.end())
            valueMap[value] = n++;
    }
}

bool Parser::operator==(const Parser &rhs) const {
    if (size != rhs.size || namen != rhs.namen || valuen != rhs.valuen || tree != rhs.tree)
        return false;
    for (int i = 0; i < size; ++i)
        if (codes[i] != rhs.codes[i]) return false;
    for (int i = 0; i < namen; ++i)
        if (names[i] != rhs.names[i]) return false;
    for (int i = 0; i < valuen; ++i)
        if (values[i] != rhs.values[i]) return false;
    return true;
}


ostream& operator<<(ostream &o, const encode &e) {
    o<<"("<<e.name<<","<<e.type;
    return (e.type >= 5 ? o<<","<<e.value : o)<<")";
}

#endif

