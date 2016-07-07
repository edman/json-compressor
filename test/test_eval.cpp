
#include "gtest/gtest.h"
#include "test_utils.hpp"
#include "rapidjson/document.h"
#include "../src/bp_tree.hpp"
#include "../src/df_tree.hpp"
#include "../src/cjson.hpp"
#include "../src/bp_traversal.hpp"
#include "../src/df_traversal.hpp"
#include "../src/serialize.hpp"

using namespace std;
using namespace rapidjson;

TEST(EvalTest, CjsonSaveToFile) {
    /* Cjson compressed file size evaluation */

    int lo = 1;
    int hi = 0;
    for (int i = lo; i <= hi; ++i) {
            cout << "start " << i << endl;
        string fn = filename(i, true);
        Document d = rapid_from_file(fn);
            cout << "loaded " << fn << " with rapidjson" << endl;

        tick();
        Cjson<BpTree> p(d);
        long long dur = tick();
            cout << "constructed cjson (size=" << get_size(p) << ")" << endl;
            cout << ".. cjson number of nodes: " << p.size << endl;
            cout << ".. values packed array size: " << p.values._array.size() << endl;
        // save_to_file_split<BpTree>(p, fn);
        save_to_file<BpTree>(p, fn + "_c");
        //     cout << "save to file" << endl;
        // log_cjson_size<BpTree>(p, dur, fn);
        //     cout << "log cjson size" << endl;
    }
}

TEST(EvalTest, CjsonDeserializationTime) {
    /* Cjson deserialization time evaluation */

    int lo = 1;
    int hi = 0;
    for (int i = lo; i <= hi; ++i) {
        string fn = filename(i, true);

        cout << "== loading " << fn << endl;
        // cout << "= rapidjson loading" << endl;
        // tick();
        // Document d = rapid_from_file(fn);
        // long long dur = tick();
        // cout << "time: " << dur << endl;

        cout << "= cjson bp loading" << endl;
        tick();
        Cjson<BpTree> loaded = load_from_file<BpTree>(fn + "_c");
        long long dur = tick();
        cout << "time: " << dur << endl;
        cout << "size " << loaded.size << endl;
        cout << "names size " << loaded.names.size() << endl;
    }
}

TEST(EvalTest, TraversalTime) {
    /* Cjson tree traversal time evaluation */

    int lo = 1;
    int hi = 0;
    int tries = 5;
    long long times[tries];
    for (int i = lo; i <= hi; ++i) {
        cout << endl << "=== (parse input " << i << " ";
        string fn = filename(i, true);
        // string fn = filename(i, false);
        Document d = rapid_from_file(fn);
        cout << fn << ") ===" << endl;

        cout << "= dftree create" << endl;
        Cjson<DfTree> pd(d);
        DfTraversal td(&pd);

        cout << "= dftree begin traversal" << endl;
        for (int k = 0; k < tries; ++k) {
            tick();
            // traversalDfs(td);
            traverserDfs(td.getTraverser());
            // traverserBfs(td.getTraverser());
            times[k] = tick("cjson dftree time");
        }
        cout << "average: " << average(times, tries) << endl;

        cout << "= bptree create" << endl;
        Cjson<BpTree> p(d);
        BpTraversal t(&p);
        cout << "= bptree begin traversal" << endl;
        for (int k = 0; k < tries; ++k) {
            tick();
            traversalDfs(t);
            times[k] = tick("cjson bptree time");
        }
        cout << "average: " << average(times, tries) << endl;

        cout << "= rapid begin traversal" << endl;
        for (int k = 0; k < tries; ++k) {
            tick();
            traversalRapid(d);
            times[k] = tick("rapid time");
        }
        cout << "average: " << average(times, tries) << endl;

        // cout << "= begin double traversal" << endl;
        // doubleTraversalCjson(td, t);
    }
}
