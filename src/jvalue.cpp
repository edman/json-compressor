#ifndef __JVALUE_CPP__
#define __JVALUE_CPP__

#include "jvalue.hpp"
#include <cstdlib>

using namespace std;
using namespace rapidjson;

Jvalue Jvalue::factory(types t, uint n) {
    return Jvalue(t, n);
}
Jvalue Jvalue::factory(string v, uint n) { return Jvalue(v, n); }
Jvalue Jvalue::factory(int v, uint n) { return Jvalue(v, n); }
Jvalue Jvalue::factory(double v, uint n) { return Jvalue(v, n); }

Jvalue::Jvalue(const Jvalue &r) : type(r.type), nameId(r.nameId)
    , val(r.isString() ? (void *) string_to_cstr(r.getString())
        : r.isChar() ? (void *) new char(r.getChar())
        : r.isShort() ? (void *) new short(r.getShort())
        : r.isInt() ? (void *) new int(r.getInt())
        : r.isDouble() ? (void *) new double(r.getDouble())
        : nullptr) {}

Jvalue::Jvalue(Jvalue &&other) : type(types::kNull), nameId(-1), val(nullptr) {
    // copy the data of other to this
    type = other.type;
    nameId = other.nameId;
    val = other.val;
    // remove the content of other
    other.type = types::kNull;
    other.nameId = -1;
    other.val = nullptr;
}

Jvalue Jvalue::factory(Value &d, uint n) {
    if (d.IsString())
        return Jvalue(d.GetString(), n);
    else if (d.IsInt())
        return Jvalue(d.GetInt(), n);
    else if (d.IsDouble())
        return Jvalue(d.GetDouble(), n);
    else if (d.IsNull())
        return Jvalue(types::kNull, n);
    else if (d.IsFalse())
        return Jvalue(types::kFalse, n);
    else if (d.IsTrue())
        return Jvalue(types::kTrue, n);
    else if (d.IsObject())
        return Jvalue(types::kObject, n);
    // else if (d.IsArray())
    return Jvalue(types::kArray, n);
}

// Jvalue& Jvalue::factory(void *pointer) {
//     Jvalue *value = (Jvalue*) pointer;
//     return value;
// }

Jvalue::~Jvalue() {
    if (isString()) { delete[] (char*) val; }
    else if (isChar()) { delete (char*) val; }
    else if (isShort()) { delete (short*) val; }
    else if (isInt()) { delete (int*) val; }
    else if (isDouble()) { delete (double*) val; }
}

Jvalue& Jvalue::operator=(Jvalue &other) {
    // Copy-assignment operator
    if (this != &other) {
        // copy the data from other to this
        type = other.type;
        nameId = other.nameId;
        val = other.isString() ? (void *) string_to_cstr(other.getString())
            : other.isInt() ? (void *) new int(other.getInt())
            : other.isDouble() ? (void *) new double(other.getDouble())
            : nullptr;
    }
    return *this;
}

Jvalue& Jvalue::operator=(Jvalue &&other) {
    // Move-assignment operator
    if (this != &other) {
        // copy the data from other to this
        type = other.type;
        nameId = other.nameId;
        val = other.val;

        // remove the content of other
        other.type = types::kNull;
        other.nameId = -1;
        other.val = nullptr;
    }
    return *this;
}

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
    o << "," << enc.nameId << "," << enc.val;
    return o << ")";
}

#endif
