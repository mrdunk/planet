
IRRLICHTHOME = /usr/include/irrlicht

# Points to the root of Google Test, relative to where this file is.
# Remember to tweak this if you move this file.
GTEST_DIR = ./gtest

# Where to find user code.
USER_DIR = .

# Flags passed to the preprocessor.
# Set Google Test's header directory as a system directory, such that
# the compiler doesn't generate warnings in Google Test headers.
CPPFLAGS += -std=c++11
CPPFLAGS_TEST = $(CPPFLAGS) -isystem $(GTEST_DIR)/include
CPPFLAGS_PLANET += $(CPPFLAGS) -isystem $(IRRLICHTHOME) -I/usr/X11R6/include

# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -Wextra -pthread

# All tests produced by this Makefile.  Remember to add new tests you
# created to the list.
TESTS = data_test

# All Google Test headers.  Usually you shouldn't change this
# definition.
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

# House-keeping build targets.

all : $(TESTS) planet

clean :
	(cd ./bin/; rm -f $(TESTS) planet)
	rm -f gtest.a gtest_main.a *.o

# Builds gtest.a and gtest_main.a.

# Usually you shouldn't tweak such internal variables, indicated by a
# trailing _.
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)


# For simplicity and to avoid depending on Google Test's
# implementation details, the dependencies specified below are
# conservative and not optimized.  This is fine as Google Test
# compiles fast and for ordinary users its source rarely changes.
gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS_TEST) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS_TEST) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc

gtest.a : gtest-all.o
	$(AR) $(ARFLAGS) $@ $^

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) $@ $^


# Builds components required by all targets.
planetMath.o : $(USER_DIR)/planetMath.cc $(USER_DIR)/planetMath.h $(USER_DIR)/defines.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/planetMath.cc

data.o : $(USER_DIR)/data.cc $(USER_DIR)/data.h $(USER_DIR)/defines.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $(USER_DIR)/data.cc


# Builds unit tests.  A test should link with either gtest.a or
# gtest_main.a, depending on whether it defines its own main()
# function.
data_test.o : planetMath.o data.o $(USER_DIR)/data_test.cc $(GTEST_HEADERS)
	$(CXX) $(CPPFLAGS_TEST) $(CXXFLAGS) -c $(USER_DIR)/data_test.cc

data_test : data.o data_test.o planetMath.o gtest_main.a
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ -o ./bin/$@


# Builds main program.
planetIrrlicht.o : $(USER_DIR)/planetIrrlicht.h $(USER_DIR)/planetIrrlicht.cc $(USER_DIR)/defines.h
	$(warning Building irrlicht...)
	$(CXX) $(CPPFLAGS_PLANET) $(CXXFLAGS) -c $(USER_DIR)/planetIrrlicht.cc

main.o : planetMath.o data.o $(USER_DIR)/main.cc $(USER_DIR)/defines.h
	$(warning Building main...)
	$(CXX) $(CPPFLAGS_PLANET) $(CXXFLAGS) -ffast-math -c $(USER_DIR)/main.cc

planet : data.o planetMath.o main.o planetIrrlicht.o
	$(warning Linking...)
	$(CXX) $(CPPFLAGS_PLANET) $(CXXFLAGS) -lpthread $^ -o ./bin/$@  -lIrrlicht -lGL -lXxf86vm -lXext -lX11 -lXcursor
