#ifndef CA_PRAGMA
#define CA_PRAGMA

#pragma once

#include "plugin.h"

typedef struct {
    const char* name;
    location_t loc;
} pragma_el;

/**
	Handles our '#pragma instrument function' logic and registration 
*/
class mpi_pragmas {
public:
	/**
		Vector that contains function names declared by our pragma
	*/
    static vec<pragma_el> seen_in_pragmas;
	/**
		Required by the gcc plugin API
		Handles pragma registration to gcc
		
		@param event_data gcc event data given by register_callback api function
		@param data User defined data
	*/
    static void my_callback_mpicoll_register(void* event_data, void* data);
};

#endif
