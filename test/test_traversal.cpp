
#include "gtest/gtest.h"
#include "test_utils.hpp"
#include "rapidjson/document.h"
#include "../src/bp_tree.hpp"
#include "../src/df_tree.hpp"
#include "../src/cjson.hpp"
#include "../src/bp_traversal.hpp"
#include "../src/df_traversal.hpp"

using namespace std;
using namespace rapidjson;

TEST(TraversalTest, Traversal) {
    /* Traversals are executed correctly */

    Document d = rapid_from_file(2);
    Cjson<BpTree> p(d);

    BpTraversal t(&p);
    traversalDfs(t);

    Cjson<DfTree> pd(d);
    DfTraversal td(&pd);
    traversalDfs(td);
}
