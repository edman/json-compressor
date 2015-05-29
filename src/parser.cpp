
#ifndef __PARSER_CPP__
#define __PARSER_CPP__

#include "parser.hpp"
#include "encoding.hpp"
#include "rapidjson/document.h"
#include <iostream>

using namespace std;
using namespace rapidjson;

Parser::Parser(Value &d) {
    unordered_set<string> nameSet;
    unordered_set<Encoding> valueSet;
    loadNames(nameSet, d);
    loadValues(valueSet, d);
}

void loadNames(unordered_set<string> nameSet, Value &d) {
    if (d.IsObject())
        for (auto it = d.MemberBegin(); it != d.MemberEnd(); ++it) {
            nameSet.insert(it->name.GetString());
            loadNames(nameSet, it->value);
        }
    if (d.IsArray()) for (auto it = d.Begin(); it != d.End(); ++it)
        loadNames(nameSet, *it);
}

void loadValues(unordered_set<Encoding> valueSet, Value &d) {
    if (d.IsObject())
        for (auto it = d.MemberBegin(); it != d.MemberEnd(); ++it)
            loadValues(valueSet, it->value);
    else if (d.IsArray()) for (auto it = d.Begin(); it != d.End(); ++it)
        loadValues(valueSet, *it);
    else valueSet.insert(Encoding(d));
}

#endif
