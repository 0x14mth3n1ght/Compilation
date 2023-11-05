CXX=~/gcc12/bin/g++
CC=~/gcc12/bin/gcc
MPICC=mpicc

CXXFLAGS=-I`$(CC) -print-file-name=plugin`/include -g -Wall -fno-rtti -shared -fPIC
LDFLAGS=-g -O3
LIBDIR=lib
BINDIR=bin

.PHONY: all
all: $(LIBDIR)/libplugin.so $(BINDIR)/TP5_2

$(LIBDIR)/libplugin.so: src/plugin.cpp | $(LIBDIR)
	$(CXX) $(CXXFLAGS) -o $@ $<

$(BINDIR)/TP5_2: test/test3.c $(LIBDIR)/libplugin.so | $(BINDIR)
	OMPI_MPICC=$(CC) $(MPICC) $< $(LDFLAGS) -o $@ -fplugin=$(LIBDIR)/libplugin.so

$(LIBDIR):
	mkdir -p $@

$(BINDIR):
	mkdir -p $@

.PHONY: clean
clean:
	rm -rf $(LIBDIR)
	rm -rf $(BINDIR)
