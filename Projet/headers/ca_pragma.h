#ifndef CA_PRAGMA
#define CA_PRAGMA

#pragma once

#include "plugin.h"

// Returns `true` when the given function should be checked for its sequence
// by the plugin.
bool ca_pragma_visit_function(function* fun);

// Will raise warnings when the functions registered by `#pragma ProjetCA`
// instructions have not been all found in the code.
void ca_pragma_check_visited_all_function(void*, void*);

// Will register the `#pragma ProjetCA mpicoll_check` instruction to allow for
// its detection and register the functions found by this instruction to be
// used in the other functions `ca_pragma_*`.
void ca_pragma_find_functions_to_visit(void*, void*);

#endif CA_PRAGMA
