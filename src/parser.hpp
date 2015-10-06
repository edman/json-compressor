
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
    sdsl::bit_vector treeBv;

    class ParseStreams
    {
    public:
    	sdsl::bit_vector nameBv, stringBv, numBv, boolBv;
    	/* TODO: How to consider different types of either int or double? */
    	std::stringstream nameStr, stringStr, numStr;
    };

public:
    Parser(Value&);
    Parser(int size, int namen, int valuen, SuccinctTree tree, encode *codes,
            string *names, Jvalue *values): size(size), namen(namen),
            valuen(valuen), tree(tree), codes(codes), names(names),
            values(values) { }
    ~Parser();

    bool operator==(const Parser &rhs) const;

	void zlibCompress(Parser::ParseStreams &);
	void docParse(Parser::ParseStreams &pst, Value &d, sdsl::bit_vector &b, int &n);
};

int type_of(Value &d);
template <typename T> T* mapToArray(map<T, int> &mmap);

ostream& operator<<(ostream &o, const encode &e);

#endif

