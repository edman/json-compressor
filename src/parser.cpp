
#ifndef __PARSER_CPP__
#define __PARSER_CPP__

#include "parser.hpp"
#include "succinct_tree.hpp"
#include "jvalue.hpp"
#include "rapidjson/document.h"
#include <iostream>
#include <map>

#include <fstream>
#include <sstream>
/* Requires boost. */
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/serialization/string.hpp>

using namespace std;
using namespace rapidjson;

/* TODO
For now, we have just used a plain "bit vector" to store the structure of a tree.
However, to link between the plain tree representation and the relevant stringstream, we need to use more bits to differentiate them.
Fortunately, SDSL provides something called "int vector" which supports rank/select queries.
Since there are finite types, I think converting the bit vector to that does not harm anything but to support queries in efficient way. */

Parser::Parser(Value &d) {
	int n = 0;
	ParseStreams pst;

	/* Load relevant values. */
	docParse(pst, d, treeBv, n);
	/* Store the tree into a file, using serialization function. */
	std::ofstream treeBvs("./output.tree");
	treeBv.serialize(treeBvs);
	treeBvs.close();
	/* Store remaining bit vectors. */
	//Not implemented yet.
	/* Compress the stream. */
	zlibCompress(pst);

	#ifdef DECOMPRESS
	/* Decompression part. */
	ifstream nameIfs("./output.zlib.name", std::ios::binary);
	ifstream valueIfs("./output.zlib.value", std::ios::binary);
	zlibDecompressN(&nameIfs, &newMap);
	zlibDecompressV(&valueIfs, &newMap2);
	#endif
}

Parser::~Parser() {

}

void Parser::zlibCompress(Parser::ParseStreams &pst)
{
	std::ofstream nameSos("./output.zlib.name"), numSos("./output.zlib.num"), stringSos("./output.zlib.str");
	boost::iostreams::filtering_istreambuf is;

	/* Do the sequence for each string or bit vector. */
	/* For the name. */
	/* Process the zlib compression. */
	is.push(boost::iostreams::zlib_compressor());
	is.push(pst.nameStr);
	/* Save the content to the file. */
	boost::iostreams::copy(is, nameSos);
	nameSos.close();

	/* For the number. */
	is.push(boost::iostreams::zlib_compressor());
	is.push(pst.numStr);
	boost::iostreams::copy(is, numSos);
	numSos.close();

	/* For the string. */
	is.push(boost::iostreams::zlib_compressor());
	is.push(pst.stringStr);
	boost::iostreams::copy(is, stringSos);
	stringSos.close();
}

#ifdef NOT_FIXED
void Parser::zlibDecompressN(ifstream *f, map<string, int> *m)
{
	stringstream ss;
	boost::iostreams::filtering_istreambuf is;
	is.push(boost::iostreams::zlib_decompressor());
	is.push(*f);
	boost::iostreams::copy(is, ss);
	boost::archive::text_iarchive iarch(ss);
	iarch >> *m;
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
}
#endif

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

void Parser::docParse(Parser::ParseStreams &pst, Value &d, sdsl::bit_vector &b, int &n)
{
	if(d.IsObject())
	{
		b.resize(n + 2);
		b[n] = 1;
		std::cout << n << " 1 " << "OBJECT" << std::endl;
		n++;
		/* Explicit resizing required. FIXME? */
		for(auto it = d.MemberBegin(); it != d.MemberEnd(); it++)
		{
			b.resize(n + 2);
			b[n] = 1;
			/* Concatenate the name. */
			std::cout << n << " 1 " << it->name.GetString() << std::endl;
			pst.nameStr << it->name.GetString();
			/* Get the length of name and mark the last bit as 1. */
			int i = strlen(it->name.GetString());
			pst.nameBv.resize(pst.nameBv.size() + i);
			pst.nameBv[pst.nameBv.size() - 1] = 1;
			n++;
			docParse(pst, it->value, b, n);
		}
		std::cout << n << " 0 " << "OBJEND" << std::endl;
		n++;
	}
	else if(d.IsArray())
	{
		b.resize(n + 2);
		b[n] = 1;
		std::cout << n << " 1 " << "ARRAY" << std::endl;
		n++;
		/* Explicit resizing required. FIXME? */
		for(auto it = d.Begin(); it != d.End(); it++)
		{
			b.resize(n + 2);
			b[n] = 1;
			/* TODO: How to deal with this case? One possible solution is to add a NULL character and put 1 in the bit vector. */
			std::cout << n << " 1 " << "ARRELE" << std::endl;
			pst.nameStr << "\a"; //This sequence is not used in the specification.
			pst.nameBv.resize(pst.nameBv.size() + 2);
			pst.nameBv[pst.nameBv.size() - 1] = 1;
			n++;
			docParse(pst, *it, b, n);
		}
		std::cout << n << " 0 " << "ARREND" << std::endl;
		n++;
	}
	else if(d.IsString())
	{
		std::cout << n << " 0 " << d.GetString() << std::endl;
		/* Concatenate the string. */
		pst.stringStr << d.GetString();
		/* Get the length of string and mark the last bit as 1. */
		int i = strlen(d.GetString());
		pst.stringBv.resize(pst.stringBv.size() + i);
		pst.stringBv[pst.stringBv.size() - 1] = 1;
		n++;
	}
	else if(d.IsNumber())
	{
		std::cout << n << " 0 " << d.GetDouble() << std::endl;
		std::string tmp;
		tmp = std::to_string(d.GetDouble());
		/* Concatenate the number, as in form of stringstream. */
		pst.numStr << tmp;
		int i = strlen(tmp.c_str());
		pst.numBv.resize(pst.numBv.size() + i);
		pst.numBv[pst.numBv.size() - 1] = 1;
		n++;
	}
	else if(d.IsBool())
	{
		std::cout << n << " 0 " << d.GetBool() << std::endl;
		/* Modifying the bit vector is enough. */
		pst.boolBv.resize(pst.boolBv.size() + 1);
		pst.boolBv[pst.boolBv.size() - 1] = d.GetBool();
		n++;
	}
	else if(d.IsNull())
	{
		std::cout << n << " 0 " << "NULL" << std::endl;
		/* TODO! How to mark NULL? */
		n++;
	}
	else
	{
		std::cout << "WTFF" << std::endl;
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

