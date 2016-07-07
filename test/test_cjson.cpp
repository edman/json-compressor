
#include "gtest/gtest.h"
#include "test_utils.hpp"
#include "../src/bp_tree.hpp"
#include "../src/df_tree.hpp"
#include "../src/cjson.hpp"
#include "../src/serialize.hpp"

using namespace std;
using namespace rapidjson;

TEST(CjsonTest, Names) {
    /* Cjson creates correct names table */

    Document d = rapid_from_file(2);
    Cjson<BpTree> p(d); // indifferent with respect to tree type

    int n = 4;
    ASSERT_EQ(n, p.names.size());

    string names[] = {"foo", "st", "name", "grades"};
    for (int i = 0; i < n; ++i)
        EXPECT_EQ(names[i], p.names[i]);
}

TEST(CjsonTest, Values) {
    /* Cjson loads and retrieves values correctly */

    Document d = rapid_from_file(2);
    CjsonBp p(d);

    for (int i = 0; i < p.names.size(); ++i)
        cout << i << "\t" << p.names[i] << endl;
    for (int i = 0; i < p.size; ++i) {
        const Jval &v = p[i];
        cout << v << endl;
        EXPECT_EQ(v.nameId(), v.hasName() ? v.nameId() : MAX_UINT);
    }
}

TEST(CjsonTest, GetSize) {
    /* Cjson get_size reports its size accurately */

    Document d = rapid_from_file(2);

    // bp tree
    // Cjson<BpTree> pb(d);
    CjsonBp pb(d);
    EXPECT_EQ(3, get_size(pb.tree));
    EXPECT_EQ(20, get_size(pb.names));
    EXPECT_EQ(133, get_size(pb.values));
    EXPECT_EQ(164, get_size(pb));

    // df tree
    // Cjson<DfTree> pd(d);
    CjsonDf pd(d);
    EXPECT_EQ(3, get_size(pd.tree));
    EXPECT_EQ(20, get_size(pd.names));
    EXPECT_EQ(133, get_size(pd.values));
    EXPECT_EQ(164, get_size(pd));
}

TEST(CjsonTest, Serialization) {
    /* Cjson serializes correctly to a compressed file */

    string fn = filename(2);
    Document d = rapid_from_file(fn);
    // load cjson
    Cjson<BpTree> p(d);
    // save to compressed file
    save_to_file(p, fn + "_c");

    // load new cjson from compressed file
    Cjson<BpTree> loaded = load_from_file<BpTree>(fn + "_c");
    // assert initial cjson equals the loaded object
    EXPECT_EQ(p.size, loaded.size);
    EXPECT_EQ(p.tree, loaded.tree);
    for (int i = 0; i < p.names.size(); ++i)
        EXPECT_EQ(strcmp(p.names[i], loaded.names[i]), 0);
    EXPECT_EQ(p.values, loaded.values);
    for (int i = 0; i < p.stringValues.size(); ++i)
        EXPECT_EQ(strcmp(p.stringValues[i], loaded.stringValues[i]), 0);
    EXPECT_EQ(p, loaded);
}

TEST(CjsonTest, SplitSerialization) {
    /* Cjson serializes correctly to split compression files */
    string fn = filename(2);
    Document d = rapid_from_file(fn);
    // load cjson
    Cjson<BpTree> p(d);
    // save to split compressed file
    save_to_file_split(p, fn);

    // load new cjson from split compressed file
    Cjson<BpTree> loaded = load_from_file_split<BpTree>(fn);
    // assert initial cjson equal loaded object
    EXPECT_EQ(p.size, loaded.size);
    EXPECT_EQ(p.tree, loaded.tree);
    for (int i = 0; i < p.names.size(); ++i)
        EXPECT_EQ(strcmp(p.names[i], loaded.names[i]), 0);
    EXPECT_EQ(p.values, loaded.values);
    EXPECT_EQ(p, loaded);
}

