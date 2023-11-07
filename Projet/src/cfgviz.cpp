#include "../headers/cfgviz.h"

/* Build a filename (as a string) based on function name */
static char* cfgviz_generate_filename(function* fun, const char* suffix)
{
    char* target_filename;

    target_filename = (char*)xmalloc(2048 * sizeof(char));

    snprintf( target_filename, 1024, "%s_%s_%d_%s.dot",
              function_name(fun),
              basename(LOCATION_FILE( fun->function_start_locus )),
              LOCATION_LINE( fun->function_start_locus ),
              suffix ) ;

    return target_filename;
}

/* Dump the graphviz representation of function 'fun' in file 'out' */
static void cfgviz_internal_dump(function* fun, FILE* out)
{
    // Print the header line and open the main graph
    fprintf(out, "Digraph G{\n");

    basic_block bb;
    FOR_EACH_BB_FN(bb, fun)
    {
        enum mpi_collective_code code = (enum mpi_collective_code)(unsigned long)bb->aux;
        if (code == LAST_AND_UNUSED_MPI_COLLECTIVE_CODE) {
            fprintf(out, "\tN%d [label=\"Node %02d\" shape=ellipse]\n", bb->index, bb->index);
        } else {
            fprintf(out, "\tN%d [label=\"Node %02d\\nMPI: %d\" shape=ellipse]\n", bb->index, bb->index, code);
        }
    }

    FOR_EACH_BB_FN(bb, fun)
    {
        auto* succs = bb->succs;
        if (!succs) {
            continue;
        }
        const unsigned int length = succs->length();
        for (unsigned int i = 0; i < length; i++) {
            edge e = (*succs)[i];
            fprintf(out, "\tN%d -> N%d [color=red label=\"\"]\n", e->src->index, e->dest->index);
        }
    }

    // Close the main graph
    fprintf(out, "}\n");
}

void cfgviz_dump(function* fun, const char* suffix)
{
    char* target_filename;
    FILE* out;

    target_filename = cfgviz_generate_filename(fun, suffix);

    printf("[GRAPHVIZ] Generating CFG of function %s in file <%s>\n",
        current_function_name(), target_filename);

    out = fopen(target_filename, "w");

    cfgviz_internal_dump(fun, out);

    fclose(out);
    free(target_filename);
}

void clear_all_bb_aux(function* fun)
{
    basic_block bb;
    FOR_EACH_BB_FN(bb, fun)
    {
        bb->aux = (void*)NULL;
    }
}

