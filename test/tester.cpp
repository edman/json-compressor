
#include "gtest/gtest.h"
#include <cmath>
#include <cstdio>
#include <iostream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "../src/succinct_tree.hpp"
#include "../src/parser.hpp"
#include "../src/serialize.hpp"

using namespace std;
using namespace rapidjson;

Document wow(int k = 1);
void print_parser(Parser &p);

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

TEST(ParserTest, TreeNamesAndValues) {
    Document d = wow(2);
    Parser p(d);

   ASSERT_EQ(p.tree.N, 8);
    int t[] = {1,1,0,1,1,0,1,1,0,1,0,1,0,1,0,0,0,0};
    for (int i = 0; i < 18; ++i)
        ASSERT_EQ(p.tree.bv[i], t[i]);

    ASSERT_EQ(p.namen, 4);
    string names[] = {"foo", "st", "name", "grades"};
    for (int i = 0; i < 4; ++i)
        ASSERT_EQ(p.names[i], names[i]);

    ASSERT_EQ(p.valuen, 6);
    // print_parser(p);
}

TEST(ParserTest, Dblp) {
    // DBLP json file found at http://projects.csail.mit.edu/dnd/DBLP/
    Document d = wow(3);
    Parser p(d);

    // print_parser(p);
}

TEST(Serialization, get_size) {
    Document d = wow(2);
    Parser p(d);
    int size = get_size(p);
    ASSERT_EQ(size, 153);

    ASSERT_EQ(p.tree.size_in_bytes(), 3);
}

TEST(Serialization, SuccinctTree) {
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

TEST(Serialization, IntAndString) {
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

TEST(Serialization, SaveLoadFile) {
    // string fn = "test/tfile.bin";
    for (int i = 1; i <= 3; ++i) {
        Document d = wow(i);
        Parser p(d);

        save_to_file(p, "test/tfile.bin");
        Parser pa = load_from_file("test/tfile.bin");
        ASSERT_EQ(p, pa);
    }
}


void print_parser(Parser &p) {
    cout << "tree: " << p.tree << endl;
    cout << "names: ";
    for (int i = 0; i < p.namen; ++i)
        cout << p.names[i] << "|"; cout << endl;
    cout << "values: ";
    for (int i = 0; i < p.valuen; ++i)
        cout << p.values[i] << "|"; cout << endl;
    cout << "codes: ";
    for (int i = 0; i < p.size; ++i)
        cout << p.codes[i] << "|"; cout << endl;
}

Document wow(int k) {
    Document d;
    char fn[100];
    sprintf(fn, "../test/sample%d.json", k);
    FILE *fp = fopen(fn, "r");
    if (!fp) sprintf(fn, "test/sample%d.json", k), fp = fopen(fn, "r");
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
