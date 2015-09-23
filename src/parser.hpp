
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

    void loadCodes(Value &d, map<string, int> nameMap, map<Jvalue, int> valueMap);

    bool operator==(const Parser &rhs) const;

	#ifdef COMPRESS
	void zlibCompressN(map<string, int> *, ofstream *);
	void zlibCompressV(map<Jvalue, int> *, ofstream *);
	void zlibDecompressN(ifstream *, map<string, int> *);
	void zlibDecompressV(ifstream *, map<Jvalue, int> *);
	#endif
};

int type_of(Value &d);
template <typename T> T* mapToArray(map<T, int> &mmap);
void loadNames(map<string, int> &nameMap, Value &d, int &n);
void loadValues(map<Jvalue, int> &valueMap, Value &d, int &n);

ostream& operator<<(ostream &o, const encode &e);

#endif

