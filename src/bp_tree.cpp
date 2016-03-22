
#ifndef __BPTREE_CPP__
#define __BPTREE_CPP__

#include "bp_tree.hpp"
#include "rapidjson/document.h"

using namespace std;
using namespace rapidjson;
using namespace sdsl;

static int counter = 0;

BpTree::BpTree(Value &document, int doc_size) {
    N = doc_size;
    bv = bit_vector(size(), 0);

    counter = 0;
    documentDfs(document);
}

BpTree::BpTree(char *p, int size_in_bits) {
    bv = bit_vector(size_in_bits, 0);
    for (int i = 0; i < size_in_bits; ++i)
        bv[i] = p[i / 8] & (1 << (7 - (i % 8)));
    N = (size_in_bits - 1) >> 1;
}

char* BpTree::to_char_array() const {
    char *p = new char[size_in_bytes()];
    for (int i = 0; i < size_in_bytes(); ++i)
        p[i] = 0;
    for (int i = 0; i < size(); ++i)
        p[i / 8] |= bv[i] << (7 - (i % 8));
    return p;
}

int BpTree::size_in_bytes() const {
    return (N + 1 + 3) / 4;
}

int BpTree::size() const {
    return N ? (N + 1) * 2 : 0;
}

void BpTree::documentDfs(Value &d) {
    bv[counter++] = 1;

    if (d.IsObject())
        for (auto it = d.MemberBegin(); it != d.MemberEnd(); ++it)
            documentDfs(it->value);
    else if (d.IsArray())
        for (auto it = d.Begin(); it != d.End(); ++it)
            documentDfs(*it);

    counter++;
}

bool BpTree::operator==(const BpTree &rhs) const {
    if (N != rhs.N) return false;
    for (int i = 0; i < size(); ++i) if (bv[i] != rhs.bv[i]) return false;
    return true;
}


ostream& operator<<(ostream &o, const BpTree &t) {
    o<<"("<<t.N<<",";
    for (int i = 0; i < t.size(); ++i) o<<t.bv[i];
    return o<<")";
}

#endif
