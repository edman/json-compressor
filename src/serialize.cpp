
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
            s += get_size(obj.tree);
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
    struct get_size_helper<SuccinctTree> {
        static size_t value(const SuccinctTree &obj) {
            return obj.size_in_bytes();
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
template size_t get_size<SuccinctTree>(const SuccinctTree&);
template size_t get_size<Jvalue>(const Jvalue&);
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
            serializer(obj.namen, res);
            serializer(obj.valuen, res);
            // store the tree
            serializer(obj.tree, res);
            // store the encodes, names and values
            for (int i = 0; i < obj.size; ++i)
                serializer(obj.codes[i], res);
            for (int i = 0; i < obj.namen; ++i)
                serializer(obj.names[i], res);
            for (int i = 0; i < obj.valuen; ++i)
                serializer(obj.values[i], res);
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
    struct serialize_helper<SuccinctTree> {
        static void apply(const SuccinctTree &obj, StreamType::iterator &res) {
            char *p = obj.to_char_array();
            int size = obj.size_in_bytes();
            serialize_array(p, size, res);
        }
    };

    template <>
    struct serialize_helper<Jvalue> {
        static void apply(const Jvalue &obj, StreamType::iterator &res) {
            if (obj.isString()) serializer(obj.vstring, res);
            else if (obj.isInt()) serializer(obj.vint, res);
            else if (obj.isDouble()) serializer(obj.vdouble, res);
        }
    };

    template <>
    struct serialize_helper<encode> {
        static void apply(const encode &obj, StreamType::iterator &res) {
            serializer(obj.name, res);
            serializer(obj.type, res);
            if (obj.type >= 5) serializer(obj.value, res);
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
template void serialize<SuccinctTree>(const SuccinctTree&, StreamType&);
template void serialize<Jvalue>(const Jvalue&, StreamType&);
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
    struct deserialize_helper<SuccinctTree> {
        static SuccinctTree apply(int doc_size, StreamType::const_iterator& begin,
                StreamType::const_iterator end) {
            int size_in_bits = (doc_size + 1) * 2;
            int size_in_bytes = (size_in_bits + 7) / 8;
            char p[size_in_bytes];
            for (int i = 0; i < size_in_bytes; ++i)
                p[i] = deserialize_helper<char>::apply(begin, end);
            return SuccinctTree(p, size_in_bits);
        }
    };

    template <>
    struct deserialize_helper<Jvalue> {
        static Jvalue apply(int type, StreamType::const_iterator& begin,
                StreamType::const_iterator end) {
            types t = static_cast<types>(type);
            if (type == 5) return Jvalue(t, deserialize_helper<string>::apply(begin, end));
            if (type == 6) return Jvalue(t, deserialize_helper<int>::apply(begin, end));
            return Jvalue(t, deserialize_helper<double>::apply(begin, end));
        }
    };

    template <>
    struct deserialize_helper<encode> {
        static encode apply(StreamType::const_iterator& begin,
                StreamType::const_iterator end) {
            int name = -1, type = -1, value = -1;
            name = deserialize_helper<int>::apply(begin, end);
            type = deserialize_helper<int>::apply(begin, end);
            if (type >= 5) value = deserialize_helper<int>::apply(begin, end);
            return encode(name, type, value);
        }
    };

    template <>
    struct deserialize_helper<Parser> {
        static Parser apply(StreamType::const_iterator& begin,
                StreamType::const_iterator end) {
            // recover header
            int size = deserialize_helper<int>::apply(begin, end);
            int namen = deserialize_helper<int>::apply(begin, end);
            int valuen = deserialize_helper<int>::apply(begin, end);
            // recover the succinct tree
            SuccinctTree st = deserialize_helper<SuccinctTree>::apply(size, begin, end);
            // recover the encodes
            encode *codes = new encode[size];
            int vtypes[valuen];
            for (int i = 0; i < size; ++i) {
                codes[i] = deserialize_helper<encode>::apply(begin, end);
                if (codes[i].type >= 5) vtypes[codes[i].value] = codes[i].type;
            }
            // recover the names
            string *names = new string[namen];
            for (int i = 0; i < namen; ++i)
                names[i] = deserialize_helper<string>::apply(begin, end);
            // recover the values
            Jvalue *values = new Jvalue[valuen];
            for (int i = 0; i < valuen; ++i)
                values[i] = deserialize_helper<Jvalue>::apply(vtypes[i], begin, end);

            return Parser(size, namen, valuen, st, codes, names, values);
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
T deserialize(const StreamType &res) {
    StreamType::const_iterator it = res.begin();
    return deserialize<T>(it, res.end());
}

template Parser deserialize(StreamType::const_iterator&, const StreamType::const_iterator &);
template Parser deserialize(const StreamType&);
template SuccinctTree deserialize(int, StreamType::const_iterator&, const StreamType::const_iterator &);
template Jvalue deserialize(int, StreamType::const_iterator&, const StreamType::const_iterator &);
template string deserialize(StreamType::const_iterator&, const StreamType::const_iterator &);
template size_t deserialize(StreamType::const_iterator&, const StreamType::const_iterator &);
template int deserialize(StreamType::const_iterator&, const StreamType::const_iterator &);
template double deserialize(StreamType::const_iterator&, const StreamType::const_iterator &);

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

