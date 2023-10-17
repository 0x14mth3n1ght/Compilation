EXE= TP5_1 \
	TP5_2 \
	TP5_3 \
	TP5_4 \
	TP5_5 \
	TP5_6 \
	TP5_7
	
all: $(EXE)

CXX=~/gcc12/bin/g++
CC=~/gcc12/bin/gcc

MPICC=mpicc

PLUGIN_FLAGS=-I`$(CC) -print-file-name=plugin`/include -g -Wall -fno-rtti -shared -fPIC

CFLAGS=-g -O3

DOT_FILES := $(wildcard *.dot)

libplugin_%.so: plugin_%.cpp
	$(CXX) $(PLUGIN_FLAGS) $(GMP_CFLAGS) -o $@ $<

% : libplugin_%.so test3.c
	OMPI_MPICC=$(CC) $(MPICC) test3.c $(CFLAGS) -o $@ -fplugin=./$< 

%.png: %.dot
	dot -Tpng $< > $@

png: $(patsubst %.dot,%.png,$(DOT_FILES))

clean:
	rm -rf $(EXE)

clean_all: clean
	rm -rf libplugin*.so *.dot
