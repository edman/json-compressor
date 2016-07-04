
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

TEST(TraversalTest, ItWorks) {
    /* Traversals are executed correctly */

    Document d = rapid_from_file(2);
    Cjson<BpTree> p(d);

    BpTraversal t(&p);
    traversalDfs(t);

    Cjson<DfTree> pd(d);
    DfTraversal td(&pd);
    traversalDfs(td);
}

TEST(TraversalTest, CompareToRapid) {
    /* A DFS traversal of the document is the same in Cjson and in RapidJson */

    Document d = rapid_from_file(2);
    Cjson<DfTree> pd(d);
    DfTraversal td(&pd);
    const DfTraverser &traverser = td.getTraverser();

    doubleTraverserRapid(d, traverser, pd.stringValues);
}

TEST(TraversalTest, FullCompareToRapid) {
    /* A DFS traversal of the document is the same in Cjson and in RapidJson */

    for (int i = 1; i <= 6; ++i) {
        Document d = rapid_from_file(i);
        CjsonDf pd(d);
        DfTraversal td(&pd);
        const DfTraverser &traverser = td.getTraverser();

        doubleTraverserRapid(d, traverser, pd.stringValues);
    }
}
