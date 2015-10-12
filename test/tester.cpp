
#include "gtest/gtest.h"
#include <cmath>
#include <cstdio>
#include <chrono>
#include <iostream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "../src/succinct_tree.hpp"
#include "../src/parser.hpp"
#include "../src/serialize.hpp"

using namespace std;
using namespace std::chrono;
using namespace rapidjson;

high_resolution_clock::time_point t1, t2, bt1, bt2;
bool dot1 = false, dobt1 = false;

long long tick(string msg="time", bool bt=false);
Document wow2(int k = 1);

char fn[100];

Document wow(int k = 1);
void filename(int k, char *fin=fn);
void filenamec(int k, char *fin=fn);
void filenameh(int k, char *fin=fn);
void log_parser_size(Parser&, long long, int);
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

TEST(SuccinctTreeTest, DocumentDFS) {
    Document d = wow(2);
    SuccinctTree tree(d, 8);

    ASSERT_EQ(tree.N, 8);
    ASSERT_EQ(tree.size(), 18);
    int t[] = {1,1,0,1,1,0,1,1,0,1,0,1,0,1,0,0,0,0};
    for (int i = 0; i < tree.size(); ++i)
        ASSERT_EQ(tree.bv[i], t[i]);
}

TEST(ParserTest, Tree) {
    Document d = wow(2);
    Parser p(d);

    ASSERT_EQ(p.tree.N, 8);
    int t[] = {1,1,0,1,1,0,1,1,0,1,0,1,0,1,0,0,0,0};
    for (int i = 0; i < 18; ++i)
        ASSERT_EQ(p.tree.bv[i], t[i]);
}

TEST(ParserTest, NamesAndValues) {
    Document d = wow(2);
    Parser p(d);

    int n = 8;
    ASSERT_EQ(n, p.namess.size());

    string names[] = {"foo", "st", "name", "grades", "", "", "", ""};
    for (int i = 0; i < n; ++i)
        ASSERT_EQ(names[i], p.namess[i]);

    // ASSERT_EQ(p.valuen, 6);
}

TEST(ParserTest, Dblp) {
    // DBLP json file found at http://projects.csail.mit.edu/dnd/DBLP/
    Document d = wow(3);
    Parser p(d);

    // print_parser(p);
}

TEST(SerializationTest, get_size) {
    Document d = wow(2);
    Parser p(d);

    ASSERT_EQ(3, get_size(p.tree));
    ASSERT_EQ(39, get_size(p.namess));
    ASSERT_EQ(57, get_size(p.valuess));
    ASSERT_EQ(103, get_size(p));

    ASSERT_EQ(4, get_size(p.namess.bv));
}

TEST(SerializationTest, SuccinctTree) {
    Document d = wow(2);
    int size = 8;
    SuccinctTree s(d, size);

    char *p = s.to_char_array();
    for (int i = 0; i < s.size(); ++i)
        ASSERT_EQ(s.bv[i], (p[i/8] & (1 << (7-(i%8))) ? 1:0));

    SuccinctTree sa(p, s.size());
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

TEST(SerializationTest, ParserSize) {
    // for (int i = 5; i <= 9; ++i) {
    for (int i = 0; i <= 0; ++i) {
            // cout << "start" << endl;
        Document d = wow(i);
            // cout << "loaded with rapidjson" << endl;
        tick();
        Parser p(d);
        long long dur = tick();
            // cout << "constructed parser" << endl;
        filenamec(i);
        save_to_file(p, string(fn));
        log_parser_size(p, dur, i);
    }
}

TEST(SerializationTest, ParserDeserialization) {
    // for (int i = 5; i <= 9; ++i) {
    for (int i = 5; i <= 9; ++i) {
            // cout << "start" << endl;
        Document d = wow(i);
            // cout << "loaded with rapidjson" << endl;
        Parser p(d);
            // cout << "constructed parser" << endl;
        filenamec(i);
        save_to_file(p, string(fn));

        tick();
        Parser loaded = load_from_file(string(fn));
        long long dur = tick();
        EXPECT_EQ(p.size, loaded.size);
        EXPECT_EQ(p.tree, loaded.tree);
        EXPECT_EQ(p.namess, loaded.namess);
        EXPECT_EQ(p.valuess, loaded.valuess);
        ASSERT_EQ(p, loaded);
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

void filename(int k, char *fn) {
    sprintf(fn, "test/sample%d.json", k);
    if (k == 10) sprintf(fn, "test/dblp_.json");
    if (k == 11) sprintf(fn, "test/dblp.json");
}

void filenamec(int k, char *fn) {
    filename(k, fn);
    sprintf(fn, "%s_c", fn);
}

void filenameh(int k, char *fn) {
    filename(k, fn);
    sprintf(fn, "%s_h", fn);
}

Document wow(int k) {
    Document d;
    char fn[100];
    filename(k, fn);
    FILE *fp = fopen(fn, "r");
    if (!fp) sprintf(fn, "../%s", fn), fp = fopen(fn, "r");
    if (!fp) {
        cout << "line: " << fn << endl;
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

void write_formatted(ofstream &f, const string &msg, long &a, size_t &b) {
    f << msg << "\t\t(" << (int)((double)a/b*100) << "%)\t\t" << a << endl;
}

void log_parser_size(Parser &obj, long long dur, int k) {
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
    names = get_size(obj.namess);
    s += names;
    // size of values
    values = get_size(obj.valuess);
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
    high_resolution_clock::time_point *ts, *te;
    bool *do1;
    bt ? (ts = &bt1, te = &bt2, do1 = &dobt1) : (ts = &t1, te = &t2, do1 = &dot1);
    *do1 ^= 1;
    if (*do1) {
        cout << "Timer start..." << endl;
        *ts = high_resolution_clock::now();
        return duration_cast<milliseconds>(*te - *ts).count();
    }
    else {
        *te = high_resolution_clock::now();
        // auto tot_time = duration_cast<microseconds>(*te - *ts).count();
        auto tot_time = duration_cast<milliseconds>(*te - *ts).count();
        cout << msg << ": " << tot_time << " millis" << endl;
        return tot_time;
    }
}


