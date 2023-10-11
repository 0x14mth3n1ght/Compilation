EXE= TP4_5 \
	TP4_6 \
	TP4_7
	
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
	rm -rf libplugin*.so *.dot *.png
