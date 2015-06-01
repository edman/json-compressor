
#ifndef __SERIALIZE_CPP__
#define __SERIALIZE_CPP__

#include "serialize.hpp"
#include "parser.hpp"
#include <numeric>
#include <string>
#include <iostream>

using namespace std;

namespace detail {

    template <class T>
    struct get_size_helper;

    template <>
    struct get_size_helper<Parser> {
        static size_t value(Parser const&obj) {
            // size of header
            size_t s = 0;
            s += 3 * sizeof(int);
            // size of tree
            s += 2 * (obj.size + 1);
            // size of encodes, names, and values
            for (int i = 0; i < obj.size; ++i)
                s += get_size(obj.codes[i]);
            for (int i = 0; i < obj.namen; ++i)
                s += get_size(obj.names[i]);
            // cout << ".." << i << ": "<< get_size(obj.values[i]) << endl;;}
            for (int i = 0; i < obj.valuen; ++i)
                s += get_size(obj.values[i]);
            return s;
        }
    };

    /**
     * Specialization for an std::string.
     */
    template <>
    struct get_size_helper<std::string> {
        static size_t value(const std::string &obj) {
            return obj.length()*sizeof(uint8_t) + 1;
        }
    };

    template <>
    struct get_size_helper<Jvalue> {
        static size_t value(const Jvalue &obj) {
            size_t s = 0;
            // s += sizeof(char);
            if (obj.isString()) s += get_size(obj.vstring);
            else if (obj.isInt()) s += sizeof(int);
            else if (obj.isDouble()) s += sizeof(double);
            return s;
        }
    };

    template <>
    struct get_size_helper<encode> {
        static size_t value(const encode &obj) {
            return (obj.type >= 5 ? 3 : 2) * sizeof(int);
        }
    };

    /**
     * Specialization for any remaining type, simply use the value of
     * sizeof()
     */
    template <class T>
    struct get_size_helper {
        static size_t value(const T &obj) { return sizeof(T); }
    };

} // end detail namespace

template <class T>
size_t get_size(const T &obj) {
    return detail::get_size_helper<T>::value(obj);
}

template size_t get_size<Parser>(const Parser&);
template size_t get_size<Jvalue>(const Jvalue&);
template size_t get_size<string>(const string&);
template size_t get_size<size_t>(const size_t&);
template size_t get_size<int>(const int&);
template size_t get_size<double>(const double&);

#endif

