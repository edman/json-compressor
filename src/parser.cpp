
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

Parser::Parser(Value &d) {
    // Construct a tree from the json sctructure
    tree = SuccinctTree(d);

    // Load JSON names and values in a hash table
    map<string, int> nameMap;
    map<Jvalue, int> valueMap;
    int n = 0;
    loadNames(nameMap, d, n);
    loadValues(valueMap, d, (n = 0, n));

    // Copy the maps to arrays
    names = mapToArray(nameMap);
    values = mapToArray(valueMap);
    namen = nameMap.size();
    valuen = valueMap.size();

    // Create the list of encodings from the Json file
}

Parser::~Parser() {
    delete[] names;
    delete[] values;
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
            string name = it->name.GetString();
            if (nameMap.find(name) == nameMap.end())
                nameMap[name] = n++;
            loadNames(nameMap, it->value, n);
        }
    if (d.IsArray()) for (auto it = d.Begin(); it != d.End(); ++it)
        loadNames(nameMap, *it, n);
}

void loadValues(map<Jvalue, int> &valueMap, Value &d, int &n) {
    if (d.IsObject())
        for (auto it = d.MemberBegin(); it != d.MemberEnd(); ++it)
            loadValues(valueMap, it->value, n);
    else if (d.IsArray()) for (auto it = d.Begin(); it != d.End(); ++it)
        loadValues(valueMap, *it, n);
    else {
        Jvalue value(d);
        if (valueMap.find(value) == valueMap.end())
            valueMap[value] = n++;
    }
}

#endif
