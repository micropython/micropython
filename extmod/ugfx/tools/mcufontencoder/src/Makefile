CXXFLAGS = -O2 -Wall -Werror -Wno-unused-function -Wno-sign-compare -std=c++0x
CXXFLAGS += -ggdb
LDFLAGS += -pthread

# Libfreetype
CXXFLAGS += $(shell freetype-config --cflags)
LDFLAGS += $(shell freetype-config --libs)

# Class to represent font data internally
OBJS = datafile.o

# Utility functions
OBJS += importtools.o exporttools.o

# Import formats
OBJS += bdf_import.o freetype_import.o

# rlefont export format
OBJS += encode_rlefont.o optimize_rlefont.o export_rlefont.o

# bwfont export format
OBJS += export_bwfont.o


all: run_unittests mcufont

clean:
	rm -f unittests unittests.cc mcufont $(OBJS)

mcufont: main.o $(OBJS)
	g++ $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

unittests.cc: *.hh
	cxxtestgen --have-eh --error-printer -o unittests.cc $^

unittests: unittests.cc $(OBJS)
	g++ $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

run_unittests: unittests
	./unittests

%.o: %.cc *.hh
	g++ $(CXXFLAGS) -c $<
