#ifndef __JVALUE_CPP__
#define __JVALUE_CPP__

#include "jvalue.hpp"
#include "cjson.hpp"
#include "rapidjson/document.h"
#include <iostream>

using namespace std;
using namespace rapidjson;

Jvalue::Jvalue(Value& d) {
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
    else if (d.IsObject())
        init(kObject);
    else if (d.IsArray())
        init(kArray);
}

Jvalue Jvalue::operator=(const Jvalue &rhs) {
    type = rhs.type;
    if (rhs.isString()) vstring = rhs.vstring;
    if (rhs.isInt()) vint = rhs.vint;
    if (rhs.isDouble()) vdouble = rhs.vdouble;
    return *this;
}

bool Jvalue::operator==(const Jvalue &rhs) const {
    // Not same type => different
    if (type != rhs.type) return false;
    // Equal type with value => compare value
    if (isString()) return vstring == rhs.vstring;
    if (isInt()) return vint == rhs.vint;
    if (isDouble()) return vdouble == rhs.vdouble;
    // Equal type without value => equal
    return true;
}

bool Jvalue::operator <(const Jvalue& rhs) const {
    // Not same type => compare type
    if (type < rhs.type) return true;
    if (type > rhs.type) return false;
    // Equal type with value => compare value
    if (isString()) return vstring < rhs.vstring;
    if (isInt()) return vint < rhs.vint;
    if (isDouble()) return vdouble < rhs.vdouble;
    // Equal type without value => equal
    return false;
}


/* Assignment of static values */
Jvalue Jvalue::OBJECT = Jvalue(types(kObject));


ostream& operator<<(ostream &o, const Jvalue &enc) {
    string t[] = {"Null", "False", "True", "Object", "Array", "String", "Int",
        "Double"};
    o << "(" << t[enc.type];
    if (enc.isString()) o << ",\"" << enc.vstring << "\"";
    if (enc.isInt()) o << "," << enc.vint;
    if (enc.isDouble()) o << "," << enc.vdouble;
    return o << ")";
}

#endif