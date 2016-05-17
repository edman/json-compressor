#ifndef __JVALUE_CPP__
#define __JVALUE_CPP__

#include "jvalue.hpp"
#include "cjson.hpp"
#include "rapidjson/document.h"
#include <cstdlib>
#include <iostream>

using namespace std;
using namespace rapidjson;

Jvalue Jvalue::factory(types t) {
    if (t == kNull) return Jvalue::NULL_VAL;
    else if (t == kFalse) return Jvalue::FALSE_VAL;
    else if (t == kTrue) return Jvalue::TRUE_VAL;
    else if (t == kObject) return Jvalue::OBJECT_VAL;
    // else if (t == kArray)
    return Jvalue::ARRAY_VAL;
}
Jvalue Jvalue::factory(string v) { return Jvalue(v); }
Jvalue Jvalue::factory(int v) { return Jvalue(v); }
Jvalue Jvalue::factory(double v) { return Jvalue(v); }

Jvalue::Jvalue(const Jvalue &r) : type(r.type)
    , val(r.isString() ? (void *) new string(r.getString())
        : r.isInt() ? (void *) new int(r.getInt())
        : r.isDouble() ? (void *) new double(r.getDouble())
        : nullptr) {}

Jvalue Jvalue::factory(Value &d) {
    if (d.IsString())
        return Jvalue(d.GetString());
    else if (d.IsInt())
        return Jvalue(d.GetInt());
    else if (d.IsDouble())
        return Jvalue(d.GetDouble());
    else if (d.IsNull())
        return Jvalue::NULL_VAL;
    else if (d.IsFalse())
        return Jvalue::FALSE_VAL;
    else if (d.IsTrue())
        return Jvalue::TRUE_VAL;
    else if (d.IsObject())
        return Jvalue::OBJECT_VAL;
    // else if (d.IsArray())
    return Jvalue::ARRAY_VAL;
}

Jvalue::~Jvalue() {
    if (isString()) { delete (string*) val; }
    else if (isInt()) { delete (int*) val; }
    else if (isDouble()) { delete (double*) val; }
}

// Jvalue Jvalue::operator=(const Jvalue &rhs) {
//     type = rhs.type;
//     if (rhs.isString()) vstring = rhs.vstring;
//     if (rhs.isInt()) vint = rhs.vint;
//     if (rhs.isDouble()) vdouble = rhs.vdouble;
//     return *this;
// }

bool Jvalue::operator==(const Jvalue &rhs) const {
    // Not same type => different
    if (type != rhs.type) return false;
    // Equal type with value => compare value
    if (isString()) return getString() == rhs.getString();
    if (isInt()) return getInt() == rhs.getInt();
    if (isDouble()) return getDouble() == rhs.getDouble();
    // Equal type without value => equal
    return true;
}

bool Jvalue::operator <(const Jvalue& rhs) const {
    // Not same type => compare type
    if (type < rhs.type) return true;
    if (type > rhs.type) return false;
    // Equal type with value => compare value
    if (isString()) return getString() < rhs.getString();
    if (isInt()) return getInt() < rhs.getInt();
    if (isDouble()) return getDouble() < rhs.getDouble();
    // Equal type without value => equal
    return false;
}


/* Assignment of static values */
Jvalue Jvalue::NULL_VAL = Jvalue(types(kNull));
Jvalue Jvalue::FALSE_VAL = Jvalue(types(kFalse));
Jvalue Jvalue::TRUE_VAL = Jvalue(types(kTrue));
Jvalue Jvalue::OBJECT_VAL = Jvalue(types(kObject));
Jvalue Jvalue::ARRAY_VAL = Jvalue(types(kArray));


ostream& operator<<(ostream &o, const Jvalue &enc) {
    string t[] = {"Null", "False", "True", "Object", "Array", "String", "Int",
        "Double"};
    o << "(" << t[enc.type];
    if (enc.isString()) o << ",\"" << enc.getString() << "\"";
    if (enc.isInt()) o << "," << enc.getInt();
    if (enc.isDouble()) o << "," << enc.getDouble();
    return o << ")";
}

#endif
