
#ifndef __Encoding_HPP__
#define __Encoding_HPP__

#include "rapidjson/document.h"

#include <iostream>
#include <unordered_set>
#include <string>
#include <vector>

using namespace std;
using namespace rapidjson;

enum types { kNull, kFalse, kTrue, kObject, kArray, kString, kInt, kDouble };

class Encoding {
public:
    int keyi;
    types type;
    // { "String", "Int", "Double" };
    string vstring;
    int vint;
    double vdouble;

public:
    /* Constructors */
    Encoding(Value&);
    /* Initialization functions */
    void init(types t) { type = t; if (hasValue()) cout << "Value expected" << endl; }
    void init(types t, string v) { type = t; vstring = v; }
    void init(types t, int v) { type = t; vint = v; }
    void init(types t, double v) { type = t; vdouble = v; }

    /* Methods */
    bool hasValue() const { return type >= kString && type <= kDouble; }
    bool isString() const { return type == kString; }
    bool isInt() const { return type == kInt; }
    bool isDouble() const { return type == kDouble; }

    /* Operators */
    bool operator==(const Encoding &enc) const;
    friend ostream& operator<<(ostream &o, const Encoding &enc);
};


namespace std {
    template<> struct hash<Encoding> {
        size_t operator()(const Encoding& enc) const {
            size_t h1 = hash<int>()(enc.type), h2;
            if (!enc.hasValue())
                return h1;
            if (enc.isString())
                h2 = hash<string>()(enc.vstring);
            if (enc.isInt())
                h2 = hash<int>()(enc.vint);
            if (enc.isDouble())
                h2 = hash<double>()(enc.vdouble);
            return h1 ^ (h2 << 1);
        }
    };
}

#endif
