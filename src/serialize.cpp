
#ifndef __SERIALIZE_CPP__
#define __SERIALIZE_CPP__

#include "serialize.hpp"
#include "bp_tree.hpp"
#include "df_tree.hpp"
#include "cjson.hpp"
#include "util.hpp"
#include "packed_array.hpp"
#include <cstring>
#include <numeric>
#include <string>
#include <vector>
#include <iostream>

using namespace std;
using namespace sdsl;

namespace detail {

    template <class T>
    struct get_size_helper;

    template <class T>
    struct get_size_helper< Cjson<T> > {
        static size_t value(Cjson<T> const&obj) {
            // size of header
            size_t s = sizeof(int);
            // size of tree
            s += get_size(obj.tree);
            // size of names table
            s += sizeof(uint);
            s += get_size(obj.names);
            // size of values BitmapIndex
            s += get_size(obj.values);
            // size of stringValues array
            s += sizeof(uint);
            s += get_size(obj.stringValues);
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
    struct get_size_helper<char*> {
        static size_t value(char *const &obj) {
            return strlen(obj) + 1;
        }
    };

    template <>
    struct get_size_helper<bit_vector> {
        static size_t value(const bit_vector &obj) {
            return bitvector_size_in_bytes(obj);
        }
    };

    template <>
    struct get_size_helper<BpTree> {
        static size_t value(const BpTree &obj) {
            // return obj.size_in_bytes();
            return get_size(obj.bv);
        }
    };

    template <>
    struct get_size_helper<DfTree> {
        static size_t value(const DfTree &obj) {
            // return obj.size_in_bytes();
            return get_size(obj.bv);
        }
    };

    template <>
    struct get_size_helper<PackedArray> {
        static size_t value(const PackedArray &obj) {
            // size and blob array
            return sizeof(obj.size()) + obj.size();
        }
    };

    template <>
    struct get_size_helper<BitmapIndex> {
        static size_t value(const BitmapIndex &obj) {
            // series of elements (packed array only)
            return get_size(obj._array);
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

template size_t get_size< Cjson<BpTree> >(const Cjson<BpTree>&);
template size_t get_size< Cjson<DfTree> >(const Cjson<DfTree>&);
template size_t get_size<bit_vector>(const bit_vector&);
template size_t get_size<BpTree>(const BpTree&);
template size_t get_size<DfTree>(const DfTree&);
template size_t get_size<PackedArray>(const PackedArray&);
template size_t get_size<BitmapIndex>(const BitmapIndex&);
template size_t get_size<vector<string>>(const vector<string>&);
template size_t get_size<vector<char*>>(const vector<char*>&);
template size_t get_size<vector<int>>(const vector<int>&);
template size_t get_size<string>(const string&);
template size_t get_size<char*>(char *const &);
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

    template <class T>
    struct serialize_helper< Cjson<T> > {
        static void apply(const Cjson<T> &obj, StreamType::iterator &res) {
            // store header
            serializer(obj.size, res);
            // store the tree
            serializer(obj.tree, res);
            // store name table
            serializer((uint) obj.names.size(), res);
            serializer(obj.names, res);
            // store bitmap indexes for values
            serializer(obj.values, res);
            // store stringValues array
            serializer((uint) obj.stringValues.size(), res);
            serializer(obj.stringValues, res);
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
    struct serialize_helper<char*> {
        static void apply(char *const &obj, StreamType::iterator &res) {
            // store each char of the char array
            for (char *cur = obj; *cur; cur++) { serializer(*cur, res); }
            // store the null character in the end
            serializer('\0', res);
        }
    };

    template <>
    struct serialize_helper<bit_vector> {
        static void apply(const bit_vector &obj, StreamType::iterator &res) {
            int size = bitvector_size_in_bytes(obj);
            char *p = new char[size];
            bitvector_to_char_array(p, obj);
            serialize_array(p, size, res);
            delete[] p;
        }
    };

    template <>
    struct serialize_helper<BpTree> {
        static void apply(const BpTree &obj, StreamType::iterator &res) {
            // char *p = obj.to_char_array();
            // int size = obj.size_in_bytes();
            // serialize_array(p, size, res);
            serializer(obj.bv, res);
        }
    };

    template <>
    struct serialize_helper<DfTree> {
        static void apply(const DfTree &obj, StreamType::iterator &res) {
            // char *p = obj.to_char_array();
            // int size = obj.size_in_bytes();
            // serialize_array(p, size, res);
            serializer(obj.bv, res);
        }
    };

    template <>
    struct serialize_helper<PackedArray> {
        static void apply(const PackedArray &obj, StreamType::iterator &res) {
            // serialize number of bytes
            serializer(obj.size(), res);
            // serialize byte array
            serialize_array(obj._array, obj.size(), res);
        }
    };

    template <>
    struct serialize_helper<BitmapIndex> {
        static void apply(const BitmapIndex &obj, StreamType::iterator &res) {
            // serialize values
            serializer(obj._array, res);
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
    // cout << "exp: " << offset + size << " | got:" << it - res.begin() << endl;
    assert(res.begin() + offset + size == it);
}

template void serialize< Cjson<BpTree> >(const Cjson<BpTree>&, StreamType&);
template void serialize< Cjson<DfTree> >(const Cjson<DfTree>&, StreamType&);
template void serialize<bit_vector>(const bit_vector&, StreamType&);
template void serialize<BpTree>(const BpTree&, StreamType&);
template void serialize<DfTree>(const DfTree&, StreamType&);
template void serialize<BitmapIndex>(const BitmapIndex&, StreamType&);
template void serialize<vector<string>>(const vector<string>&, StreamType&);
template void serialize<vector<char*>>(const vector<char*>&, StreamType&);
template void serialize<vector<int>>(const vector<int>&, StreamType&);
template void serialize<string>(const string&, StreamType&);
template void serialize<char*>(char *const &, StreamType&);
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

    void deserialize_array(char *a, int size, StreamType::const_iterator &begin,
            StreamType::const_iterator end) {
        for (int i = 0; i < size; ++i)
            a[i] = deserialize_helper<char>::apply(begin, end);
    }

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
    struct deserialize_helper<char*> {
        static char* apply(StreamType::const_iterator& begin,
                StreamType::const_iterator end) {
            string str = deserialize_helper<string>::apply(begin, end);
            return string_to_cstr(str);
        }
    };

    template <>
    struct deserialize_helper<bit_vector> {
        static bit_vector apply(int size_in_bits, int size_in_bytes, StreamType::const_iterator& begin,
        // static bit_vector apply(int size_in_bytes, StreamType::const_iterator& begin,
                StreamType::const_iterator end) {
            // int size_in_bits = size_in_bytes * 8;
            // int size_in_bytes = bv_size_in_bytes;
            char *p = new char[size_in_bytes];

            for (int i = 0; i < size_in_bytes; ++i)
                p[i] = deserialize_helper<char>::apply(begin, end);

            bit_vector bv = char_array_to_bitvector(p, size_in_bits);
            delete[] p;
            return bv;
        }
    };

    template <>
    struct deserialize_helper<BpTree> {
        static BpTree apply(int doc_size, StreamType::const_iterator& begin,
                StreamType::const_iterator end) {
            int size_in_bits = (doc_size + 1) * 2;
            int size_in_bytes = (size_in_bits + 7) / 8;

            bit_vector bv = deserialize_helper<bit_vector>::apply(size_in_bits, size_in_bytes, begin, end);
            return BpTree(doc_size, bv);
       }
    };

    template <>
    struct deserialize_helper<DfTree> {
        static DfTree apply(int doc_size, StreamType::const_iterator& begin,
                StreamType::const_iterator end) {
            int size_in_bits = (doc_size + 1) * 2;
            int size_in_bytes = (size_in_bits + 7) / 8;

            bit_vector bv = deserialize_helper<bit_vector>::apply(size_in_bits, size_in_bytes, begin, end);
            return DfTree(doc_size, bv);
       }
    };

    template <>
    struct deserialize_helper<PackedArray> {
        static PackedArray apply(StreamType::const_iterator& begin,
                StreamType::const_iterator end) {
            // deserialize void array size in bytes
            size_t size = deserialize_helper<size_t>::apply(begin, end);
            // allocate an array of the appropriate size
            char *array = new char[size];
            // deserialize the void array byte by byte
            deserialize_array(array, size, begin, end);
            // construct and return a PackedArray
            return PackedArray(size, array);
       }
    };

    template <>
    struct deserialize_helper<BitmapIndex> {
        static BitmapIndex apply(int values_size, StreamType::const_iterator& begin,
                StreamType::const_iterator end) {
            // deserialize values array
            PackedArray array = deserialize_helper<PackedArray>::apply(begin, end);

            // load jval bitmap index from its values array
            BitmapIndex index(array);
            loadJvalBitmapIndex(index);
            return index;
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

    template <class T>
    struct deserialize_helper< Cjson<T> > {
        static Cjson<T> apply(StreamType::const_iterator& begin,
                StreamType::const_iterator end) {
            // recover header
            int size = deserialize_helper<int>::apply(begin, end);
            // recover the succinct tree
            T st = deserialize_helper<T>::apply(size, begin, end);
            // recover names table
            uint namesSize = deserialize_helper<uint>::apply(begin, end);
            vector<char*> names = deserialize_helper<vector<char*>>::apply(namesSize, begin, end);
            // recover values
            BitmapIndex values = deserialize_helper<BitmapIndex>::apply(size, begin, end);
            // recover stringValues array
            uint strvSize = deserialize_helper<uint>::apply(begin, end);
            vector<char*> stringValues = deserialize_helper<vector<char*>>::apply(strvSize, begin, end);

            return Cjson<T>(size, st, names, values, stringValues);
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

template Cjson<BpTree> deserialize(const StreamType&);
template Cjson<DfTree> deserialize(const StreamType&);
template Cjson<BpTree> deserialize(StreamType::const_iterator&, const StreamType::const_iterator&);
template Cjson<DfTree> deserialize(StreamType::const_iterator&, const StreamType::const_iterator&);
template bit_vector deserialize(int, int, StreamType::const_iterator&, const StreamType::const_iterator&);
template BpTree deserialize(int, StreamType::const_iterator&, const StreamType::const_iterator&);
template DfTree deserialize(int, StreamType::const_iterator&, const StreamType::const_iterator&);
template BitmapIndex deserialize(int, StreamType::const_iterator&, const StreamType::const_iterator&);
template vector<char*> deserialize(int, StreamType::const_iterator&, const StreamType::const_iterator&);
template vector<string> deserialize(int, StreamType::const_iterator&, const StreamType::const_iterator&);
template vector<int> deserialize(int, StreamType::const_iterator&, const StreamType::const_iterator&);
template char*  deserialize(StreamType::const_iterator&, const StreamType::const_iterator&);
template string deserialize(StreamType::const_iterator&, const StreamType::const_iterator&);
template size_t deserialize(StreamType::const_iterator&, const StreamType::const_iterator&);
template int deserialize(StreamType::const_iterator&, const StreamType::const_iterator&);
template double deserialize(StreamType::const_iterator&, const StreamType::const_iterator&);

template <class T>
void save_to_file(Cjson<T> &cjson, const string &filename) {
    StreamType res;
    serialize(cjson, res);
    save_to_file(res, filename);
}

template void save_to_file(Cjson<BpTree>&, const string&);
template void save_to_file(Cjson<DfTree>&, const string&);

template <class T>
void save_to_file_split(Cjson<T> &cjson, const string &filename) {
    const int nsplit = 5;
    string splitnames[] = {"_header", "_tree", "_names", "_values", "_strvalues"};

    StreamType res[nsplit];
    serialize(cjson.size, res[0]); // header (size)
    serialize((int) cjson.names.size(), res[0]); // header (name table size)
    serialize((uint) cjson.stringValues.size(), res[0]); // header (str values size)
    serialize(cjson.tree, res[1]); // tree
    serialize(cjson.names, res[2]); // name table
    serialize(cjson.values, res[3]); // values
    serialize(cjson.stringValues, res[4]); // string values

    for (int i = 0; i < nsplit; i++) {
        string splitname = filename + splitnames[i];
        save_to_file(res[i], splitname);
    }
}

template void save_to_file_split(Cjson<BpTree>&, const string&);
template void save_to_file_split(Cjson<DfTree>&, const string&);

void save_to_file(StreamType &res, const string &filename) {
    ofstream ofile(filename, ios::binary);
    ofile.write((char*) &res[0], res.size());
    ofile.close();
}

template <class T>
Cjson<T> load_from_file(const string &filename) {
    ifstream ifile(filename, ios::binary);
    ifile.seekg(0, ifile.end);
    long size = ifile.tellg();
    StreamType res(size);
    ifile.seekg(0);
    ifile.read((char*) &res[0], size);
    ifile.close();
    return deserialize< Cjson<T> >(res);
}

template Cjson<BpTree> load_from_file(const string&);
template Cjson<DfTree> load_from_file(const string&);

template <class T>
Cjson<T> load_from_file_split(const string &filename) {
    const int nsplit = 5;
    string splitnames[] = {"_header", "_tree", "_names", "_values", "_strvalues"};

    int psize, namesSize, strvSize;
    T st;
    vector<char*> names;
    BitmapIndex values;
    vector<char*> stringValues;

    for (int i = 0; i < nsplit; i++) {
        ifstream ifile(filename + splitnames[i], ios::binary);
        ifile.seekg(0, ifile.end);
        long size = ifile.tellg();
        StreamType res(size);
        ifile.seekg(0);
        ifile.read((char*) &res[0], size);
        ifile.close();

        StreamType::const_iterator begin = res.begin(), end = res.end();
        if (i == 0) {
            psize = deserialize<int>(begin, end);
            namesSize = deserialize<int>(begin, end);
            strvSize = deserialize<uint>(begin, end);
        }
        else if (i == 1) st = deserialize<T>(psize, begin, end);
        else if (i == 2) names = deserialize<vector<char*>>(namesSize, begin, end);
        else if (i == 3) values = deserialize<BitmapIndex>(psize, begin, end);
        // else if (i == 4)
        else stringValues = deserialize<vector<char*>>(strvSize, begin, end);
    }

    return Cjson<T>(psize, st, names, values, stringValues);
}

template Cjson<BpTree> load_from_file_split(const string&);
template Cjson<DfTree> load_from_file_split(const string&);

#endif

