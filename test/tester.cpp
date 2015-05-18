
#include "gtest/gtest.h"
#include <cmath>
#include <cstdio>
#include <iostream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "../src/bla.hpp"
#include "../src/succinct_tree.hpp"
#include "../src/parser.hpp"

using namespace std;
using namespace rapidjson;

Document wow(int k = 1);

TEST(SuccinctTreeTest, DocumentDFS) {
    Document d = wow();
    SuccinctTree tree(d);
}

TEST(ParserTest, NamesAndValues) {
    Document d = wow(2);
    Parser p(d);

    cout << "names# " << p.names.size() << endl;
    for (auto i = p.names.begin(); i != p.names.end(); ++i)
        cout << *i << "|"; cout << endl;
    cout << "values# " << p.values.size() << endl;
    for (auto i = p.values.begin(); i != p.values.end(); ++i)
        cout << *i << "|"; cout << endl;
}

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
