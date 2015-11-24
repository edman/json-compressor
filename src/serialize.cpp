
#ifndef __SERIALIZE_CPP__
#define __SERIALIZE_CPP__

#include "serialize.hpp"
#include "parser.hpp"
#include "util.hpp"
#include <numeric>
#include <string>
#include <vector>
#include <iostream>

using namespace std;
using namespace sdsl;

namespace detail {

    template <class T>
    struct get_size_helper;

    template <>
    struct get_size_helper<Parser> {
        static size_t value(Parser const&obj) {
            // size of header
            size_t s = sizeof(int);
            // size of tree
            s += get_size(obj.tree);
            // size of names table
            s += sizeof(int);
            s += get_size(obj.namess);
            // size of names list
            s += get_size(obj.nameList);
            // size of values BitmapIndex
            s += get_size(obj.valuess);
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
    struct get_size_helper<bit_vector> {
        static size_t value(const bit_vector &obj) {
            return bitvector_size_in_bytes(obj);
        }
    };

    template <>
    struct get_size_helper<SuccinctTree> {
        static size_t value(const SuccinctTree &obj) {
            // return obj.size_in_bytes();
            return get_size(obj.bv);
        }
    };

    template <class T>
    struct get_size_helper<BitmapIndex<T>> {
        static size_t value(const BitmapIndex<T> &obj) {
            // size of bit vector plus size of values vector
            return get_size(obj.bv) + obj.byte_size + sizeof(int);
        }
    };

    template <>
    struct get_size_helper<Jvalue> {
        static size_t value(const Jvalue &obj) {
            size_t s = sizeof(char);
            if (obj.isString()) s += get_size(obj.vstring);
            else if (obj.isInt()) s += sizeof(int);
            else if (obj.isDouble()) s += sizeof(double);
            return s;
        }
    };

    template <class T>
    struct get_size_helper<vector<T>> {
        static size_t value(const vector<T> &obj) {
            size_t s = 0;
            for (auto it = obj.begin(); it != obj.end(); it++)
                s += get_size(*it);
            return s;
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
template size_t get_size<bit_vector>(const bit_vector&);
template size_t get_size<SuccinctTree>(const SuccinctTree&);
template size_t get_size<BitmapIndex<string>>(const BitmapIndex<string>&);
template size_t get_size<BitmapIndex<Jvalue>>(const BitmapIndex<Jvalue>&);
template size_t get_size<Jvalue>(const Jvalue&);
template size_t get_size<vector<string>>(const vector<string>&);
template size_t get_size<vector<int>>(const vector<int>&);
template size_t get_size<string>(const string&);
template size_t get_size<size_t>(const size_t&);
template size_t get_size<int>(const int&);
template size_t get_size<double>(const double&);

namespace detail {

    template <class T>
    struct serialize_helper;

    template <class T>
    void serializer(const T& obj, StreamType::iterator&);

    void serialize_array(char *a, int size, StreamType::iterator &res) {
        for (int i = 0; i < size; ++i) serializer(a[i], res);
    }

    template <>
    struct serialize_helper<Parser> {
        static void apply(const Parser &obj, StreamType::iterator &res) {
            // store header
            serializer(obj.size, res);
            // store the tree
            serializer(obj.tree, res);
            /* Store this part as a ".tree". */
            /* Can store_to_file from SDSL be used here? */
            // store name table
            serializer((int) obj.namess.size(), res);
            serializer(obj.namess, res);
            /* Store this part as a ".namet". */
            // store name list
            serializer(obj.nameList, res);
            /* Store this part as a ".namel". */
            // store bitmap indexes for names and values
            serializer(obj.valuess, res);
            /* Store this part as a ".value". */
        }
    };

    template <>
    struct serialize_helper<std::string> {
        static void apply(const std::string &obj, StreamType::iterator &res) {
            // store each char of the string
            for(const auto &cur : obj) { serializer(cur, res); }
            // store the null character in the end
            serializer('\0', res);
        }
    };

    template <>
    struct serialize_helper<bit_vector> {
        static void apply(const bit_vector &obj, StreamType::iterator &res) {
            int size = bitvector_size_in_bytes(obj);
            char *p;
            p = new char[size];
            bitvector_to_char_array(p, obj);
            serialize_array(p, size, res);
            delete[] p;
        }
    };

    template <>
    struct serialize_helper<SuccinctTree> {
        static void apply(const SuccinctTree &obj, StreamType::iterator &res) {
            // char *p = obj.to_char_array();
            // int size = obj.size_in_bytes();
            // serialize_array(p, size, res);
            serializer(obj.bv, res);
        }
    };

    template <class T>
    struct serialize_helper<BitmapIndex<T>> {
        static void apply(const BitmapIndex<T> &obj, StreamType::iterator &res) {
            // serialize byte_size information
            serializer(obj.byte_size, res);

            // serialize bitmap index
            serializer(obj.bv, res);

            // serialize values
            for (auto it : obj.values) {
                serializer(it, res);
                // store the null character as a separator
                serializer('\0', res);
            }
        }
    };

    template <>
    struct serialize_helper<Jvalue> {
        static void apply(const Jvalue &obj, StreamType::iterator &res) {
            serializer((char) obj.type, res);
            if (obj.isString()) serializer(obj.vstring, res);
            else if (obj.isInt()) serializer(obj.vint, res);
            else if (obj.isDouble()) serializer(obj.vdouble, res);
        }
    };

    template <class T>
    struct serialize_helper<vector<T>> {
        static void apply(const vector<T> &obj, StreamType::iterator &res) {
            for (auto it = obj.begin(); it != obj.end(); it++)
                serializer(*it, res);
        }
    };

    template <class T>
    struct serialize_helper {
        static void apply(const T& obj, StreamType::iterator& res) {
            const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&obj);
            std::copy(ptr,ptr+sizeof(T),res);
            res+=sizeof(T);
        }
    };

    template <class T>
    void serializer(const T& obj, StreamType::iterator& res) {
        serialize_helper<T>::apply(obj,res);
    }

} // end detail namespace

template <class T>
void serialize(const T &obj, StreamType &res) {
    size_t offset = res.size();
    size_t size = get_size(obj);
    res.resize(res.size() + size);


    StreamType::iterator it = res.begin() + offset;
    detail::serializer(obj,it);
    assert(res.begin() + offset + size == it);
}

template void serialize<Parser>(const Parser&, StreamType&);
template void serialize<bit_vector>(const bit_vector&, StreamType&);
template void serialize<SuccinctTree>(const SuccinctTree&, StreamType&);
template void serialize<BitmapIndex<string>>(const BitmapIndex<string>&, StreamType&);
template void serialize<BitmapIndex<Jvalue>>(const BitmapIndex<Jvalue>&, StreamType&);
template void serialize<Jvalue>(const Jvalue&, StreamType&);
template void serialize<vector<string>>(const vector<string>&, StreamType&);
template void serialize<vector<int>>(const vector<int>&, StreamType&);
template void serialize<string>(const string&, StreamType&);
template void serialize<size_t>(const size_t&, StreamType&);
template void serialize<int>(const int&, StreamType&);
template void serialize<double>(const double&, StreamType&);


namespace detail {

    template <class T>
    struct deserialize_helper;

    template <class T>
    struct deserialize_helper {
        static T apply(StreamType::const_iterator& begin,
                StreamType::const_iterator end) {
            // cout << "end - begin " << end - begin << endl;
            // cout << "sizeof(T) " << sizeof(T) << endl;

            assert(begin+sizeof(T)<=end);
            T val;
            uint8_t* ptr = reinterpret_cast<uint8_t*>(&val);
            std::copy(begin, begin+sizeof(T), ptr);
            begin+=sizeof(T);
            return val;
        }
    };

    template <>
    struct deserialize_helper<string> {
        static std::string apply(StreamType::const_iterator& begin,
                StreamType::const_iterator end) {
            string str = "";
            for (char c; (c = deserialize_helper<char>::apply(begin, end));)
                str += c;
            return str;
        }
    };

    template <>
    struct deserialize_helper<bit_vector> {
        static bit_vector apply(int size_in_bits, int size_in_bytes, StreamType::const_iterator& begin,
        // static bit_vector apply(int size_in_bytes, StreamType::const_iterator& begin,
                StreamType::const_iterator end) {
            // int size_in_bits = size_in_bytes * 8;
            // int size_in_bytes = bv_size_in_bytes;
            char *p;
            p = new char[size_in_bytes];

            for (int i = 0; i < size_in_bytes; ++i)
                p[i] = deserialize_helper<char>::apply(begin, end);

            bit_vector bv = char_array_to_bitvector(p, size_in_bits);
            delete[] p;
            return bv;
        }
    };

    template <>
    struct deserialize_helper<SuccinctTree> {
        static SuccinctTree apply(int doc_size, StreamType::const_iterator& begin,
                StreamType::const_iterator end) {
            int size_in_bits = (doc_size + 1) * 2;
            int size_in_bytes = (size_in_bits + 7) / 8;

            bit_vector bv = deserialize_helper<bit_vector>::apply(size_in_bits, size_in_bytes, begin, end);
            return SuccinctTree(doc_size, bv);
       }
    };

    template <class T>
    struct deserialize_helper<BitmapIndex<T>> {
        static BitmapIndex<T> apply(int values_size, StreamType::const_iterator& begin,
                StreamType::const_iterator end) {

            // deserialize byte_size
            int byte_size = deserialize_helper<int>::apply(begin, end);

            // deserialize bit vector
            int bv_size_in_bytes = bit_size_to_bytes(byte_size);
            int bv_size_in_bits = byte_size;
            bit_vector bv = deserialize_helper<bit_vector>::apply(bv_size_in_bits, bv_size_in_bytes, begin, end);

            // deserialize values vector
            vector<T> values; values.reserve(values_size);
            for (int i = 0; i < values_size; ++i) {
                // deserialize a single value
                values.push_back(deserialize_helper<T>::apply(begin, end));
                // consume separator character
                deserialize_helper<char>::apply(begin, end);
            }

            return BitmapIndex<T>(byte_size, bv, values);
        }
    };

    template <>
    struct deserialize_helper<Jvalue> {
        static Jvalue apply(StreamType::const_iterator& begin,
                StreamType::const_iterator end) {
            types type = static_cast<types>(deserialize_helper<char>::apply(begin, end));

            if (type == kString) return Jvalue(type, deserialize_helper<string>::apply(begin, end));
            else if (type == kInt) return Jvalue(type, deserialize_helper<int>::apply(begin, end));
            else if (type == kDouble) return Jvalue(type, deserialize_helper<double>::apply(begin, end));
            return Jvalue(type);
        }
    };

    template <class T>
    struct deserialize_helper<vector<T>> {
        static vector<T> apply(int size, StreamType::const_iterator& begin,
                StreamType::const_iterator end) {
            vector<T> v; v.reserve(size);
            for (int i = 0; i < size; i++)
                v.push_back(deserialize_helper<T>::apply(begin, end));
            return v;
        }
    };

    template <>
    struct deserialize_helper<Parser> {
        static Parser apply(StreamType::const_iterator& begin,
                StreamType::const_iterator end) {
            // recover header
            int size = deserialize_helper<int>::apply(begin, end);
            // recover the succinct tree
            SuccinctTree st = deserialize_helper<SuccinctTree>::apply(size, begin, end);
            // recover names table
            int namessSize = deserialize_helper<int>::apply(begin, end);
            vector<string> namess = deserialize_helper<vector<string>>::apply(namessSize, begin, end);
            // recover names list
            vector<int> nameList = deserialize_helper<vector<int>>::apply(size, begin, end);
            // recover values
            BitmapIndex<Jvalue> valuess = deserialize_helper<BitmapIndex<Jvalue>>::apply(size, begin, end);

            return Parser(size, st, namess, nameList, valuess);
        }
    };

} // end detail namespace

template <class T>
T deserialize(StreamType::const_iterator &begin, const StreamType::const_iterator &end) {
    return detail::deserialize_helper<T>::apply(begin, end);
}

template <class T>
T deserialize(int extra, StreamType::const_iterator &begin, const StreamType::const_iterator &end) {
    return detail::deserialize_helper<T>::apply(extra, begin, end);
}

template <class T>
T deserialize(int extra1, int extra2, StreamType::const_iterator &begin, const StreamType::const_iterator &end) {
    return detail::deserialize_helper<T>::apply(extra1, extra2, begin, end);
}

template <class T>
T deserialize(const StreamType &res) {
    StreamType::const_iterator it = res.begin();
    return deserialize<T>(it, res.end());
}

template Parser deserialize(const StreamType&);
template Parser deserialize(StreamType::const_iterator&, const StreamType::const_iterator&);
template bit_vector deserialize(int, int, StreamType::const_iterator&, const StreamType::const_iterator&);
template SuccinctTree deserialize(int, StreamType::const_iterator&, const StreamType::const_iterator&);
template BitmapIndex<string> deserialize(int, StreamType::const_iterator&, const StreamType::const_iterator&);
template BitmapIndex<Jvalue> deserialize(int, StreamType::const_iterator&, const StreamType::const_iterator&);
template Jvalue deserialize(StreamType::const_iterator&, const StreamType::const_iterator&);
template vector<string> deserialize(int, StreamType::const_iterator&, const StreamType::const_iterator&);
template vector<int> deserialize(int, StreamType::const_iterator&, const StreamType::const_iterator&);
template string deserialize(StreamType::const_iterator&, const StreamType::const_iterator&);
template size_t deserialize(StreamType::const_iterator&, const StreamType::const_iterator&);
template int deserialize(StreamType::const_iterator&, const StreamType::const_iterator&);
template double deserialize(StreamType::const_iterator&, const StreamType::const_iterator&);

void save_to_file(Parser &parser, const string &filename) {
    StreamType res;
    serialize(parser, res);
    save_to_file(res, filename);
}

void save_to_file(StreamType &res, const string &filename) {
    ofstream ofile(filename, ios::binary);
    ofile.write((char*) &res[0], res.size());
    ofile.close();
}

Parser load_from_file(const string &filename) {
    ifstream ifile(filename, ios::binary);
    ifile.seekg(0, ifile.end);
    long size = ifile.tellg();
    StreamType res(size);
    ifile.seekg(0);
    ifile.read((char*) &res[0], size);
    ifile.close();
    return deserialize<Parser>(res);
}

#endif

