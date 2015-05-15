
#ifndef __PARSER_CPP__
#define __PARSER_CPP__

#include "parser.hpp"
#include "rapidjson/document.h"
#include <iostream>

using namespace std;
using namespace rapidjson;

Parser::Parser(Value &d) {
    loadNames(d);
    loadValues(d);

    for (auto i = names.begin(); i != names.end(); ++i)
        cout << *i << endl;
}

void Parser::loadNames(Value &d) {
    if (d.IsObject())
        for (auto it = d.MemberBegin(); it != d.MemberEnd(); ++it) {
            // cout << it->name.GetString() << " ";
            names.insert(it->name.GetString());
            loadNames(it->value);
        }
    if (d.IsArray()) for (auto it = d.Begin(); it != d.End(); ++it)
        loadNames(*it);
}

void Parser::loadValues(Value &d) {
    // TODO: you just copied this from loadNames, change what has to be updated
    if (d.IsObject())
        for (auto it = d.MemberBegin(); it != d.MemberEnd(); ++it) {
            // cout << it->name.GetString() << " ";
            names.insert(it->name.GetString());
            loadNames(it->value);
        }
    if (d.IsArray()) for (auto it = d.Begin(); it != d.End(); ++it)
        loadNames(*it);
}
#endif

