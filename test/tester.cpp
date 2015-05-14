
#include "gtest/gtest.h"
#include <cmath>
#include <cstdio>
#include <iostream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "../src/bla.hpp"
using namespace std;
using namespace rapidjson;

int wow();

TEST (SquareRootTest, PositiveNos) {
    EXPECT_EQ(2.0, sqrt(4.0));
    EXPECT_EQ(4, mult2(2.0));
    wow();
}

int wow() {
    {
        // 1. Parse a JSON string into DOM.
        const char* json = "{\"project\":\"rapidjson\",\"stars\":10}";
        Document d;
        d.Parse(json);

        // 2. Modify it by DOM.
        Value& s = d["stars"];
        s.SetInt(s.GetInt() + 1);

        // 3. Stringify the DOM
        StringBuffer buffer;
        Writer<StringBuffer> writer(buffer);
        d.Accept(writer);

        // Output {"project":"rapidjson","stars":11}
        cout << buffer.GetString() << endl;

        sayHi();
        cout << "10 x 2 = " << mult2(10) << endl;
        cout << "20 x 2 = " << mult2(20) << endl;
    }

    FILE *fp = fopen("../test/sample1.json", "r");
    if (!fp) {
      cout << "Make sure you're running this from the bin folder\n";
      return 1;
    }

    char buf[2 << 16];
    FileReadStream is(fp, buf, sizeof(buf));
    Document d;
    d.ParseStream(is);
    fclose(fp);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);
    cout << buffer.GetString() << endl;

    for (Value::ConstMemberIterator it = d.MemberBegin(); it != d.MemberEnd(); ++it) {
        cout << "Name: " << it->name.GetString() << endl;
        cout << "Value type: " << it->value.GetType() << endl;
    }

    // d.Accept(writer);
    // cout << buffer.GetString() << endl;

    return 0;
}
