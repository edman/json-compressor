#include <iostream>
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "parser.hpp"

#include <fstream>
#include <cstdio>

using namespace std;
using namespace rapidjson;
using namespace sdsl;

int main(int argc, char *argv[]) {
    Document d;
	FILE *fp = fopen(argv[1], "r");
	unsigned long len;
	fseek(fp, 0L, SEEK_END);
	len = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	char *rb = new char[len];
	fread(rb, 1, len, fp);
	std::cout << "Original size of the file: " << len << std::endl;
	d.Parse(rb);
	Parser qn(d);
	fclose(fp);

    return 0;
}
