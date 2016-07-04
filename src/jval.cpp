
#ifndef __JVAL_CPP__
#define __JVAL_CPP__

#include "jval.hpp"

using namespace std;


/******************************************************************************/
/* Member access methods implementations */

uint Jval::nameId() const {
    return (hasName(_type)) ? ((NamedJval*) this)->nameId() : MAX_UINT;
}

uint Jval::getStringIndex() const {
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
/* Output operator */
ostream& operator<<(ostream &o, const Jval &v) {
    string types[] = {"NULL", "OBJECT", "ARRAY", "FALSE", "TRUE",
        "STRING", "CHAR", "SHORT", "INT", "DOUBLE"};
    o << "(" << types[v.type() - 1];
    if (v.hasName()) o << ",n" << v.nameId();
    if (v.isString()) o << ",v" << v.getStringIndex();
    if (v.isInt()) o << ",v" << v.getInt();
    if (v.isDouble()) o << ",v" << v.getDouble();
    return o << ")";
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
