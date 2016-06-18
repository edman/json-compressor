
#include "gtest/gtest.h"
#include "../src/bitmap_index.hpp"
#include "../src/jvalue.hpp"

using namespace std;

TEST(BitmapIndexTest, Append) {
    /* BitmapIndex correctly indexes the positions of inserted elements */

    BitmapIndex values;

    // initially empty
    EXPECT_EQ(0, values.size());

    short s = 2;
    int i = 10;
    double d = 3.14;

    values.append(s); // append a short
    values.append(i); // append an int
    values.append(d); // append a double
    EXPECT_EQ(3, values.size());

    // load the bitmap index
    values.loadBitmap();

    // assert the bitmap has correct indices
    int bitlen = 14;
    int t[] = {1,0,1,0,0,0,1,0,0,0,0,0,0,0};
    EXPECT_EQ(bitlen, values._bitmap.size());
    for (int i = 0; i < bitlen; ++i)
        EXPECT_EQ(t[i], values._bitmap[i]);
}

TEST(BitmapIndexTest, Get) {
    /* BitmapIndex correctly retrieves inserted elements */

    BitmapIndex values;

    short s = 2;
    int i = 10;
    double d = 3.14;

    values.append(s); // append a short
    values.append(i); // append an int
    values.append(d); // append a double
    values.loadBitmap(); // load bitmap index

    // assert the inserted objects can be retrieved by index given its type
    EXPECT_EQ(s, values.get<short>(0));
    EXPECT_EQ(i, values.get<int>(1));
    EXPECT_EQ(d, values.get<double>(2));
}

TEST(BitmapIndexTest, Jvalue) {
    /* BitmapIndex correctly handles Jvalue objects */

    BitmapIndex values;

    Jvalue o = Jvalue::factory(types::kObject, 1);
    Jvalue i = Jvalue::factory(1 << 20, 2);
    Jvalue d = Jvalue::factory(3.14, 3);

    values.append(o); // append an object
    values.append(i); // append an int
    values.append(d); // append a double
    values.loadBitmap(); // load bitmap index

    // assert the inserted Jvalue objects can be retrieved correctly
    EXPECT_EQ(o, values.get<Jvalue>(0));
    EXPECT_EQ(i, values.get<Jvalue>(1));
    EXPECT_EQ(d, values.get<Jvalue>(2));
}

