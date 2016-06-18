
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
    types type;
    uint nameId;
    // { "String", "Int", "Double" };
    const void *val;

public:
    /* Explicit copy constructor */
    Jvalue(const Jvalue&);
    /* Explicit move constructor */
    Jvalue(Jvalue&&);
    /* Destructor */
    ~Jvalue();
    /* Static factory methods */
    static Jvalue factory(Value &d, uint n);
    static Jvalue factory(types t, uint n);
    static Jvalue factory(string v, uint n);
    static Jvalue factory(int v, uint n);
    static Jvalue factory(double v, uint n);
    // static Jvalue factory(void*);
    // Unnamed
    // static Jvalue factory(Value &d);
    // static Jvalue factory(types t);
    // static Jvalue factory(string v);
    // static Jvalue factory(int v);
    // static Jvalue factory(double v);

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
    Jvalue& operator=(Jvalue&);  // copy-assignment operator
    Jvalue& operator=(Jvalue&&); // move-assignment operator
    bool operator==(const Jvalue&) const;
    bool operator!=(const Jvalue &rhs) const { return !(*this == rhs); }
    bool operator<(const Jvalue&) const;
    friend ostream& operator<<(ostream &o, const Jvalue &enc);

private:
    /* Constructors */
    Jvalue(types t, uint n) : type(t), nameId(n), val(nullptr) {}
    Jvalue(char *v, uint n) : type(types::kString), nameId(n)
        , val(cstr_copy(v)) {}
    Jvalue(string v, uint n) : type(types::kString), nameId(n)
        , val(string_to_cstr(v)) {}
    Jvalue(int v, uint n) : type(typeForInt(v)), nameId(n)
        , val(pointerForInt(v)) {}
    Jvalue(double v, uint n) : type(types::kDouble), nameId(n)
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
