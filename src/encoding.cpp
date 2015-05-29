
#ifndef __ENCODEDVALUE_CPP__
#define __ENCODEDVALUE_CPP__

#include "parser.hpp"
#include "rapidjson/document.h"
#include <iostream>

using namespace std;
using namespace rapidjson;

Encoding::Encoding(Value& d) {
    if (d.IsString())
        init(kString, string(d.GetString()));
    else if (d.IsInt())
        init(kInt, d.GetInt());
    else if (d.IsDouble())
        init(kDouble, d.GetDouble());
    else if (d.IsTrue())
        init(kTrue);
    else if (d.IsFalse())
        init(kFalse);
    else if (d.IsNull())
        init(kNull);
}

bool Encoding::operator==(const Encoding &enc) const {
    // Not same type => different
    if (type != enc.type) return false;
    // Equal type with value => compare value
    if (isString()) return vstring == enc.vstring;
    if (isInt()) return vint == enc.vint;
    if (isDouble()) return vdouble == enc.vdouble;
    // Equal type without value => equal
    return true;
}

ostream& operator<<(ostream &o, const Encoding &enc) {
    string t[] = {"Null", "False", "True", "Object", "Array", "String", "Int",
        "Double"};
    o << "(" << t[enc.type];
    if (enc.isString()) o << ",\"" << enc.vstring << "\"";
    if (enc.isInt()) o << "," << enc.vint;
    if (enc.isDouble()) o << "," << enc.vdouble;
    return o << ")";
}

#endif
