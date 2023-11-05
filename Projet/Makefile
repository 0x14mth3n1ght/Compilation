CXX=~/gcc12/bin/g++
CC=~/gcc12/bin/gcc
MPICC=mpicc

CXXFLAGS=-I`$(CC) -print-file-name=plugin`/include -g -Wall -fno-rtti -shared -fPIC
LDFLAGS=-g -O3
LIBDIR=lib
BINDIR=bin
DOT_FILES= $(wildcard *.dot)

.PHONY: all
all: $(LIBDIR)/libplugin.so $(BINDIR)/Projet

$(LIBDIR)/libplugin.so: src/plugin.cpp | $(LIBDIR)
	$(CXX) $(CXXFLAGS) -o $@ $<

$(BINDIR)/Projet: test/test3.c $(LIBDIR)/libplugin.so | $(BINDIR)
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
