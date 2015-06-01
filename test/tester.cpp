
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
    SuccinctTree tree(d, 18);

    ASSERT_EQ(tree.N, 8);
    int t[] = {1,1,0,1,1,0,1,1,0,1,0,1,0,1,0,0,0,0};
    for (int i = 0; i < 18; ++i)
        ASSERT_EQ(tree.bv[i], t[i]);
}

TEST(ParserTest, TreeNamesAndValues) {
    Document d = wow(2);
    Parser p(d);

   ASSERT_EQ(p.tree.N, 8);
    int t[] = {1,1,0,1,1,0,1,1,0,1,0,1,0,1,0,0,0,0};
    for (int i = 0; i < 18; ++i)
        ASSERT_EQ(p.tree.bv[i], t[i]);

    cout << "size " <<p.namen<<endl;;
    ASSERT_EQ(p.namen, 4);
    string names[] = {"foo", "st", "name", "grades"};
    for (int i = 0; i < 4; ++i)
        ASSERT_EQ(p.names[i], names[i]);

    ASSERT_EQ(p.valuen, 6);
    for (int i = 0; i < p.namen; ++i)
        cout << p.names[i] << "|"; cout << endl;
    for (int i = 0; i < p.valuen; ++i)
        cout << p.values[i] << "|"; cout << endl;
}

TEST(ParserTest, Dblp) {
    // DBLP json file found at http://projects.csail.mit.edu/dnd/DBLP/
    Document d = wow(3);
    Parser p(d);

    cout << "tree: " << p.tree.bv << endl;
    cout << "names: ";
    for (int i = 0; i < p.namen; ++i)
        cout << p.names[i] << "|"; cout << endl;
    cout << "values: ";
    for (int i = 0; i < p.valuen; ++i)
        cout << p.values[i] << "|"; cout << endl;
    cout << "codes: " << p.size;
    for (int i = 0; i < p.size; ++i)
        cout << p.codes[i] << "|"; cout << endl;
}

TEST(Serialization, fstream) {
    ofstream mfile;
    mfile.open("test/tfile.bin", ios::binary);
    mfile << 387;
    mfile << "mom said I'm a grown man";
    mfile.close();

    ifstream ofile;
    ofile.open("test/tfile.bin", ios::binary);
    int i;
    ofile >> i;
    ofile.close();
    ASSERT_EQ(i, 387);
    cout << "h.. " << i << " " << sizeof(double) << endl;
}

TEST(Serialization, get_size) {
    Document d = wow(2);
    Parser p(d);
    int size = get_size(p);
    cout << "get_size: "<< size << endl;

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
