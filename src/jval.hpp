
#ifndef __JVAL_HPP__
#define __JVAL_HPP__

#include "util.hpp"

using namespace std;

/******************************************************************************/
/* Simple Jval */
class Jval {
public:
    /* Member variables */
    uchar _type;

    /* Type static constants */
    static const uchar TYPE_NULL   = 1;
    static const uchar TYPE_OBJECT = 2;
    static const uchar TYPE_ARRAY  = 3;
    static const uchar TYPE_FALSE  = 4;
    static const uchar TYPE_TRUE   = 5;
    static const uchar TYPE_STRING = 6;
    static const uchar TYPE_CHAR   = 7;
    static const uchar TYPE_SHORT  = 8;
    static const uchar TYPE_INT    = 9;
    static const uchar TYPE_DOUBLE = 10;
    /* Named value flag */
    static const uchar NAMED  = (1 << 7);

public:
    /* Constructors */
    Jval(uchar type) : _type(type) { }
    // Copy constructor (folds as move constructor too because const ref)
    Jval(const Jval &r) : _type(r._type) {}

    /* Static helper methods */
    inline static uchar unname(uchar type) { return type & ~NAMED; }
    inline static uchar doname(uchar type) { return type | NAMED; }
    inline static bool hasName(uchar type) { return type & NAMED; }
    inline static bool hasValue(uchar t) {
        return unname(t) >= TYPE_STRING && unname(t) <= TYPE_DOUBLE; }

    /* Type check helper member methods */
    inline bool isNull() const { return type() == TYPE_NULL; }
    inline bool isObject() const { return type() == TYPE_OBJECT; }
    inline bool isArray() const { return type() == TYPE_ARRAY; }
    inline bool isFalse() const { return type() == TYPE_FALSE; }
    inline bool isTrue() const { return type() == TYPE_TRUE; }
    inline bool isBoolean() const { return isTrue() || isFalse(); }
    inline bool isString() const { return type() == TYPE_STRING; }
    inline bool isChar() const { return type() == TYPE_CHAR; }
    inline bool isShort() const { return isChar() || type() == TYPE_SHORT; }
    inline bool isInt() const { return isShort() || type() == TYPE_INT; }
    inline bool isDouble() const { return type() == TYPE_DOUBLE; }

    /* Member methods */
    inline uchar type() const { return unname(_type); }
    uint nameId() const;
    inline bool hasName() const { return hasName(_type); }
    inline bool hasValue() const { return hasValue(_type); }

    /* Data access methods */
    uint getStringIndex() const;
    char getChar() const;
    short getShort() const;
    int getInt() const;
    double getDouble() const;

    /* Operators */
    // copy-assignment operator (folds as move-assignment operator)
    Jval& operator=(const Jval &other) { _type = other._type; return *this; }

} __attribute__ ((packed));

ostream& operator<<(ostream &o, const Jval &v);

/******************************************************************************/
/* Simple Named Jval */
class NamedJval : public Jval {
public:
    /* Member variables */
    uint _nameId;

public:
    /* Constructors */
    NamedJval(uchar type, uint nameId) : Jval(doname(type)), _nameId(nameId) { }
    // Copy constructor (folds as move constructor)
    NamedJval(const NamedJval &r) : Jval(r._type), _nameId(r._nameId) { }

    /* Member methods */
    uint nameId() { return _nameId; }

    /* Operators */
    // copy-assignment operator (folds as move-assignment operator)
    NamedJval& operator=(const NamedJval &other) {
        _type = other._type; _nameId = other._nameId; return *this; }

} __attribute__ ((packed));


/******************************************************************************/
/* Valued Jval */
template <typename T, uchar TYPE>
class DataJval : public Jval {
public:
    /* Member variables */
    T _data;

public:
    /* Constructors */
    DataJval(T data) : Jval(TYPE), _data(data) {}
    // Copy constructor (folds as move constructor)
    DataJval(const DataJval<T,TYPE> &r) : Jval(TYPE), _data(r._data) {}

    /* Operators */
    // copy-assignment operator (folds as move-assignment operator)
    DataJval& operator=(const DataJval &other) {
        _type = other._type; _data = other._data; return *this; }
} __attribute__ ((packed));


/******************************************************************************/
/* Named Valued Jval */
template <typename T, uchar TYPE>
class NamedDataJval : public NamedJval {
public:
    /* Member variables */
    T _data;

public:
    /* Constructors */
    NamedDataJval(uint nameId, T data) : NamedJval(doname(TYPE), nameId),
            _data(data) {}
    // Copy constructor (folds as move constructor)
    NamedDataJval(const NamedDataJval &r) : NamedJval(doname(TYPE), r._nameId),
            _data(r._data) {}

    /* Operators */
    // copy-assignment operator (folds as move-assignment operator)
    NamedDataJval& operator=(const NamedDataJval &other) {
        _type = other._type; _nameId = other._nameId; _data = other._data;
        return *this; }
} __attribute__ ((packed));


/******************************************************************************/
/* Typedefs and template instantiations */
typedef DataJval<uint,   Jval::TYPE_STRING> StringJval;
typedef DataJval<char,   Jval::TYPE_CHAR>   CharJval;
typedef DataJval<short,  Jval::TYPE_SHORT>  ShortJval;
typedef DataJval<int,    Jval::TYPE_INT>    IntJval;
typedef DataJval<double, Jval::TYPE_DOUBLE> DoubleJval;

typedef NamedDataJval<uint,   Jval::TYPE_STRING> NamedStringJval;
typedef NamedDataJval<char,   Jval::TYPE_CHAR>   NamedCharJval;
typedef NamedDataJval<short,  Jval::TYPE_SHORT>  NamedShortJval;
typedef NamedDataJval<int,    Jval::TYPE_INT>    NamedIntJval;
typedef NamedDataJval<double, Jval::TYPE_DOUBLE> NamedDoubleJval;


#endif
