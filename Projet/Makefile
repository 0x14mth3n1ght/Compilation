CXX=~/gcc12/bin/g++
CC=~/gcc12/bin/gcc
MPICC=mpicc

CXXFLAGS=-I`$(CC) -print-file-name=plugin`/include -g -Wall -fno-rtti -shared -fPIC
LDFLAGS=-g -O3
LIBDIR=lib
BINDIR=bin
DOT_FILES= $(wildcard *.dot)
TEST_FILES= $(wildcard test/*.c)

.PHONY: all
all: $(LIBDIR)/libplugin.so $(TEST_FILES:test/%.c=$(BINDIR)/%)

$(LIBDIR)/libplugin.so: src/plugin.cpp | $(LIBDIR)
	$(CXX) $(CXXFLAGS) -o $@ $<

$(BINDIR)/%: test/%.c $(LIBDIR)/libplugin.so | $(BINDIR)
	OMPI_MPICC=$(CC) $(MPICC) $< $(LDFLAGS) -o $@ -fplugin=$(LIBDIR)/libplugin.so

$(LIBDIR):
	mkdir -p $@

$(BINDIR):
	mkdir -p $@

%.png: %.dot
	dot -Tpng $< > $@

png: $(patsubst %.dot,%.png,$(DOT_FILES))

.PHONY: clean
clean:
	rm -rf $(LIBDIR)
	rm -rf $(BINDIR)
