
#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include "rapidjson/document.h"

#include "encodedvalue.hpp"
#include <iostream>
#include <unordered_set>
#include <string>

using namespace std;
using namespace rapidjson;

class Parser {
public:
    unordered_set<string> names;
    unordered_set<EncodedValue> values;

public:
    Parser(Value&);

private:
    void loadNames(Value&);
    void loadValues(Value&);
};

#endif

