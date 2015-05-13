
# As seen on
# http://hiltmon.com/blog/2013/07/03/a-simple-c-plus-plus-project-structure/

# Use this to inspect a variable
# $(info $$OBJECTS is [${OBJECTS}])

CC := g++ # This is the main compiler
# CC := clang --analyze # and comment out the linker last line for sanity
SRCDIR := src
BUILDDIR := build
LIBDIR := lib
TARGET := bin/runner

SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))
CFLAGS := -g -Wall
# LIB := -pthread -L lib -lboost_thread-mt -lboost_filesystem-mt -lboost_system-mt
LIB := -L $(LIBDIR)
INC := -I include

GTESTDIR := test/gtest-1.7.0
OBJECTS_NO_MAIN := $(patsubst $(BUILDDIR)/main.o,,$(OBJECTS))

$(TARGET): $(OBJECTS)
	@echo " Linking...";
	$(CC) $^ -o $(TARGET) $(LIB)
	@echo " Running..."; $(TARGET)


$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning...";
	$(RM) -r $(BUILDDIR) $(TARGET)

# Tests
$(BUILDDIR)/gtest-all.o: $(GTESTDIR)/src/gtest-all.cc
	g++ -isystem $(GTESTDIR)/include -I$(GTESTDIR) -pthread -c \
		$(GTESTDIR)/src/gtest-all.cc -o build/gtest-all.o

$(BUILDDIR)/gtest_main.o: $(GTESTDIR)/src/gtest_main.cc
	g++ -isystem $(GTESTDIR)/include -I$(GTESTDIR) -pthread -c \
		$(GTESTDIR)/src/gtest_main.cc -o build/gtest_main.o

$(LIBDIR)/gtest_main.a: $(BUILDDIR)/gtest_main.o $(BUILDDIR)/gtest-all.o
	@echo " Creating gtest library archive..."
	ar -rv $(LIBDIR)/gtest_main.a $(BUILDDIR)/gtest-all.o $(BUILDDIR)/gtest_main.o

test: $(OBJECTS_NO_MAIN) test/tester.cpp $(LIBDIR)/gtest_main.a
	@echo " Building tests...";
	$(CC) $(CFLAGS) -isystem ${GTESTDIR}/include $(INC) $(LIB) -pthread \
		$^ -o bin/tester
	@echo " Running tests..."
	bin/tester


.PHONY: clean

