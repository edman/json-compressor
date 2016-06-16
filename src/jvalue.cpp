#ifndef __JVALUE_CPP__
#define __JVALUE_CPP__

#include "jvalue.hpp"
#include <cstdlib>

using namespace std;
using namespace rapidjson;

Jvalue Jvalue::factory(uint n, types t) {
    return Jvalue(n, t);
}
Jvalue Jvalue::factory(uint n, string v) { return Jvalue(n, v); }
Jvalue Jvalue::factory(uint n, int v) { return Jvalue(n, v); }
Jvalue Jvalue::factory(uint n, double v) { return Jvalue(n, v); }

Jvalue::Jvalue(const Jvalue &r) : nameId(r.nameId), type(r.type)
    , val(r.isString() ? (void *) string_to_cstr(r.getString())
        : r.isInt() ? (void *) new int(r.getInt())
        : r.isDouble() ? (void *) new double(r.getDouble())
        : nullptr) {}

Jvalue Jvalue::factory(uint n, Value &d) {
    if (d.IsString())
        return Jvalue(n, d.GetString());
    else if (d.IsInt())
        return Jvalue(n, d.GetInt());
    else if (d.IsDouble())
        return Jvalue(n, d.GetDouble());
    else if (d.IsNull())
        return Jvalue(n, types::kNull);
    else if (d.IsFalse())
        return Jvalue(n, types::kFalse);
    else if (d.IsTrue())
        return Jvalue(n, types::kTrue);
    else if (d.IsObject())
        return Jvalue(n, types::kObject);
    // else if (d.IsArray())
    return Jvalue(n, types::kArray);
}

Jvalue::~Jvalue() {
    if (isString()) { delete[] (char*) val; }
    else if (isChar()) { delete (char*) val; }
    else if (isShort()) { delete (short*) val; }
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


types typeForInt(int num) {
    if (intInChar(num)) return types::kChar;
    if (intInShort(num)) return types::kShort;
    return types::kInt;
}

void* pointerForInt(int num) {
    if (intInChar(num)) return (void*) new char(num);
    if (intInShort(num)) return (void*) new short(num);
    return (void*) new int(num);
}

ostream& operator<<(ostream &o, const Jvalue &enc) {
    string t[] = {"Null", "False", "True", "Object", "Array",
            "String", "Char", "Short", "Int", "Double"};
    o << "(" << t[static_cast<unsigned char>(enc.type)];
    if (enc.isString()) o << ",\"" << enc.getString() << "\"";
    if (enc.isInt()) o << "," << enc.getInt();
    if (enc.isDouble()) o << "," << enc.getDouble();
    return o << ")";
}

#endif
