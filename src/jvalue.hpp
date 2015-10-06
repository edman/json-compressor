
#ifndef __JVALUE_HPP__
#define __JVALUE_HPP__

#include "rapidjson/document.h"

#include <iostream>
#include <unordered_set>
#include <string>
#include <vector>
#include <utility>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/utility.hpp>

using namespace std;
using namespace rapidjson;

enum types { kNull, kFalse, kTrue, kObject, kArray, kString, kInt, kDouble };

class Jvalue {
public:
    types type;
    // { "String", "Int", "Double" };
    string vstring;
    int vint;
    double vdouble;

public:
    /* Constructors */
    Jvalue() {}
    Jvalue(types t, string v): type(t), vstring(v) {}
    Jvalue(types t, int v): type(t), vint(v) {}
    Jvalue(types t, double v): type(t), vdouble(v) {}
    // Jvalue(Jvalue &a) { *this = a; cout << "Jvalue construct" << endl; }
    Jvalue(Value&);
    // ~Jvalue() { cout << * this << " encoding destruct" << endl; }

	#ifdef OLD
    /* Serialization. */
    friend class boost::serialization::access;
    template<class Archive> void save(Archive &ar, const unsigned int) const
    {
    	pair<int, string> pi;
    	if(type == kString)
    	{
    		pi = make_pair(type, vstring);
    		ar & pi;
    	}
    	else if(type == kInt)
    	{
    		pi = make_pair(type, std::to_string(vint));
    		ar & pi;
    	}
    	else if(type == kDouble)
    	{
    		pi = make_pair(type, std::to_string(vdouble));
    		ar & pi;
    	}
    	else if(type < kNull || type > kDouble)
    	{
    		
    	}
    	else
    	{
    		pi = make_pair(type, 0);
    		ar & pi;
    	}
    }
    template<class Archive> void load(Archive &ar, const unsigned int)
    {
    	pair<int, string> pi;
    	ar & pi;
    	type = (types)pi.first;
    	if(type == kString)
    	{
    		vstring = pi.second;
    	}
    	else if(type == kInt)
    	{
		vint = stoi(pi.second);
    	}
    	else if(type == kDouble)
    	{
    		vdouble = stof(pi.second);
    	}
    }
    template<class Archive> void serialize(Archive &ar, const unsigned int file_version)
    {
    	boost::serialization::split_member(ar, *this, file_version);
    }
	#endif

    /* Initialization functions */
    void init(types t) { type = t; if (hasValue()) cout << "Value expected" << endl; }
    void init(types t, string v) { type = t; vstring = v; }
    void init(types t, int v) { type = t; vint = v; }
    void init(types t, double v) { type = t; vdouble = v; }

    /* Methods */
    bool hasValue() const { return type >= kString && type <= kDouble; }
    bool isString() const { return type == kString; }
    bool isInt() const { return type == kInt; }
    bool isDouble() const { return type == kDouble; }

    /* Operators */
    Jvalue operator=(const Jvalue &enc);
    bool operator==(const Jvalue &enc) const;
    bool operator!=(const Jvalue &rhs) const { return !(*this == rhs); }
    bool operator<(const Jvalue& rhs) const;
    friend ostream& operator<<(ostream &o, const Jvalue &enc);
};


namespace std {
    template<> struct hash<Jvalue> {
        size_t operator()(const Jvalue& enc) const {
            size_t h1 = hash<int>()(enc.type), h2;
            if (!enc.hasValue())
                return h1;
            if (enc.isString())
                h2 = hash<string>()(enc.vstring);
            if (enc.isInt())
                h2 = hash<int>()(enc.vint);
            if (enc.isDouble())
                h2 = hash<double>()(enc.vdouble);
            return h1 ^ (h2 << 1);
        }
    };
}

#endif
