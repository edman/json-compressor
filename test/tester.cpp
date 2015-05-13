
#include "gtest/gtest.h"
#include <cmath>
#include <iostream>
#include "../src/bla.hpp"
using namespace std;

TEST (SquareRootTest, PositiveNos) {
    EXPECT_EQ(2.0, sqrt(4.0));
    EXPECT_EQ(4, mult2(2.0));
}

