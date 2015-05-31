
#ifndef __PARSER_HPP__
#define __PARSER_HPP__

#include "succinct_tree.hpp"
#include "jvalue.hpp"
#include "rapidjson/document.h"
#include <iostream>
#include <map>
#include <string>

using namespace std;
using namespace rapidjson;

struct code {
    int name;
    int value;
};

class Parser {
public:
    SuccinctTree tree;
    string *names;
    Jvalue *values;
    code *codes;
    int namen, valuen, coden;

public:
    Parser(Value&);
    ~Parser();
};

template <typename T> T* mapToArray(map<T, int> &mmap);
void loadNames(map<string, int> &nameMap, Value &d, int &n);
void loadValues(map<Jvalue, int> &valueMap, Value &d, int &n);

#endif
