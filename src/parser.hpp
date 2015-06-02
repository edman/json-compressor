
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

struct encode {
    int name, type, value;
    encode(int n=-1, int t=-1, int v=-1): name(n), type(t), value(v) {}
    void print() { cout<<"("<<name<<","<<type<<","<<value<<")"; }
    bool operator==(const encode &rhs) const { return name == rhs.name &&
        type == rhs.type && (type < 5 || value == rhs.value); }
    bool operator!=(const encode &rhs) const { return !(*this == rhs); }
};

class Parser {
public:
    int size, namen, valuen;
    SuccinctTree tree;
    encode *codes;
    string *names;
    Jvalue *values;

public:
    Parser(Value&);
    Parser(int size, int namen, int valuen, SuccinctTree tree, encode *codes,
            string *names, Jvalue *values): size(size), namen(namen),
            valuen(valuen), tree(tree), codes(codes), names(names),
            values(values) { }
    ~Parser();

    void loadInfo(Value&, map<string, int>&, map<Jvalue, int>&,
            vector<encode>&);
    void loadCodes(Value&, map<string, int>&, map<Jvalue, int>&);

    bool operator==(const Parser &rhs) const;
};

int resolveName(const string&, map<string, int>&, int&);
int resolveValue(Value&, map<Jvalue, int>&, int&);
int type_of(Value &d);
encode* vectorToArray(vector<encode>&);
template <typename T> T* mapToArray(map<T, int> &mmap);
void loadNames(map<string, int> &nameMap, Value &d, int &n);
void loadValues(map<Jvalue, int> &valueMap, Value &d, int &n);

ostream& operator<<(ostream &o, const encode &e);

#endif

