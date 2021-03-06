
#ifndef __SERIALIZE_HPP__
#define __SERIALIZE_HPP__

#include "cjson.hpp"
#include <cstdlib>

typedef std::vector<uint8_t> StreamType;

template <class T> size_t get_size(const T &obj);

template <class T> void serialize(const T &obj, StreamType &res);

template <class T> T deserialize(StreamType::const_iterator&, const StreamType::const_iterator&);
template <class T> T deserialize(int, StreamType::const_iterator&, const StreamType::const_iterator&);
template <class T> T deserialize(const StreamType&);

void save_to_file(StreamType&, const string&);

template <class T> void save_to_file(Cjson<T>&, const string&);
template <class T> void save_to_file_split(Cjson<T>&, const string&);
template <class T> Cjson<T> load_from_file(const string&);
template <class T> Cjson<T> load_from_file_split(const string&);

#endif

