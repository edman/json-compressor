#include <iostream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/stringbuffer.h"
#include "cjson.hpp"
#include "jvalue.hpp"
#include "bp_tree.hpp"
#include "bp_traversal.hpp"
#include "df_tree.hpp"
#include "df_traversal.hpp"
#include "serialize.hpp"

#include "json.hpp"
#include "jsoncpp/json.h"

#include <thread>
#include <chrono>
#include <cstdio>

using namespace std;
using namespace rapidjson;
using namespace sdsl;

int fib(int n) { if (n <= 1) return n; return fib(n - 1) + fib(n - 2); }

void bebusy() {
    fib(25);
    // const int LARGE_MEM = 1000000;
    // void *p = malloc(LARGE_MEM);
    // this_thread::sleep_for(chrono::milliseconds(50));
    // free(p);
}

long filesize(const char *);
template <class T> void log_cjson_size(Cjson<T> &, const char *, long);

bool check_arguments(int argc, char *argv[]) {
    if (argc > 3 || argc == 1) {
        cout << "Usage" << endl;
        cout << "runner [-rc] <file.json>" << endl;
        return false;
    }
    return true;
}

Json::Value* jsoncpp_from_file(char *filename) {
    ifstream file(filename, std::ifstream::binary);
    Json::Value *v = new Json::Value();
    file >> *v;
    return v;
}

nlohmann::json* modernjson_from_file(char *filename) {
    ifstream file;
    file.open(filename);
    nlohmann::json *j = new nlohmann::json();
    file >> *j;
    return j;
}

void modernjson_usage_test(char *filename) {
    nlohmann::json *j = modernjson_from_file(filename);
}

Document* rapidjson_document_from_file(char *filename) {
	FILE *fp;
	char buf[2 << 16];
	fp = fopen(filename, "r");
	FileReadStream is(fp, buf, sizeof(buf));
    Document *d = new Document();
	d->ParseStream(is);
    fclose(fp);

    return d;
}

void rapidjson_usage_test(char *filename) {
	Document *d = rapidjson_document_from_file(filename);
    // cout << d << endl;
    // delete d;

    // // 1. Parse a JSON string into DOM.
    // const char* json = "{\"project\":\"rapidjson\",\"stars\":10}";
    // Document d;
    // d.Parse(json);
    //
    // // 2. Modify it by DOM.
    // Value& s = d["stars"];
    // s.SetInt(s.GetInt() + 2);
    //
    // // 3. Stringify the DOM
    // StringBuffer buffer;
    // Writer<StringBuffer> writer(buffer);
    // d.Accept(writer);
    //
    // // Output {"project":"rapidjson","stars":12}
    // cout << buffer.GetString() << endl;
}

Cjson<DfTree>* cjson_df_from_file(char *filename) {
    Document *d = rapidjson_document_from_file(filename);
	Cjson<DfTree> *p = new Cjson<DfTree>(*d);
    delete d;
    return p;
}

Cjson<BpTree>* cjson_bp_from_file(char *filename) {
    Document *d = rapidjson_document_from_file(filename);
	Cjson<BpTree> *p = new Cjson<BpTree>(*d);
    delete d;
    return p;
}

template <class T>
void cjson_save(Cjson<T> &p, char *filename) {
	char fn[128];
	strcpy(fn, filename); strcat(fn, "_c");
	save_to_file(p, fn);
}

template <class T>
void cjson_log(Cjson<T> &p, char *filename) {
	char fn[128];
	strcpy(fn, filename); strcat(fn, "_h");
	long original_size = filesize(filename);
	log_cjson_size(p, fn, original_size);
}

void cjson_usage_test(char *fnarg, bool bp) {
    // bebusy();
    if (bp) {
        Cjson<BpTree> *p = cjson_bp_from_file(fnarg);
        cout << "BpTraversal creation" << endl;
        BpTraversal *tbp = new BpTraversal(p);
    }
    else {
        Cjson<DfTree> *p = cjson_df_from_file(fnarg);
        cout << "DFTraversal creation" << endl;
        DfTraversal *tdf = new DfTraversal(p);
     }
    // bebusy();
    // delete p;


    // bebusy();

    // cjson_save(*p, fnarg);
    //
    // bebusy();
    // cjson_log(*p, fnarg);
}

int main(int argc, char *argv[]) {
    // Usage => "runner file.json"
    if (!check_arguments(argc, argv)) { return 1; }

    if (strcmp(argv[1], "-r") == 0) rapidjson_usage_test(argv[2]);
    else if (strcmp(argv[1], "-m") == 0) modernjson_usage_test(argv[2]);
    else if (strcmp(argv[1], "-c") == 0) jsoncpp_from_file(argv[2]);
    else if (strcmp(argv[1], "-bp") == 0) cjson_usage_test(argv[2], true);
    else if (strcmp(argv[1], "-df") == 0) cjson_usage_test(argv[2], false);

    // bebusy();

    // Right now we have 3 bebusy calls
    // 1 -> right before json processing begins
    // 2 -> right after json structure is processed and in memory
    // 3 -> right before program execution ends
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


template <class T>
void log_cjson_size(Cjson<T> &obj, const char *fn, long orig) {
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

