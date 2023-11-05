#ifndef CA_PRAGMA
#define CA_PRAGMA

#pragma once

#include "plugin.h"

// Returns `true` when the given function should be checked for its sequence
// by the plugin.
void pragma_mpicoll_check(cpp_reader* ARG_UNUSED(dummy));

// Will raise warnings when the functions registered by `#pragma ProjetCA`
// instructions have not been all found in the code.

// Will register the `#pragma ProjetCA mpicoll_check` instruction to allow for
// its detection and register the functions found by this instruction to be
// used in the other functions `ca_pragma_*`.
void my_callback_mpicoll_register(void* event_data, void* data);

#endif
