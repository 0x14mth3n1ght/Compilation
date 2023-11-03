#include <string.h>
#include <stdio.h>
#include "../include/plugin.h"
#include "../headers/cfgviz.h"
#include "../headers/mpi_collectives.h"

// Liste pour stocker les fonctions spécifiées dans les directives
vec<const char *> specified_functions {};

void pragma_mpicoll_check(cpp_reader* ARG_UNUSED(dummy)) {
    if (cfun) {
        // Avertissement : La directive ne peut être utilisée qu'en dehors des fonctions
        warning_at(input_location, 0, "Directive mpicoll_check ne peut être utilisée à l'intérieur d'une fonction");
        return;
    }

    bool need_closing_parenthese = false;
    tree x;
    enum cpp_ttype token;

    token = pragma_lex(&x);
    if (CPP_OPEN_PAREN == token) {
        need_closing_parenthese = true;
        token = pragma_lex(&x);
    }

    while (CPP_NAME == token) {
        const char* function_name = x->identifier.id.str;

        // Vérifier l'existence de la fonction dans le code source
        if (!function_exists_in_code(function_name)) {
            // Avertissement : La fonction spécifiée dans la directive n'existe pas dans le code source
            warning_at(input_location, 0, "La fonction '%s' spécifiée dans mpicoll_check n'existe pas dans le code source", function_name);
        }

        // Vérifier si la fonction a déjà été spécifiée précédemment
        if (specified_functions.contains(function_name)) {
            // Avertissement : La fonction est spécifiée plusieurs fois dans les directives
            warning_at(input_location, 0, "La fonction '%s' est spécifiée plusieurs fois dans les directives mpicoll_check", function_name);
        } else {
            specified_functions.safe_push(function_name);
        }

        // Passer à la prochaine fonction spécifiée
        do {
            token = pragma_lex(&x);
        } while (CPP_COMMA == token);
    }

    if (need_closing_parenthese && CPP_CLOSE_PAREN != token) {
        // Avertissement : Format incorrect pour la directive
        warning_at(input_location, 0, "Directive mpicoll_check mal formatée (parenthèse fermante manquante)");
        return;
    }

    if (CPP_EOF != token) {
        // Avertissement : Format incorrect pour la directive
        warning_at(input_location, 0, "Directive mpicoll_check mal formatée");
    }
}

void my_callback_mpicoll_register(void*, void*)
{
    c_register_pragma("ProjetCA", "mpicoll_check", pragma_mpicoll_check);
}

int plugin_is_GPL_compatible = 1;

int plugin_init(struct plugin_name_args* plugin_info, struct plugin_gcc_version* version) {
    if (!plugin_default_version_check(version, &gcc_version)) {
        return 1;
    }

    // Enregistrement de la fonction de traitement de la directive mpicoll_check
    my_callback_register = c_register_pragma("ProjetCA", "mpicoll_check", pragma_mpicoll_check);
    
    // Vérification des autres enregistrements de callback
    if (!my_callback_register) {
        // Gérer l'échec de l'enregistrement de la fonction de traitement de la directive
        fprintf(stderr, "Échec de l'enregistrement de la fonction de traitement de la directive mpicoll_check.\n");
        return 1;
    }
    
    // Autres enregistrements de callbacks, le cas échéant...

    return 0;
}
