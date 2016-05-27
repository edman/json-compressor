
#ifndef __DFTREE_CPP__
#define __DFTREE_CPP__

#include "df_tree.hpp"
#include "jvalue.hpp"
#include "rapidjson/document.h"

using namespace std;
using namespace rapidjson;
using namespace sdsl;

static int counter = 0;

DfTree::DfTree(Value &document, int doc_size) {
    N = doc_size;
    bv = bit_vector(size(), 0);

    // Artificial first open parenthesis
    bv[0] = counter = 1;
    documentDfs(document);
}

DfTree::DfTree(char *p, int size_in_bits) {
    bv = bit_vector(size_in_bits, 0);
    for (int i = 0; i < size_in_bits; ++i)
        bv[i] = p[i / 8] & (1 << (7 - (i % 8)));
    N = (size_in_bits - 1) >> 1;
}

char* DfTree::to_char_array() const {
    char *p = new char[size_in_bytes()];
    for (int i = 0; i < size_in_bytes(); ++i)
        p[i] = 0;
    for (int i = 0; i < size(); ++i)
        p[i / 8] |= bv[i] << (7 - (i % 8));
    return p;
}

int DfTree::size_in_bytes() const {
    return (N + 1 + 3) / 4;
}

int DfTree::size() const {
    return N ? (N + 1) * 2 : 0;
}

void DfTree::documentDfs(Value &d) {
    if (d.IsObject()) {
        for (int i = 0; i < d.MemberCount(); ++i)
            bv[counter++] = 1; // Set a 1 for each child
        counter++; // Leave a 0 for the current node after its children
        for (auto it = d.MemberBegin(); it != d.MemberEnd(); ++it) {
            documentDfs(it->value);
        }
    }
    else if (d.IsArray()) {
        for (int i = 0; i < d.Size(); ++i)
            bv[counter++] = 1; // Set a 1 for each child
        counter++; // Leave a 0 for the current node after its children
        for (auto it = d.Begin(); it != d.End(); ++it) {
            documentDfs(*it);
        }
    }
    else {
        // not an object nor an array, just leave a 0 in the bit vector
        counter++;
    }
}

bool DfTree::operator==(const DfTree &rhs) const {
    if (N != rhs.N) return false;
    for (int i = 0; i < size(); ++i) if (bv[i] != rhs.bv[i]) return false;
    return true;
}


ostream& operator<<(ostream &o, const DfTree &t) {
    o<<"("<<t.N<<",";
    for (int i = 0; i < t.size(); ++i) o<<t.bv[i];
    return o<<")";
}

#endif
