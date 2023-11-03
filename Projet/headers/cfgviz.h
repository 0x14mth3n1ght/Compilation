#ifndef CFGVIZ
#define CFGVIZ

// Produce .dot files to observe function graphs.
//
// `errno` is checked for when opening/closing files.
#pragma once

#include "plugin.h"

// Dumps a function as a graph, appending `suffix` to the filename.
void cfgviz_dump(function* fun, const char* const suffix);

#endif CFGVIZ
