
#ifndef __SUCCINCTTREE_CPP__
#define __SUCCINCTTREE_CPP__

#include "succinct_tree.hpp"
#include "rapidjson/document.h"

using namespace std;
using namespace rapidjson;
using namespace sdsl;

int counter = 0;

SuccinctTree::SuccinctTree(Value &document) {
    bv = bit_vector(100, 0);

    counter = 0;
    cout << "size: " << bv.size() << endl;
    documentDfs(document);
    N = (counter - 1) >> 1;
    cout << endl << "counter " << counter << endl;
}

//  { "Null", "False", "True", "Object", "Array", "String", "Number" };

void SuccinctTree::documentDfs(Value &d) {
    bv[counter++] = 1;
    cout << bv[counter-1];
    // cout << counter;

    if (d.IsObject()) {
        // cout << "{ ";
        for (auto it = d.MemberBegin(); it != d.MemberEnd(); ++it) {
            // cout << it->name.GetString() << " ";
            documentDfs(it->value);
        }
        // cout << "} ";
    }
    else if (d.IsArray()) {
        // cout << "[ ";
        for (auto it = d.Begin(); it != d.End(); ++it) {
            // cout << "x ";
            documentDfs(*it);
        }
        // cout << "] ";
    }

    counter++;
    cout << bv[counter-1];
}

#endif

