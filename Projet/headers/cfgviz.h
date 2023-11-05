#ifndef CFGVIZ
#define CFGVIZ

// Produce .dot files to observe function graphs.
//
// `errno` is checked for when opening/closing files.
#pragma once

#include "mpi_collectives.h"

static char* cfgviz_generate_filename(function* fun, const char* suffix);
static void cfgviz_internal_dump(function* fun, FILE* out);

// Dumps a function as a graph, appending `suffix` to the filename.
void cfgviz_dump(function* fun, const char* suffix);

void clear_all_bb_aux(function* fun);

#endif
