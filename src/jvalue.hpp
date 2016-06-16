
#ifndef __JVALUE_HPP__
#define __JVALUE_HPP__

#include "util.hpp"
#include "rapidjson/document.h"

#include <cstring>
#include <iostream>
#include <unordered_set>
#include <string>
#include <vector>

using namespace std;
using namespace rapidjson;

enum class types : unsigned char { kNull, kFalse, kTrue, kObject, kArray,
        kString, kChar, kShort, kInt, kDouble };

types typeForInt(int);
void* pointerForInt(int);

class Jvalue {
public:
    const uint nameId;
    const types type;
    // { "String", "Int", "Double" };
    const void *const val;

public:
    /* Explicit copy constructor */
    Jvalue(const Jvalue &j);
    /* Destructor */
    ~Jvalue();
    /* Static factory methods */
    static Jvalue factory(uint n, Value &d);
    static Jvalue factory(uint n, types t);
    static Jvalue factory(uint n, string v);
    static Jvalue factory(uint n, int v);
    static Jvalue factory(uint n, double v);

    /* Methods */
    bool isNull() const { return type == types::kNull; }
    bool isTrue() const { return type == types::kTrue; }
    bool isFalse() const { return type == types::kFalse; }
    bool isBoolean() const { return isTrue() || isFalse(); }
    bool isObject() const { return type == types::kObject; }
    bool isArray() const { return type == types::kArray; }

    bool isString() const { return type == types::kString; }
    bool isChar() const { return type == types::kChar; }
    bool isShort() const { return isChar() || type == types::kShort; }
    bool isInt() const { return isShort() || type == types::kInt; }
    bool isDouble() const { return type == types::kDouble; }
    bool hasValue() const { return type >= types::kString && type <= types::kDouble; }

    string getString() const { return string((char*) val); }
    char *const getCStr() const { return (char*) val; }
    char getChar() const { return *((char*) val); }
    short getShort() const { return isChar() ? getChar() : *((short*) val); }
    int getInt() const { return isShort() ? getShort() : *((int*) val); }
    double getDouble() const { return *((double*) val); }

    /* Operators */
    // Jvalue operator=(const Jvalue &enc);
    bool operator==(const Jvalue &enc) const;
    bool operator!=(const Jvalue &rhs) const { return !(*this == rhs); }
    bool operator<(const Jvalue& rhs) const;
    friend ostream& operator<<(ostream &o, const Jvalue &enc);

private:
    /* Constructors */
    Jvalue(uint n, types t) : nameId(n), type(t), val(nullptr) {}
    Jvalue(uint n, char *v) : nameId(n), type(types::kString)
        , val(cstr_copy(v)) {}
    Jvalue(uint n, string v) : nameId(n), type(types::kString)
        , val(string_to_cstr(v)) {}
    Jvalue(uint n, int v) : nameId(n), type(typeForInt(v))
        , val(pointerForInt(v)) {}
    Jvalue(uint n, double v) : nameId(n), type(types::kDouble)
        , val((void*) new double(v)) {}
} __attribute__ ((packed));


namespace std {
    template<> struct hash<Jvalue> {
        size_t operator()(const Jvalue& enc) const {
            size_t h1 = hash<unsigned char>()(static_cast<unsigned char>(enc.type)), h2, h3;
            if (!enc.hasValue())
                return h1;
            if (enc.isString())
                h2 = hash<string>()(enc.getString());
            if (enc.isInt())
                h2 = hash<int>()(enc.getInt());
            if (enc.isDouble())
                h2 = hash<double>()(enc.getDouble());
            h3 = hash<uint>()(enc.nameId);
            return h1 ^ ((h2 ^ (h3 << 1)) << 1);
        }
    };
}

#endif
