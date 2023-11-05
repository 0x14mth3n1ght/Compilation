#include <string.h>
#include <stdio.h>
#include "../headers/plugin.h"

// Liste pour stocker les fonctions spécifiées dans les directives
vec<const unsigned char*> pragma_instrumented_functions {};

void pragma_mpicoll_check(cpp_reader* ARG_UNUSED(dummy))
{
    if (cfun) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
        error("%<#pragma instrumente%> option is not allowed inside functions");
#pragma GCC diagnostic pop
        return;
    }

    bool correctly_formatted = true;
    bool need_closing_parenthese = false;
    tree x;
    enum cpp_ttype token;
    vec<const unsigned char*> pending_functions {};

    // Checking for the opening parenthese, if any.
    token = pragma_lex(&x);
    if (CPP_OPEN_PAREN == token) {
        need_closing_parenthese = true;
        token = pragma_lex(&x);
    }

    while (CPP_NAME == token) {
        const unsigned char* name = x->identifier.id.str;
        pending_functions.safe_push(name);

        do {
            token = pragma_lex(&x);
        } while (CPP_COMMA == token);
    }

    if (need_closing_parenthese && CPP_CLOSE_PAREN != token) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
        error("%<#pragma instrumente (name1[, name2]...)%> is missing a closing %<)%>");
#pragma GCC diagnostic pop
        correctly_formatted = false;
    }

    token = pragma_lex(&x);
    if (CPP_EOF != token) {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
        error("%<#pragma instrumente ...%> is badly formed");
#pragma GCC diagnostic pop
        correctly_formatted = false;
    }

    if (correctly_formatted) {
        pragma_instrumented_functions.reserve_exact(pragma_instrumented_functions.length() + pending_functions.length());
        while (!pending_functions.is_empty()) {
            const unsigned char* name = pending_functions.pop();
            if (pragma_instrumented_functions.contains(name)) {
                fprintf(stderr, "%s: warning: '%s' is duplicated in '#pragma instrumente ...'\n", progname, name);
            } else {
                pragma_instrumented_functions.safe_push(name);
            }
        }
    }
}

void my_callback_mpicoll_register(void *event_data, void *data)
{
    (void) event_data;
    (void) data;
    c_register_pragma("ProjetCA", "mpicoll_check", pragma_mpicoll_check);
}



