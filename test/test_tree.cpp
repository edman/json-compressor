
#include "gtest/gtest.h"
#include "test_utils.hpp"
#include "../src/bp_tree.hpp"
#include "../src/df_tree.hpp"

using namespace std;
using namespace rapidjson;

TEST(BpTreeTest, Construction) {
    /* BpTree construction */

    Document d = rapid_from_file(2);
    BpTree tree(d, 8);

    ASSERT_EQ(tree.N, 8);
    ASSERT_EQ(tree.size(), 18);
    int t[] = {1,1,0,1,1,0,1,1,0,1,0,1,0,1,0,0,0,0};
    for (int i = 0; i < tree.size(); ++i)
        ASSERT_EQ(tree.bv[i], t[i]);
}

TEST(DfTreeTest, Construction) {
    /* DfTree construction */

    Document d = rapid_from_file(2);
    DfTree tree(d, 8);

    ASSERT_EQ(8, tree.N);
    ASSERT_EQ(18, tree.size());
    int t[] = {1,1,1,0,0,1,1,0,0,1,1,1,1,0,0,0,0,0};
    for (int i = 0; i < tree.size(); ++i)
        ASSERT_EQ(t[i], tree.bv[i]);
}

TEST(BpTreeTest, ToCharArray) {
    /* BpTree is converted correctly from and to char arrays */

    Document d = rapid_from_file(2);
    int size = 8;
    BpTree s(d, size);

    char *p = s.to_char_array();
    for (int i = 0; i < s.size(); ++i)
        ASSERT_EQ((p[i/8] & (1 << (7-(i%8))) ? 1:0), s.bv[i]);

    BpTree sa(p, s.size());
    delete[] p;
    ASSERT_EQ(sa, s);
}

