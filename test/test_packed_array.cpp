
#include "gtest/gtest.h"
#include "../src/packed_array.hpp"

using namespace std;

const uint INITIAL_CAPACITY = 200;

TEST(PackedArrayTest, Double) {
    /* PackedArray correctly doubles its capacity when needed */

    PackedArray array;

    // initially empty
    EXPECT_EQ(0, array.size());
    EXPECT_EQ(0, array.capacity());

    // inserting 50 ints should not trigger a double
    for (int i = 0; i < 50; ++i) {
        array.append(2 * i);
        EXPECT_EQ(sizeof(int) * (i + 1), array.size());
        EXPECT_EQ(INITIAL_CAPACITY, array.capacity());
    }

    // inserting the 51st int should trigger a double
    array.append(3);
    EXPECT_EQ(sizeof(int) * 51, array.size());
    EXPECT_EQ(2 * INITIAL_CAPACITY, array.capacity());
}

TEST(PackedArrayTest, Get) {
    /* PackedArray retrieves elements correctly */

    PackedArray array;
    int i = 10;
    double d = 1.0;
    char s[] = "even"; int len = 5;

    array.append(10); // append an int
    array.append(1.0); // append a double
    array.appendArray(s, len); // append a char array
    array.append(i); // append an int
    array.append(d); // append a double
    array.appendArray(s, len); // append a char array

    // assert all appended elements are accessible
    EXPECT_EQ(2 * (sizeof(i) + sizeof(d) + len), array.size());
    EXPECT_EQ(i, array.get<int>(0));
    EXPECT_EQ(d, array.get<double>(4));
    EXPECT_EQ(0, strcmp(s, array.getArray(12)));
    EXPECT_EQ(i, array.get<int>(17));
    EXPECT_EQ(d, array.get<double>(21));
    EXPECT_EQ(0, strcmp(s, array.getArray(29)));
}

TEST(PackedArrayTest, Shrink) {
    /* PackedArray shrinks itself to a minimum size correctly */

    PackedArray array;

    // initially empty
    EXPECT_EQ(0, array.size());
    EXPECT_EQ(0, array.capacity());

    // insert 10 integers
    for (int i = 0; i < 10; ++i)
        array.append(i); // append an int
    int newSize = 10 * sizeof(int);

    // after inserting 10 integers
    EXPECT_EQ(newSize, array.size());
    EXPECT_EQ(INITIAL_CAPACITY, array.capacity());

    // all integers can be retrieved
    for (int i = 0; i < 10; ++i) EXPECT_EQ(i, array.get<int>(i * sizeof(int)));

    array.shrink();

    // after shrink
    EXPECT_EQ(newSize, array.size());
    EXPECT_EQ(newSize, array.capacity());

    // all integers can still be retrieved
    for (int i = 0; i < 10; ++i) EXPECT_EQ(i, array.get<int>(i * sizeof(int)));
}
