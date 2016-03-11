#include <iostream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/stringbuffer.h"
#include "parser.hpp"
#include "serialize.hpp"

#include <cstdio>

using namespace std;
using namespace rapidjson;
using namespace sdsl;

long filesize(const char *);
void log_parser_size(Parser &, const char *, long);

int main(int argc, char *argv[]) {
    // 1. Parse a JSON string into DOM.
    const char* json = "{\"project\":\"rapidjson\",\"stars\":10}";
    Document d;
    d.Parse(json);

    // 2. Modify it by DOM.
    Value& s = d["stars"];
    s.SetInt(s.GetInt() + 1);

    // 3. Stringify the DOM
    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    d.Accept(writer);

    // Output {"project":"rapidjson","stars":11}
    cout << buffer.GetString() << endl;


	FILE *fp;
	char *buf = new char[2 << 16];
	char fn[128];
	fp = fopen(argv[1], "r");
	FileReadStream is(fp, buf, sizeof(buf));
	d.ParseStream(is);
	Parser p(d, true);
	fclose(fp);
	strcpy(fn, argv[1]);
	strcat(fn, "_c");
	save_to_file(p, fn);
	strcpy(fn, argv[1]);
	strcat(fn, "_h");
	long orig = filesize(argv[1]);
	log_parser_size(p, fn, orig);

	delete[] buf;

    return 0;
}

long filesize(const char *fn) {
    ifstream ifile;
    ifile.open(fn, ios::binary);
    ifile.seekg(0, ifile.end);
    long size = ifile.tellg();
    ifile.close();
    return size;
}

void write_formatted(ofstream &f, const string &msg, long &a, size_t &b) {
    f << msg << "\t\t(" << (int)((double)a/b*100) << "%)\t\t" << a << endl;
}

void log_parser_size(Parser &obj, const char *fn, long orig) {
    ofstream mfile(fn, ios::out);
    // original file size
    mfile << "original size (bytes): " << orig << endl;

    long header, tree, namel, names, values;
    // size of header
    size_t s = sizeof(int);
    header = s;
    // size of tree
    tree = get_size(obj.tree);
    s += tree;
    namel = get_size(obj.nameList);
    s += namel;
    // size of names
    names = get_size(obj.names);
    s += names;
    // size of values
    values = get_size(obj.values);
    s += values;

    write_formatted(mfile, ".header", header, s);
    write_formatted(mfile, ".tree", tree, s);
    write_formatted(mfile, ".names", names, s);
    write_formatted(mfile, ".namel", namel, s);
    write_formatted(mfile, ".values", values, s);
    mfile << "Total " << s << endl;

    // compression ratio
    mfile << "Compression ratio (bigger is better): " << (double)orig/s << endl;
    mfile << endl;

    cout << "original size: " << orig << endl;
    cout << "compressed: " << s << endl;
    cout << "ratio (bigger better): " << (double)orig/s << endl << endl;

    mfile.close();
}
