# Json Compressor

<img src="http://www.risk.net/IMG/899/276899/squash-compression.jpg" alt="Json
Compression" style="width: 260px;"/>

> Json file compression with succinct data structures.
<hr>

* [Team Members](#team-members)
* [Tech](#tech)
* [Build](#build)
* [Tests](#tests)

#### <a name="team-members"></a>Team members

This project is being developed by:
* Edman P. Anjos (<edman@tcs.snu.ac.kr>)
* Junhee Lee (<jvl@tcs.snu.ac.kr>)


#### <a name="tech"></a>Tech

Json Compressor depends on a number of open source projects to work properly.

* [SDSL](https://github.com/simongog/sdsl-lite) - Succinct Data Structure
  Library that provides several useful data structures, such as *Rank/Select*
and *Balanced Parentheses*.

* [RapidJSON](https://github.com/miloyip/rapidjson) - Json parser and generator
  for C++, used on an intermediate step to read Json files into *Json
Compressor*.

* [Google Test](https://github.com/google/googletest) - Google's C++ unit
  testing framework.


#### <a name="build"></a>Build

Install SDSL in your system globally.

```sh
git clone https://github.com/simongog/sdsl-lite.git
cd sdsl-lite
./install.sh /user/local
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

