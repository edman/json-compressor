
#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include "rapidjson/document.h"

#include <set>
#include <string>
#include <vector>

using namespace std;
using namespace rapidjson;

class Parser {
public:
    set<string> names;
    vector<string> values;

public:
    Parser(Value&);

private:
    void loadNames(Value&);
    void loadValues(Value&);
};

#endif

