
#include "gtest/gtest.h"
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

high_resolution_clock::time_point t1, t2, bt1, bt2;
bool timerStarted = false;
bool dot1 = false, dobt1 = false;

long long tick(string msg="time", bool bt=false);
long long average(long long *a, int n);
Document wow2(int k = 1);
template <class T> void traversalDfs(T&);
void traversalRapid(Value&);
void doubleTraversalCjson(DfTraversal &tdf, BpTraversal &tbp);

char fn[100];

Document wow(int k = 1, bool input = false);
void filename(int k, char *fin=fn);
void filenamec(int k, char *fin=fn);
void filenameh(int k, char *fin=fn);
template <class T> void log_cjson_size(Cjson<T>&, long long, int);
long filesize(int k = 1);

TEST(RapidJsonTest, ReadingStuff) {
    // 1. Parse a JSON string into DOM.
    const char* json = "{\"project\":\"rapidjson\",\"stars\":10}";
    Document d;
    d.Parse(json);

    // 2. Modify it by DOM.
    Value& s = d["stars"];
    s.SetInt(s.GetInt() + 1);
    // cout << "stars: " << s.GetInt() << endl;

    // 3. Stringify the DOM
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);

    // Output {"project":"rapidjson","stars":11}
    // cout << buffer.GetString() << endl;
}

TEST(BpTreeTest, DocumentDFS) {
    Document d = wow(2);
    BpTree tree(d, 8);

    ASSERT_EQ(tree.N, 8);
    ASSERT_EQ(tree.size(), 18);
    int t[] = {1,1,0,1,1,0,1,1,0,1,0,1,0,1,0,0,0,0};
    for (int i = 0; i < tree.size(); ++i)
        ASSERT_EQ(tree.bv[i], t[i]);
}

TEST(DfTreeTest, DocumentDFS) {
    Document d = wow(2);
    DfTree tree(d, 8);

    ASSERT_EQ(tree.N, 8);
    ASSERT_EQ(tree.size(), 18);
    int t[] = {1,1,1,0,0,1,1,0,0,1,1,1,1,0,0,0,0,0};
    for (int i = 0; i < tree.size(); ++i)
        ASSERT_EQ(tree.bv[i], t[i]);
}

TEST(CjsonBpTreeTest, Tree) {
    Document d = wow(2);
    Cjson<BpTree> p(d, true);

    ASSERT_EQ(p.tree.N, 8);
    int t[] = {1,1,0,1,1,0,1,1,0,1,0,1,0,1,0,0,0,0};
    for (int i = 0; i < 18; ++i)
        ASSERT_EQ(p.tree.bv[i], t[i]);
}

TEST(CjsonBpTreeTest, NamesAndValues) {
    Document d = wow(2);
    Cjson<BpTree> p(d);

    int n = 5;
    ASSERT_EQ(n, p.names.size());

    string names[] = {"foo", "st", "name", "grades", ""};
    for (int i = 0; i < n; ++i)
        ASSERT_EQ(names[i], p.names[i]);
}

TEST(CjsonBpTreeTest, Dblp) {
    // DBLP json file found at http://projects.csail.mit.edu/dnd/DBLP/
    Document d = wow(3);
    Cjson<BpTree> p(d);

    // print_cjson(p);
}

TEST(CjsonDfTreeTest, NamesAndValues) {
    Document d = wow(2);
    Cjson<DfTree> p(d);

    int n = 5;
    ASSERT_EQ(n, p.names.size());

    string names[] = {"foo", "st", "name", "grades", ""};
    for (int i = 0; i < n; ++i)
        ASSERT_EQ(names[i], p.names[i]);
}

TEST(CjsonDfTreeTest, Dblp) {
    // DBLP json file found at http://projects.csail.mit.edu/dnd/DBLP/
    Document d = wow(3);
    Cjson<DfTree> p(d);

    // print_cjson(p);
}

TEST(SerializationTest, get_size) {
    Document d = wow(2);
    Cjson<BpTree> pb(d);

    ASSERT_EQ(3, get_size(pb.tree));
    // ASSERT_EQ(39, get_size(pb.names));
    ASSERT_EQ(48, get_size(pb.values));
    ASSERT_EQ(111, get_size(pb));
    // ASSERT_EQ(4, get_size(pb.names.bv));

    Cjson<DfTree> pd(d);

    ASSERT_EQ(3, get_size(pd.tree));
    // ASSERT_EQ(39, get_size(pd.names));
    ASSERT_EQ(48, get_size(pd.values));
    ASSERT_EQ(111, get_size(pd));
}

TEST(SerializationTest, BpTree) {
    Document d = wow(2);
    int size = 8;
    BpTree s(d, size);

    char *p = s.to_char_array();
    for (int i = 0; i < s.size(); ++i)
        ASSERT_EQ(s.bv[i], (p[i/8] & (1 << (7-(i%8))) ? 1:0));

    BpTree sa(p, s.size());
    delete[] p;
    ASSERT_EQ(sa, s);

    // cout << s<<endl;;
    // cout << sa<< endl;;
}

TEST(SerializationTest, IntAndString) {
    int v = 10;
    StreamType res;
    serialize(v, res);

    EXPECT_EQ(sizeof(uint32_t), res.size());
    EXPECT_EQ(res, std::vector<uint8_t>({0xA, 0, 0, 0}));

    string s = "mom sent me home";
    serialize(s, res);

    ofstream mfile;
    mfile.open("test/tfile.bin", ios::binary);
    mfile.write((char*) &res[0], res.size());
    mfile.close();

    ifstream ifile;
    ifile.open("test/tfile.bin", ios::binary);

    ifile.seekg(0, ifile.end);
    long size = ifile.tellg();
    ifile.seekg(0);
    StreamType stream(size);
    ifile.read((char*) &stream[0], size);
    ifile.close();

    StreamType::const_iterator it = stream.begin();
    int i = deserialize<int>(it, stream.end());
    string t = deserialize<string>(it, stream.end());
    ASSERT_EQ(i, v);
    ASSERT_EQ(s, t);
}

TEST(SerializationTest, CjsonSize) {
    // for (int i = 5; i <= 9; ++i) {
    for (int i = 1; i <= 1; ++i) {
            cout << "start" << endl;
        Document d = wow(i);
            cout << "loaded with rapidjson" << endl;
        tick();
        Cjson<BpTree> p(d);
        long long dur = tick();
            cout << "constructed cjson" << endl;
        filenamec(i);
            cout << "filenamec" << endl;
        save_to_file<BpTree>(p, string(fn));
            cout << "save to file" << endl;
        log_cjson_size<BpTree>(p, dur, i);
            cout << "log cjson size" << endl;
    }
}

TEST(SerializationTest, CjsonDeserialization) {
    // for (int i = 5; i <= 9; ++i) {
    for (int i = 5; i <= 5; ++i) {
            // cout << "start" << endl;
        Document d = wow(i);
            // cout << "loaded with rapidjson" << endl;
        Cjson<BpTree> p(d);
            // cout << "constructed cjson" << endl;
        filenamec(i);
        save_to_file(p, string(fn));

        tick();
        Cjson<BpTree> loaded = load_from_file<BpTree>(string(fn));
        long long dur = tick();
        cout << "duration " << dur << endl;
        EXPECT_EQ(p.size, loaded.size);
        EXPECT_EQ(p.tree, loaded.tree);
        for (int i = 0; i < p.names.size(); ++i)
            EXPECT_EQ(strcmp(p.names[i], loaded.names[i]), 0);
        EXPECT_EQ(p.values, loaded.values);
        ASSERT_EQ(p, loaded);
    }
}

TEST(SplitSerializationTest, CjsonDeserialization) {
    // for (int i = 12; i <= 13; ++i) { // comment for speed
    for (int i = 1; i <= 1; ++i) {
        Document d = wow(i);
        Cjson<BpTree> p(d);
        filenamec(i);
        save_to_file_split(p, string(fn));

        tick();
        Cjson<BpTree> loaded = load_from_file_split<BpTree>(string(fn));
        long long dur = tick();
        cout << "duration " << dur << endl;
        EXPECT_EQ(p.size, loaded.size);
        EXPECT_EQ(p.tree, loaded.tree);
        for (int i = 0; i < p.names.size(); ++i)
            EXPECT_EQ(strcmp(p.names[i], loaded.names[i]), 0);
        EXPECT_EQ(p.values, loaded.values);
        ASSERT_EQ(p, loaded);
    }
}

TEST(TraversalTest, Traversal) {
    Document d = wow(2);
    Cjson<BpTree> p(d);

    BpTraversal t(&p);
    cout << "bptree " << t.cjson->tree << endl;
    traversalDfs(t);

    Cjson<DfTree> pd(d);
    DfTraversal td(&pd);
    cout << "dftree " << td.cjson->tree << endl;
    traversalDfs(td);

    // Cjson<DfTree> pd(d);
    // Traversal<DfTree> td(p);
    // cout << "dftree " << td.cjson.tree << endl;

    // cout << "root " << t << endl;
    // EXPECT_TRUE(t.hasChild());
    // EXPECT_TRUE(t.goToChild());
    // cout << "to child " << t << endl;
    // cout << "current node " << t.getCurrentNode() << endl;
    // EXPECT_TRUE(t.hasNextSibling());
}

TEST(TraversalTest, Time) {
    int lo = 1;
    int hi = 1;
    int tries = 1;
    long long times[tries];
    for (int i = lo; i <= hi; ++i) {
        cout << endl << "=== (parse input " << i << " ";
        Document d = wow(i, true);
        cout << fn << ") ===" << endl;

        cout << "= dftree create" << endl;
        Cjson<DfTree> pd(d);
        DfTraversal td(&pd);

        cout << "number of nodes (tree) " << pd.tree.N << endl;
        cout << "number of nodes (cjson) " << pd.size << endl;

        cout << "= dftree begin traversal" << endl;
        for (int k = 0; k < tries; ++k) {
            tick();
            traversalDfs(td);
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

template <class T> void traversalDfs(T& t) {
    // cout << "dfs node " << t << " " << t.getCurrentNode() << " ";
    // cout << t.hasChild() << " " << t.hasNextSibling() << " ";
    // // cout << t.bp.find_close(t.treeIndex) << " ";
    // cout << endl;
    // auto node = t.getCurrentNode();
    // if (t.hasChild()) {
    //     t.goToChild();
    if (t.goToChild()) {
        traversalDfs(t);
        t.goToParent();
    }
    if (t.goToNextSibling()) traversalDfs(t);
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

long filesize(int k) {
    char fn[100];
    filename(k, fn);
    ifstream ifile;
    ifile.open(fn, ios::binary);
    ifile.seekg(0, ifile.end);
    long size = ifile.tellg();
    ifile.close();
    return size;
}

void input_filename(int k, char *fn) {
    sprintf(fn, "test/inputs/input%d.json", k);
}

void filename(int k, char *fn) {
    sprintf(fn, "test/sample%d.json", k);
    if (k == 10) sprintf(fn, "test/dblp_.json");
    if (k == 11) sprintf(fn, "test/dblp.json");
    if (k == 12) sprintf(fn, "test/dataset/r1.json");
    if (k == 13) sprintf(fn, "test/dataset/exi.json");
}

void filenamec(int k, char *fn) {
    filename(k, fn);
    sprintf(fn, "%s_c", fn);
}

void filenameh(int k, char *fn) {
    filename(k, fn);
    sprintf(fn, "%s_h", fn);
}

Document rapid_from_file(char *filename) {
    Document d;
    FILE *fp = fopen(filename, "r");
    if (!fp) sprintf(filename, "../%s", filename), fp = fopen(filename, "r");
    if (!fp) {
        cout << "line: " << filename << endl;
        cout << "Sample file not found\n";
        return d;
    }

    char buf[2 << 16];
    FileReadStream is(fp, buf, sizeof(buf));
    d.ParseStream(is);
    fclose(fp);

    // StringBuffer buffer;
    // Writer<StringBuffer> writer(buffer);
    // d.Accept(writer);
    // cout << buffer.GetString() << endl;
    //
    // for (Value::ConstMemberIterator it = d.MemberBegin(); it != d.MemberEnd(); ++it) {
    //     cout << "Name: " << it->name.GetString() << endl;
    //     cout << "Value type: " << it->value.GetType() << endl;
    // }

    // d.Accept(writer);
    // cout << buffer.GetString() << endl;

    return d;
}

Document wow(int k, bool input) {
    if (input) input_filename(k, fn);
    else filename(k, fn);
    return rapid_from_file(fn);
}

void write_formatted(ofstream &f, const string &msg, long &a, size_t &b) {
    f << msg << "\t\t(" << (int)((double)a/b*100) << "%)\t\t" << a << endl;
}

template <class T>
void log_cjson_size(Cjson<T> &obj, long long dur, int k) {
    filenameh(k);
    ofstream mfile(fn, ios::out);
    // original file size
    long orig = filesize(k);
    mfile << fn << endl;
    mfile << "original size (bytes): " << orig << endl;

    long header, tree, names, values;
    // size of header
    size_t s = sizeof(int);
    header = s;
    // size of tree
    tree = get_size(obj.tree);
    s += tree;
    // size of names
    names = get_size(obj.names);
    s += names;
    // size of values
    values = get_size(obj.values);
    s += values;

    write_formatted(mfile, ".header", header, s);
    write_formatted(mfile, ".tree", tree, s);
    write_formatted(mfile, ".names", names, s);
    write_formatted(mfile, ".values", values, s);
    mfile << "Total " << s << endl;

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
