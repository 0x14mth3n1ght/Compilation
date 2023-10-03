EXE= TP3_1 \
	TP3_2 \
	TP3_3 \
	TP3_4 \
	TP3_5 \
	TP3_6 \
	TP3_6_bis \
	TP3_7 \
	TP3_8

all: $(EXE)

CXX=~/gcc12/bin/g++
CC=~/gcc12/bin/gcc

MPICC=mpicc

PLUGIN_FLAGS=-I`$(CC) -print-file-name=plugin`/include -g -Wall -fno-rtti -shared -fPIC

CFLAGS=-g -O3


libplugin_%.so: plugin_%.cpp
	$(CXX) $(PLUGIN_FLAGS) -o $@ $<

% : libplugin_%.so test2.c
	OMPI_MPICC=$(CC) $(MPICC) test2.c $(CFLAGS) -o $@ -fplugin=./$< 

clean:
	rm -rf $(EXE)

clean_all: clean
	rm -rf libplugin*.so *.dot
