EXE=bin/plugin
SRC_DIR=src
OBJ_DIR=obj
CXX=~/gcc12/bin/g++
CC=~/gcc12/bin/gcc
MPICC=mpicc
PLUGIN_FLAGS=-I`$(CC) -print-file-name=plugin`/include -g -Wall -fno-rtti -shared -fPIC
CFLAGS=-g -O3
DOT_FILES := $(wildcard *.dot)

$(OBJ_DIR)/plugin.o: $(SRC_DIR)/plugin.cpp | $(OBJ_DIR)
	$(CXX) -c $(PLUGIN_FLAGS) $(GMP_CFLAGS) -o $@ $<

$(EXE): $(OBJ_DIR)/plugin.o libplugin.so
	OMPI_MPICC=$(CC) $(MPICC) test/test3.c $(CFLAGS) -o $@ -fplugin=./libplugin.so

%.png: %.dot
	dot -Tpng $< > $@

png: $(patsubst %.dot,%.png,$(DOT_FILES))

$(OBJ_DIR):
	mkdir -p $@

clean:
	rm -rf $(OBJ_DIR) $(EXE)

clean_all: clean
	rm -rf libplugin*.so *.dot
