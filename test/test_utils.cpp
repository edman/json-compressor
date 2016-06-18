
#include "gtest/gtest.h"
#include "test_utils.hpp"
#include <cmath>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <chrono>
#include <iostream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "../src/bp_tree.hpp"
#include "../src/df_tree.hpp"
#include "../src/cjson.hpp"
#include "../src/bp_traversal.hpp"
#include "../src/df_traversal.hpp"
#include "../src/serialize.hpp"

using namespace std;
using namespace std::chrono;
using namespace rapidjson;

/******************************************************************************/
/* File handling and logging */
long filesize(string fn) {
    ifstream ifile;
    ifile.open(fn, ios::binary);
    ifile.seekg(0, ifile.end);
    long size = ifile.tellg();
    ifile.close();
    return size;
}

string filename(int k, bool input) {
    if (input) return "test/inputs/input" + to_string(k) + ".json";
    return "test/sample" + to_string(k) + ".json";
}

void write_formatted(ofstream &f, const string &msg, long &a, size_t &b) {
    f << msg << "\t\t(" << (int)((double)a/b*100) << "%)\t\t" << a << endl;
}

template <class T>
void log_cjson_size(Cjson<T> &obj, long long dur, string &fn) {
    // original file size
    long orig = filesize(fn);
    long comp = filesize(fn + "_c");

    ofstream mfile(fn + "_h", ios::out);
    mfile << fn << endl;
    mfile << "original size (bytes): " << orig << endl;
    mfile << "compressed size (bytes): " << comp << endl;

    long header, tree, names, values;
    // long header, tree, names, nameList, values; // remove

    // size of header
    size_t s = sizeof(int);
    header = s;
    // size of tree
    tree = get_size(obj.tree);
    s += tree;
    // size of names (length of names table + table)
    names = sizeof(int) + get_size(obj.names);
    s += names;
    // size of namelist
    // nameList = get_size(obj.nameList); // remove
    // s += nameList; //remove
    // size of values
    values = get_size(obj.values);
    s += values;

    write_formatted(mfile, ".header", header, s);
    write_formatted(mfile, ".tree", tree, s);
    write_formatted(mfile, ".names", names, s);
    // write_formatted(mfile, ".nameList", nameList, s); // remove
    write_formatted(mfile, ".values", values, s);
    mfile << "Total " << s << endl;
    assert(s == get_size(obj));

    // compression ratio
    mfile << "Compression ratio (bigger is better): " << (double)orig/s << endl;
    mfile << "Compression time: " << dur << " millis" << endl;
    mfile << endl;

    cout << fn << endl;
    cout << "original size: " << orig << endl;
    cout << "compressed: " << s << endl;
    cout << "ratio (bigger better): " << (double)orig/s << endl << endl;
    // cout << "time: " << dur << "ms" << endl << endl;

    mfile.close();
}

template void log_cjson_size(Cjson<BpTree> &, long long, string&);
template void log_cjson_size(Cjson<DfTree> &, long long, string&);


/******************************************************************************/
/* Json parsing */
Document rapid_from_file(string fn) {
    Document d;
    FILE *fp = fopen(fn.c_str(), "r");
    if (!fp) { fn = "../" + fn; fp = fopen(fn.c_str(), "r"); }
    if (!fp) {cout << "'" << fn << "' not found" << endl; return d; }

    char buf[2 << 16];
    FileReadStream is(fp, buf, sizeof(buf));
    d.ParseStream(is);
    fclose(fp);
    return d;
}

Document rapid_from_file(int k, bool input) {
    return rapid_from_file(filename(k, input));
}


/******************************************************************************/
/* Time measurement and evaluation */
high_resolution_clock::time_point t1, t2, bt1, bt2;
bool timerStarted = false;
bool dot1 = false, dobt1 = false;

long long tick(string msg, bool bt) {
    timerStarted = !timerStarted;
    if (timerStarted) {
        // cout << ".. timer start" << endl;
        t1 = high_resolution_clock::now();
        // return duration_cast<milliseconds>(t1 - t2).count();
        return duration_cast<nanoseconds>(t1 - t2).count();
    }
    else {
        t2 = high_resolution_clock::now();
        // auto tot_time = duration_cast<milliseconds>(t2 - t1).count();
        // cout << ".. " << msg << ": " << tot_time << " millis" << endl;
        auto tot_time = duration_cast<nanoseconds>(t2 - t1).count();
        // if (!msg.empty()) cout << ".. " << msg << ": " << tot_time << " nanos" << endl;
        return tot_time;
    }
}

long long average(long long *a, int n) {
    sort(a, a+n);

    long long avg = 0;
    for (int i = n/4; i < n - (n/4); ++i) {
        avg += a[i];
    }
    return avg / (n - 2 * (n / 4));
}

/******************************************************************************/
/* Document traversal */
void doubleTraversalCjson(DfTraversal &tdf, BpTraversal &tbp) {
    // cout << "..DF: " << tdf << " " << tdf.getCurrentNode() << " | ";
    // cout << "BP: " << tbp << " " << tbp.getCurrentNode() << endl;
    ASSERT_EQ(tdf.getCurrentNode().value, tbp.getCurrentNode().value);

    if (tdf.hasChild()) { ASSERT_EQ(tbp.hasChild(), true);
        tdf.goToChild(); tbp.goToChild();

        doubleTraversalCjson(tdf, tbp);
        tdf.goToParent(); tbp.goToParent();
    } else { ASSERT_EQ(tbp.hasChild(), false); }
    if (tdf.goToNextSibling()) {
        ASSERT_EQ(tbp.goToNextSibling(), true);
        doubleTraversalCjson(tdf, tbp);
    } else { ASSERT_EQ(tbp.goToNextSibling(), false); }
}

void traversalRapid(Value &d) {
    if (d.IsObject()) {
        for (auto it = d.MemberBegin(); it != d.MemberEnd(); ++it) {
            traversalRapid(it->value);
        }
    }
    else if (d.IsArray()) {
        for (auto it = d.Begin(); it != d.End(); ++it) {
            traversalRapid(*it);
        }
    }
}

