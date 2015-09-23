
#ifndef __PARSER_CPP__
#define __PARSER_CPP__

#include "parser.hpp"
#include "succinct_tree.hpp"
#include "jvalue.hpp"
#include "rapidjson/document.h"
#include <iostream>
#include <map>

#ifdef COMPRESS
#include <fstream>
#include <sstream>
/* Requires boost. */
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/serialization/map.hpp>
#endif

using namespace std;
using namespace rapidjson;

static int counter;

Parser::Parser(Value &d) {
    // Load JSON names and values in a hash table
    map<string, int> nameMap;
    map<string, int> newMap;
    map<Jvalue, int> valueMap;
    map<Jvalue, int> newMap2;
    int n = 0; counter = 0;
    loadNames(nameMap, d, n);
    loadValues(valueMap, d, (n = 0, n));

    // Copy the maps to arrays
    names = mapToArray(nameMap);
    values = mapToArray(valueMap);
    namen = nameMap.size();
    valuen = valueMap.size();

    // Construct a tree from the JSON sctructure
    size = counter;
    tree = SuccinctTree(d, size);

    // Create the list of encodings from the JSON file
    counter = 0;
    codes = new encode[size];
    loadCodes(d, nameMap, valueMap);

	#ifdef COMPRESS
	/* Compress the constructed arrays using zlib. */
	ofstream nameOfs("./output.zlib.name", std::ios::binary);
	ofstream valueOfs("./output.zlib.value", std::ios::binary);
	zlibCompressN(&nameMap, &nameOfs);
	zlibCompressV(&valueMap, &valueOfs);
	/* TODO: Loading data from ifstream, still not resolved yet. */

	/* Decompression part. */
	ifstream nameIfs("./output.zlib.name", std::ios::binary);
	ifstream valueIfs("./output.zlib.value", std::ios::binary);
	zlibDecompressN(&nameIfs, &newMap);
	zlibDecompressV(&valueIfs, &newMap2);

	if(nameMap == newMap) std::cout << "T1" << std::endl;
	if(valueMap == newMap2) std::cout << "T2" << std::endl;
	else
	{
		for(auto it = newMap2.cbegin(); it != newMap2.cend(); ++it)
		{
			std::cout << it->first.type << " " << it->second << std::endl;
		}
	}
	#endif
}

Parser::~Parser() {
    delete[] names;
    delete[] values;
}

#ifdef COMPRESS
void Parser::zlibCompressN(map<string, int> *m, ofstream *f)
{
	stringstream ss;
	boost::archive::text_oarchive oarch(ss);
	boost::iostreams::filtering_istreambuf is;
	oarch << *m;
	std::cout << ss.str() << std::endl;
	is.push(boost::iostreams::zlib_compressor());
	is.push(ss);
	boost::iostreams::copy(is, *f);
	f->close();
}

/* TODO: Need to serialize Jvalue. */
void Parser::zlibCompressV(map<Jvalue, int> *m, ofstream *f)
{
        stringstream ss;
	boost::archive::text_oarchive oarch(ss);
        boost::iostreams::filtering_istreambuf is;
	oarch << *m;
	std::cout << ss.str() << std::endl;
        is.push(boost::iostreams::zlib_compressor());
        is.push(ss);
	boost::iostreams::copy(is, *f);
        f->close();
}

void Parser::zlibDecompressN(ifstream *f, map<string, int> *m)
{
	stringstream ss;
	boost::iostreams::filtering_istreambuf is;
	is.push(boost::iostreams::zlib_decompressor());
	is.push(*f);
	boost::iostreams::copy(is, ss);
	boost::archive::text_iarchive iarch(ss);
	iarch >> *m;
	std::cout << ss.str() << std::endl;
}

void Parser::zlibDecompressV(ifstream *f, map<Jvalue, int> *m)
{
	stringstream ss;
	boost::iostreams::filtering_istreambuf is;
	is.push(boost::iostreams::zlib_decompressor());
	is.push(*f);
	boost::iostreams::copy(is, ss);
	boost::archive::text_iarchive iarch(ss);
	iarch >> *m;
	std::cout << ss.str() << std::endl;
}

#endif

void Parser::loadCodes(Value &d, map<string, int> nameMap, map<Jvalue, int> valueMap) {
    if (d.IsObject()) {
        for (auto it = d.MemberBegin(); it != d.MemberEnd(); ++it) {
            int name = nameMap[it->name.GetString()];
            int type = type_of(it->value);
            int value = (type >= 5 ? valueMap[Jvalue(it->value)] : -1);
            codes[counter++] = encode(name, type, value);

            loadCodes(it->value, nameMap, valueMap);
        }
    }
    else if (d.IsArray())
        for (auto it = d.Begin(); it != d.End(); ++it) {
            int name = -1;
            int type = type_of(*it);
            int value = (type >= 5 ? valueMap[Jvalue(*it)] : -1);
            codes[counter++] = encode(name, type, value);

            loadCodes(*it, nameMap, valueMap);
        }
}

int type_of(Value &d) {
    if (d.GetType() != 6) return d.GetType();
    return d.IsInt() ? 6 : 7;
}

template <typename T> T* mapToArray(map<T, int> &mmap) {
    // Allocate space for array a
    T *a = new T[mmap.size()];
    // Copy the elements from map to array
    for (auto it = mmap.begin(); it != mmap.end(); ++it)
        a[it->second] = it->first;
    return a;
}

void loadNames(map<string, int> &nameMap, Value &d, int &n) {
    if (d.IsObject())
        for (auto it = d.MemberBegin(); it != d.MemberEnd(); ++it) {
            counter++;
            string name = it->name.GetString();
            if (nameMap.find(name) == nameMap.end())
                nameMap[name] = n++;
            loadNames(nameMap, it->value, n);
        }
    if (d.IsArray()) for (auto it = d.Begin(); it != d.End(); ++it) {
        counter++;
        loadNames(nameMap, *it, n);
    }
}

void loadValues(map<Jvalue, int> &valueMap, Value &d, int &n) {
    if (d.IsObject())
        for (auto it = d.MemberBegin(); it != d.MemberEnd(); ++it)
            loadValues(valueMap, it->value, n);
    else if (d.IsArray()) for (auto it = d.Begin(); it != d.End(); ++it)
        loadValues(valueMap, *it, n);
    else if (d.IsString() || d.IsNumber()) {
        Jvalue value(d);
        if (valueMap.find(value) == valueMap.end())
            valueMap[value] = n++;
    }
}

bool Parser::operator==(const Parser &rhs) const {
    if (size != rhs.size || namen != rhs.namen || valuen != rhs.valuen || tree != rhs.tree)
        return false;
    for (int i = 0; i < size; ++i)
        if (codes[i] != rhs.codes[i]) return false;
    for (int i = 0; i < namen; ++i)
        if (names[i] != rhs.names[i]) return false;
    for (int i = 0; i < valuen; ++i)
        if (values[i] != rhs.values[i]) return false;
    return true;
}


ostream& operator<<(ostream &o, const encode &e) {
    o<<"("<<e.name<<","<<e.type;
    return (e.type >= 5 ? o<<","<<e.value : o)<<")";
}

#endif

