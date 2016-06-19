
#include "gtest/gtest.h"
#include "../src/util.hpp"
#include "../src/jval.hpp"
#include <cstring>

using namespace std;

TEST(JvalueTest, Jval) {
    /* Jval correctly constructs an unnamed dataless value */

    Jval v(Jval::TYPE_OBJECT);
    EXPECT_EQ(1, sizeof(v)); // correct size
    EXPECT_EQ(v.type(), Jval::TYPE_OBJECT); // correct type
    EXPECT_EQ(v.nameId(), MAX_UINT); // correct name id
    EXPECT_FALSE(v.hasName()); // shouldn't have name
    EXPECT_FALSE(v.hasValue()); // shouldn't have value
}

TEST(JvalueTest, StringJval) {
    /* Jval correctly constructs an unnamed int value */

    string val = "a dog could smell your mistakes";
    StringJval i(string_to_cstr(val));
    EXPECT_EQ(9, sizeof(i)); // correct size
    EXPECT_EQ(Jval::TYPE_STRING, i.type()); // correct type
    EXPECT_TRUE(i.hasValue()); // should have value
    EXPECT_EQ(0, strcmp(val.c_str(), i.getCStr())); // correct value
    EXPECT_EQ(val, i.getString()); // correct value
    EXPECT_FALSE(i.hasName()); // shouldn't have name

    // assert things still work with a pointer to Jval
    Jval *v = &i;
    EXPECT_EQ(Jval::TYPE_STRING, v->type()); // correct type
    EXPECT_TRUE(v->hasValue()); // should have value
    EXPECT_EQ(0, strcmp(val.c_str(), v->getCStr())); // correct value
    EXPECT_EQ(val, v->getString()); // correct value
    EXPECT_FALSE(v->hasName()); // shouldn't have name
}


TEST(JvalueTest, CharJval) {
    /* Jval correctly constructs an unnamed char value */

    CharJval i(42);
    EXPECT_EQ(2, sizeof(i)); // correct size
    EXPECT_EQ(Jval::TYPE_CHAR, i.type()); // correct type
    EXPECT_TRUE(i.hasValue()); // should have value
    EXPECT_EQ(42, i.getInt()); // correct value
    EXPECT_FALSE(i.hasName()); // shouldn't have name

    // assert things still work with a pointer to Jval
    Jval *v = &i;
    EXPECT_EQ(Jval::TYPE_CHAR, v->type()); // correct type
    EXPECT_TRUE(v->hasValue()); // should have value
    EXPECT_EQ(42, v->getInt()); // correct value
    EXPECT_FALSE(v->hasName()); // shouldn't have name
}

TEST(JvalueTest, ShortJval) {
    /* Jval correctly constructs an unnamed short value */

    ShortJval i(4200);
    EXPECT_EQ(3, sizeof(i)); // correct size
    EXPECT_EQ(Jval::TYPE_SHORT, i.type()); // correct type
    EXPECT_TRUE(i.hasValue()); // should have value
    EXPECT_EQ(4200, i.getInt()); // correct value
    EXPECT_FALSE(i.hasName()); // shouldn't have name

    // assert things still work with a pointer to Jval
    Jval *v = &i;
    EXPECT_EQ(Jval::TYPE_SHORT, v->type()); // correct type
    EXPECT_TRUE(v->hasValue()); // should have value
    EXPECT_EQ(4200, v->getInt()); // correct value
    EXPECT_FALSE(v->hasName()); // shouldn't have name
}

TEST(JvalueTest, IntJval) {
    /* Jval correctly constructs an unnamed int value */

    IntJval i(420000);
    EXPECT_EQ(5, sizeof(i)); // correct size
    EXPECT_EQ(Jval::TYPE_INT, i.type()); // correct type
    EXPECT_TRUE(i.hasValue()); // should have value
    EXPECT_EQ(420000, i.getInt()); // correct value
    EXPECT_FALSE(i.hasName()); // shouldn't have name

    // assert things still work with a pointer to Jval
    Jval *v = &i;
    EXPECT_EQ(Jval::TYPE_INT, v->type()); // correct type
    EXPECT_TRUE(v->hasValue()); // should have value
    EXPECT_EQ(420000, v->getInt()); // correct value
    EXPECT_FALSE(v->hasName()); // shouldn't have name
}

TEST(JvalueTest, DoubleJval) {
    /* Jval correctly constructs an unnamed double value */

    DoubleJval i(3.14);
    EXPECT_EQ(9, sizeof(i)); // correct size
    EXPECT_EQ(Jval::TYPE_DOUBLE, i.type()); // correct type
    EXPECT_TRUE(i.hasValue()); // should have value
    EXPECT_EQ(3.14, i.getDouble()); // correct value
    EXPECT_FALSE(i.hasName()); // shouldn't have name

    // assert things still work with a pointer to Jval
    Jval *v = &i;
    EXPECT_EQ(Jval::TYPE_DOUBLE, v->type()); // correct type
    EXPECT_TRUE(v->hasValue()); // should have value
    EXPECT_EQ(3.14, v->getDouble()); // correct value
    EXPECT_FALSE(v->hasName()); // shouldn't have name
}

TEST(JvalueTest, NamedJval) {
    /* Jval correctly constructs a named dataless value */

    NamedJval n(Jval::TYPE_ARRAY, 42);
    EXPECT_EQ(5, sizeof(n)); // correct size
    EXPECT_EQ(Jval::TYPE_ARRAY, n.type()); // correct type
    EXPECT_EQ(42, n.nameId()); // correct name id
    EXPECT_TRUE(n.hasName()); // should have name
    EXPECT_FALSE(n.hasValue()); // should have value

    // assert things still work with a pointer to Jval
    Jval *v = &n;
    EXPECT_EQ(Jval::TYPE_ARRAY, v->type()); // correct type
    EXPECT_EQ(42, v->nameId()); // correct name id
    EXPECT_TRUE(v->hasName()); // should have name
    EXPECT_FALSE(v->hasValue()); // should have value
}

TEST(JvalueTest, NamedStringJval) {
    /* Jval correctly constructs a named string value */

    string val = "even a dog can smell your farts";
    NamedStringJval i(42, string_to_cstr(val));
    EXPECT_EQ(13, sizeof(i)); // correct size
    EXPECT_EQ(Jval::TYPE_STRING, i.type()); // correct type
    EXPECT_TRUE(i.hasValue()); // should have value
    EXPECT_EQ(0, strcmp(val.c_str(), i.getCStr())); // correct value
    EXPECT_EQ(val, i.getString()); // correct value
    EXPECT_TRUE(i.hasName()); // should have name

    // assert things still work with a pointer to Jval
    Jval *v = &i;
    EXPECT_EQ(Jval::TYPE_STRING, v->type()); // correct type
    EXPECT_TRUE(v->hasValue()); // should have value
    EXPECT_EQ(0, strcmp(val.c_str(), v->getCStr())); // correct value
    EXPECT_EQ(val, v->getString()); // correct value
    EXPECT_TRUE(v->hasName()); // should have name
}

TEST(JvalueTest, NamedCharJval) {
    /* Jval correctly constructs an unnamed char value */

    NamedCharJval i(27, 42);
    EXPECT_EQ(6, sizeof(i)); // correct size
    EXPECT_EQ(Jval::TYPE_CHAR, i.type()); // correct type
    EXPECT_TRUE(i.hasValue()); // should have value
    EXPECT_EQ(42, i.getInt()); // correct value
    EXPECT_TRUE(i.hasName()); // should have name
    EXPECT_EQ(27, i.nameId()); // correct name

    // assert things still work with a pointer to Jval
    Jval *v = &i;
    EXPECT_EQ(Jval::TYPE_CHAR, v->type()); // correct type
    EXPECT_TRUE(v->hasValue()); // should have value
    EXPECT_EQ(42, v->getInt()); // correct value
    EXPECT_TRUE(v->hasName()); // should have name
    EXPECT_EQ(27, v->nameId()); // correct name
}

TEST(JvalueTest, NamedShortJval) {
    /* Jval correctly constructs an unnamed short value */

    NamedShortJval i(27, 4200);
    EXPECT_EQ(7, sizeof(i)); // correct size
    EXPECT_EQ(Jval::TYPE_SHORT, i.type()); // correct type
    EXPECT_TRUE(i.hasValue()); // should have value
    EXPECT_EQ(4200, i.getInt()); // correct value
    EXPECT_TRUE(i.hasName()); // should have name
    EXPECT_EQ(27, i.nameId()); // correct name

    // assert things still work with a pointer to Jval
    Jval *v = &i;
    EXPECT_EQ(Jval::TYPE_SHORT, v->type()); // correct type
    EXPECT_TRUE(v->hasValue()); // should have value
    EXPECT_EQ(4200, v->getInt()); // correct value
    EXPECT_TRUE(v->hasName()); // should have name
    EXPECT_EQ(27, v->nameId()); // correct name
}

TEST(JvalueTest, NamedIntJval) {
    /* Jval correctly constructs an unnamed int value */

    NamedIntJval i(27, 420000);
    EXPECT_EQ(9, sizeof(i)); // correct size
    EXPECT_EQ(Jval::TYPE_INT, i.type()); // correct type
    EXPECT_TRUE(i.hasValue()); // should have value
    EXPECT_EQ(420000, i.getInt()); // correct value
    EXPECT_TRUE(i.hasName()); // should have name
    EXPECT_EQ(27, i.nameId()); // correct name

    // assert things still work with a pointer to Jval
    Jval *v = &i;
    EXPECT_EQ(Jval::TYPE_INT, v->type()); // correct type
    EXPECT_TRUE(v->hasValue()); // should have value
    EXPECT_EQ(420000, v->getInt()); // correct value
    EXPECT_TRUE(v->hasName()); // should have name
    EXPECT_EQ(27, v->nameId()); // correct name
}

TEST(JvalueTest, NamedDoubleJval) {
    /* Jval correctly constructs an unnamed double value */

    NamedDoubleJval i(27, 3.14);
    EXPECT_EQ(13, sizeof(i)); // correct size
    EXPECT_EQ(Jval::TYPE_DOUBLE, i.type()); // correct type
    EXPECT_TRUE(i.hasValue()); // should have value
    EXPECT_EQ(3.14, i.getDouble()); // correct value
    EXPECT_TRUE(i.hasName()); // should have name
    EXPECT_EQ(27, i.nameId()); // correct name

    // assert things still work with a pointer to Jval
    Jval *v = &i;
    EXPECT_EQ(Jval::TYPE_DOUBLE, v->type()); // correct type
    EXPECT_TRUE(v->hasValue()); // should have value
    EXPECT_EQ(3.14, v->getDouble()); // correct value
    EXPECT_TRUE(v->hasName()); // should have name
    EXPECT_EQ(27, v->nameId()); // correct name
}

TEST(JvalueTest, CopyConstructor) {
    /* Jval copy constructor works correctly */

    NamedDoubleJval one(27, 3.14);
    NamedDoubleJval other(one);

    EXPECT_EQ(one.type(), other.type());
    EXPECT_EQ(one.hasName(), other.hasName());
    EXPECT_EQ(one.hasValue(), other.hasValue());
    EXPECT_EQ(one.nameId(), other.nameId());
    EXPECT_EQ(one.getDouble(), other.getDouble());
}

TEST(JvalueTest, StrngCopyConstructor) {
    /* Jval string copy constructor works correctly */

    string val = "a dog could smell your mistakes";
    NamedStringJval one(27, string_to_cstr(val));
    NamedStringJval other(one);

    EXPECT_EQ(one.type(), other.type());
    EXPECT_EQ(one.hasName(), other.hasName());
    EXPECT_EQ(one.hasValue(), other.hasValue());
    EXPECT_EQ(one.nameId(), other.nameId());
    EXPECT_EQ(one.getString(), other.getString());
}

TEST(JvalueTest, MoveConstructor) {
    /* Jval move constructor works correctly */

    IntJval one(IntJval(42));

    EXPECT_EQ(Jval::TYPE_INT, one.type());
    EXPECT_TRUE(one.hasValue());
    EXPECT_FALSE(one.hasName());
    EXPECT_EQ(MAX_UINT, one.nameId());
    EXPECT_EQ(42, one.getInt());
}

TEST(JvalueTest, StrngMoveConstructor) {
    /* Jval string movw constructor works correctly */

    string val = "a dog could smell your mistakes";
    NamedStringJval one(NamedStringJval(27, string_to_cstr(val)));

    EXPECT_EQ(Jval::TYPE_STRING, one.type());
    EXPECT_TRUE(one.hasValue());
    EXPECT_TRUE(one.hasName());
    EXPECT_EQ(27, one.nameId());
    EXPECT_EQ(val, one.getString());
}
