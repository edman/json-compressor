
#ifndef __JVAL_CPP__
#define __JVAL_CPP__

#include "jval.hpp"
// #include <cstdlib>
// #include <iostream>

using namespace std;


/******************************************************************************/
/* Member access methods implementations */

uint Jval::nameId() const {
    return (hasName(_type)) ? ((NamedJval*) this)->nameId() : MAX_UINT;
}

string Jval::getString() const {
    return string(getCStr());
}

const char* Jval::getCStr() const {
    assert(isString());
    if (hasName()) return ((NamedStringJval*) this)->_data;
    return ((StringJval*) this)->_data;
}

char Jval::getChar() const {
    assert(isChar());
    if (hasName()) return ((NamedCharJval*) this)->_data;
    return ((CharJval*) this)->_data;
}

short Jval::getShort() const {
    assert(isShort());
    if (isChar()) return getChar();
    if (hasName()) return ((NamedShortJval*) this)->_data;
    return ((ShortJval*) this)->_data;
}

int Jval::getInt() const {
    assert(isInt());
    if (isShort()) return getShort();
    if (hasName()) return ((NamedIntJval*) this)->_data;
    return ((IntJval*) this)->_data;
}

double Jval::getDouble() const {
    assert(isDouble());
    if (hasName()) return ((NamedDoubleJval*) this)->_data;
    return ((DoubleJval*) this)->_data;
}


/******************************************************************************/
/* Specialized copy constructors for strings */

template<> StringJval::DataJval(const StringJval &other) : Jval(other._type),
        _data(cstr_copy(other._data)) {}

template<> NamedStringJval::NamedDataJval(const NamedStringJval &other) :
        NamedJval(other._type, other._nameId), _data(cstr_copy(other._data)) {}


/******************************************************************************/
/* Specialized move constructors for strings */

template<> StringJval::DataJval(StringJval &&other) : Jval(other._type),
        _data(cstr_copy(other._data)) { other._data = nullptr; }

template<> NamedStringJval::NamedDataJval(NamedStringJval &&other) :
        NamedJval(other._type, other._nameId), _data(cstr_copy(other._data)) {
    other._data = nullptr;
}


/******************************************************************************/
/* Specialized destructors for strings */

template<> StringJval::~DataJval() {
    if (_data != nullptr) delete[] _data;
    // if (_data != nullptr) { cout << "deleting " << _data << endl; delete[] _data; }
}

template<> NamedStringJval::~NamedDataJval() {
    if (_data != nullptr) delete[] _data;
    // if (_data != nullptr) { cout << "deleting " << _data << endl; delete[] _data; }
}


/******************************************************************************/
/* Definition of static variables */

const uchar Jval::TYPE_NULL;
const uchar Jval::TYPE_OBJECT;
const uchar Jval::TYPE_ARRAY;
const uchar Jval::TYPE_FALSE;
const uchar Jval::TYPE_TRUE;
const uchar Jval::TYPE_STRING;
const uchar Jval::TYPE_CHAR;
const uchar Jval::TYPE_SHORT;
const uchar Jval::TYPE_INT;
const uchar Jval::TYPE_DOUBLE;
const uchar Jval::NAMED;


#endif
