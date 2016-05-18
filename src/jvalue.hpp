
#ifndef __JVALUE_HPP__
#define __JVALUE_HPP__

#include "rapidjson/document.h"

#include <cstring>
#include <iostream>
#include <unordered_set>
#include <string>
#include <vector>

using namespace std;
using namespace rapidjson;

enum class types : char { kNull, kFalse, kTrue, kObject, kArray, kString, kInt, kDouble };

class Jvalue {
public:
    const types type;
    // { "String", "Int", "Double" };
    const void *const val;

public:
    /* Explicit copy constructor */
    Jvalue(const Jvalue &j);
    /* Destructor */
    ~Jvalue();
    /* Static factory methods */
    static Jvalue factory(Value&);
    static Jvalue factory(types t);
    static Jvalue factory(string v);
    static Jvalue factory(int v);
    static Jvalue factory(double v);

    /* Methods */
    bool isNull() const { return type == types::kNull; }
    bool isTrue() const { return type == types::kTrue; }
    bool isFalse() const { return type == types::kFalse; }
    bool isBoolean() const { return isTrue() || isFalse(); }
    bool isObject() const { return type == types::kObject; }
    bool isArray() const { return type == types::kArray; }

    bool isString() const { return type == types::kString; }
    bool isInt() const { return type == types::kInt; }
    bool isDouble() const { return type == types::kDouble; }
    bool hasValue() const { return type >= types::kString && type <= types::kDouble; }

    string getString() const { return string((char*) val); }
    int getInt() const { return *((int*) val); }
    double getDouble() const { return *((double*) val); }

    /* Operators */
    // Jvalue operator=(const Jvalue &enc);
    bool operator==(const Jvalue &enc) const;
    bool operator!=(const Jvalue &rhs) const { return !(*this == rhs); }
    bool operator<(const Jvalue& rhs) const;
    friend ostream& operator<<(ostream &o, const Jvalue &enc);

public:
    /* Static helpers */
    static Jvalue NULL_VAL;
    static Jvalue FALSE_VAL;
    static Jvalue TRUE_VAL;
    static Jvalue OBJECT_VAL;
    static Jvalue ARRAY_VAL;

private:
    /* Constructors */
    Jvalue(types t) : type(t), val(nullptr) {}
    Jvalue(string v) : type(types::kString)
        , val((void*) new char[v.size() + 1]) { strcpy((char*) val, v.c_str()); }
    Jvalue(int v) : type(types::kInt)
        , val((void*) new int(v)) {}
    Jvalue(double v) : type(types::kDouble)
        , val((void*) new double(v)) {}
} __attribute__ ((packed));


namespace std {
    template<> struct hash<Jvalue> {
        size_t operator()(const Jvalue& enc) const {
            size_t h1 = hash<char>()(static_cast<char>(enc.type)), h2;
            if (!enc.hasValue())
                return h1;
            if (enc.isString())
                h2 = hash<string>()(enc.getString());
            if (enc.isInt())
                h2 = hash<int>()(enc.getInt());
            if (enc.isDouble())
                h2 = hash<double>()(enc.getDouble());
            return h1 ^ (h2 << 1);
        }
    };
}

#endif
