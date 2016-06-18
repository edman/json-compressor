
#include "gtest/gtest.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"

using namespace std;
using namespace rapidjson;

TEST(RapidJsonTest, ParseString) {
    /* RapidJson parsing json from string */

    // 1. Parse a JSON string into DOM.
    const char* json = "{\"project\":\"rapidjson\",\"stars\":10}";
    Document d;
    d.Parse(json);

    // 2. Modify it by DOM.
    Value& s = d["stars"];
    ASSERT_EQ(s.GetInt(), 10); // assert value was read correctly

    s.SetInt(s.GetInt() + 1);
    ASSERT_EQ(s.GetInt(), 11); // assert value was updated correctly

    // 3. Stringify the DOM
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);

    string exp = "{\"project\":\"rapidjson\",\"stars\":11}";
    ASSERT_EQ(buffer.GetString(), exp); // assert string was well generated
    // Output {"project":"rapidjson","stars":11}
    // cout << buffer.GetString() << endl;
}

TEST(RapidJsonTest, ParseJsonFile) {
    /* RapidJson parsing json from file */

    FILE *fp = fopen("test/sample2.json", "r");
    char buf[2 << 16];
    FileReadStream is(fp, buf, sizeof(buf));

    Document d;
    d.ParseStream(is);
    fclose(fp);

    Value &s = d["foo"];
    ASSERT_EQ(s.GetType(), 5);
    ASSERT_EQ(string(s.GetString()), "bar");
}
