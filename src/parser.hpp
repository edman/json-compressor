
#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include "rapidjson/document.h"

#include "encoding.hpp"
#include <iostream>
#include <unordered_set>
#include <string>

using namespace std;
using namespace rapidjson;

class Parser {
public:
    string *keys;
    Encoding *values;

public:
    Parser(Value&);
};

void loadNames(unordered_set<string>, Value&);
void loadValues(unordered_set<Encoding>, Value&);

#endif
