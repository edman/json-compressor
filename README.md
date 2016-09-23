# Json Compressor

<img src="http://www.risk.net/IMG/899/276899/squash-compression.jpg" alt="Json
Compression" style="width: 260px;"/>

> JSON file compression with succinct data structures.
<hr>

* [Team Members](#team-members)
* [Tech](#tech)
* [Build](#build)
* [Tests](#tests)
* [Publications](#publications)
* [Future Works](#future)

#### <a name="team-members"></a>Team Members

This project is being developed by:
* Edman P. Anjos (<edman@tcs.snu.ac.kr>)
* Junhee Lee (<jvl@tcs.snu.ac.kr>)


#### <a name="tech"></a>Tech

Json Compressor depends on a number of open source projects to work properly.

* [SDSL](https://github.com/simongog/sdsl-lite) - Succinct Data Structure
  Library that provides several useful data structures, such as *Rank/Select*
and *Balanced Parentheses*.

* [RapidJSON](https://github.com/miloyip/rapidjson) - JSON parser and generator
  for C++, used on an intermediate step to read JSON files into *Json
Compressor*.

* [Google Test](https://github.com/google/googletest) - Google's C++ unit
  testing framework.


#### <a name="build"></a>Build

Install SDSL in your system globally.

```sh
git clone https://github.com/simongog/sdsl-lite.git
cd sdsl-lite
./install.sh /usr/local
```

Other required libraries are already included in the project. Considering you
have `g++` installed all you need to do to compile *Json Compressor* is to run
from the project root directory.

```sh
$ make
```


#### <a name="tests"></a>Tests

Similarly, test can be built and run with

```sh
$ make test
```

#### <a name="publications"></a>Publications

- Edman Anjos, Junhee Lee, Srinivasa Rao Satti: *SJSON: A Succinct Representation for JSON Documents*. to appear in 11th International Conference on Digital Information Management (ICDIM), 2016.


#### <a name=future"></a>Future Works

- Possible combination of text compression/transformation.

- Optimization of mechanism dealing with *String*s.

- Integration to XML and/or different document exchange formats.

- Supporting various queries.
