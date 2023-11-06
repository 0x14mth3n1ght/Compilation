#include <string.h>
#include <stdio.h>
#include "../headers/plugin.h"

vec<pragma_el> mpi_pragmas::seen_in_pragmas = vNULL; //NOLINT

void handle_instrument_function(__attribute__((unused)) cpp_reader *dummy){
    tree data = nullptr;
    location_t token_loc;
	// Get the first token after a '#pragma instrument function ' define line
	// token_loc keeps track of where is the parsed token
	// token_loc is kept to create pretty gcc user outputs
    enum cpp_ttype token = pragma_lex(&data, &token_loc);

	// We don't allow to define our pragma inside a function
    if (cfun) {
        error_at(token_loc, "%s %<#pragma ProjetCA mpicoll_check%> %s", "Can't use", "inside functions !");
        return;
    }
	
	// Check if we are in a "#pragma instrument function fun_name" form
    if(token == CPP_NAME){
        mpi_pragmas::seen_in_pragmas.safe_push({
            .name = (const char*)data->identifier.id.str,
            .loc = token_loc
        });
        return;
    }
	
	// The first form has been checked, so we check if first token is an open parenthesis (second form)
	// The second form is "#pragma instrument function(fun_name,fun_name2)"
    if(token != CPP_OPEN_PAREN){
        error_at(token_loc, "Unsupported pragma token");
        return;
    }
	
	// We iterate on all tokens on the line until we reach a close parenthesis or the end of the line
    while((token = pragma_lex(&data, &token_loc)) != CPP_CLOSE_PAREN && token != CPP_EOF){
		// If we hit a name, we store it in the seen_in_pragmas
		// If we hit a comma, we ignore it and continue
		// Else we can output that the current token is not supported
        switch(token){
            case CPP_COMMA:
                continue;
            case CPP_NAME:
                mpi_pragmas::seen_in_pragmas.safe_push({
                    .name = (const char*)data->identifier.id.str,
                    .loc = token_loc
                });
                break;
            default:
                error_at(token_loc, "Unsupported pragma token");
                return;
        }
    }
}

void mpi_pragmas::my_callback_mpicoll_register(__attribute__((unused)) void *event_data,__attribute__((unused)) void *data) {
    c_register_pragma("ProjetCA", "mpicoll_check", &handle_instrument_function);
}



