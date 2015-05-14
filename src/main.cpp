#include <iostream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "bla.hpp"

#include <sdsl/suffix_arrays.hpp>
#include <fstream>

using namespace std;
using namespace rapidjson;
using namespace sdsl;

int main(int argc, char *argv[]) {
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

    csa_wt<> fm_index;
    construct_im(fm_index, "mississippi!", 1);
    cout << "'si' occurs " << count(fm_index,"si") << " times.\n";

    return 0;
}

