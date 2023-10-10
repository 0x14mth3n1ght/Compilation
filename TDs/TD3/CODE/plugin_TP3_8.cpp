#include <gcc-plugin.h>
#include <plugin-version.h>
#include <tree.h>
#include <basic-block.h>
#include <gimple.h>
#include <tree-pass.h>
#include <context.h>
#include <function.h>
#include <gimple-iterator.h>

#include <string.h>

// Enum to represent the collective operations
#define DEFMPICOLLECTIVES(CODE, NAME) CODE,
enum mpi_collective_code {
#include "MPI_collectives.def"
    LAST_AND_UNUSED_MPI_COLLECTIVE_CODE
};
#undef DEFMPICOLLECTIVES

// Name of each MPI collective operations
#define DEFMPICOLLECTIVES(CODE, NAME) NAME,
const char* const mpi_collective_name[] = {
#include "MPI_collectives.def"
};
#undef DEFMPICOLLECTIVES

enum mpi_collective_code print_if_mpi_coll(const gimple* stmt)
{
    if (is_gimple_call(stmt)) {
        const tree t = gimple_call_fndecl(stmt);
        const char* ident = IDENTIFIER_POINTER(DECL_NAME(t));
        for (int i = 0; i < LAST_AND_UNUSED_MPI_COLLECTIVE_CODE; i++) {
            if (strcmp(mpi_collective_name[i], ident) == 0) {
                printf("\tMPI COLLECTIVE: '%s' (code: %d)\n", ident, i);
                return (enum mpi_collective_code)i;
            }
        }
    }

    return LAST_AND_UNUSED_MPI_COLLECTIVE_CODE;
}

// Return 1 if split, 0 if not.
bool split_bb_if_necessary(basic_block bb)
{
    int count = 0;
    for (gimple_stmt_iterator gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi)) {
        const gimple* stmt = gsi_stmt(gsi);

        if (is_gimple_call(stmt)) {
            const tree t = gimple_call_fndecl(stmt);
            const char* ident = IDENTIFIER_POINTER(DECL_NAME(t));

            for (int i = 0; i < LAST_AND_UNUSED_MPI_COLLECTIVE_CODE; i++) {
                count += (strcmp(mpi_collective_name[i], ident) == 0);

                if (count >= 2) {
                    gsi_prev(&gsi);
                    gimple* prev_stmt = gsi_stmt(gsi);
                    split_block(bb, prev_stmt);
                    return true;
                }
            }
        }
    }

    return false;
}

/* Build a filename (as a string) based on function name */
static char* cfgviz_generate_filename(function* fun, const char* suffix)
{
    char* target_filename;

    target_filename = (char*)xmalloc(2048 * sizeof(char));

    snprintf(target_filename, 1024, "%s_%s_%d_%s.dot", current_function_name(),
        LOCATION_FILE(fun->function_start_locus),
        LOCATION_LINE(fun->function_start_locus), suffix);

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

const pass_data my_pass_data = {
    .type = GIMPLE_PASS,
    .name = "TD3 Q8",
    .optinfo_flags = OPTGROUP_NONE,
    .tv_id = TV_OPTIMIZE,
    .properties_required = 0,
    .properties_provided = 0,
    .properties_destroyed = 0,
    .todo_flags_start = 0,
    .todo_flags_finish = 0,
};

class my_pass : public gimple_opt_pass {
public:
    my_pass(gcc::context* ctxt)
        : gimple_opt_pass(my_pass_data, ctxt)
    {
    }

    my_pass* clone()
    {
        return new my_pass(g);
    }

    bool gate(function*)
    {
        return true;
    }

    unsigned int execute(function* fun)
    {
        printf("Function: '%s'\n", function_name(fun));

        basic_block bb;
        FOR_EACH_BB_FN(bb, fun)
        {
            bb->aux = (void*)LAST_AND_UNUSED_MPI_COLLECTIVE_CODE;
            if (split_bb_if_necessary(bb)) {
                printf("\tSplit the block %02d\n", bb->index);
            }

            for (gimple_stmt_iterator gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi)) {
                const enum mpi_collective_code code = print_if_mpi_coll(gsi_stmt(gsi));
                if (code != LAST_AND_UNUSED_MPI_COLLECTIVE_CODE) {
                    bb->aux = (void*)code;
                }
            }
        }

        cfgviz_dump(fun, function_name(fun));

        clear_all_bb_aux(fun);

        return 0;
    }
};

int plugin_is_GPL_compatible = 1;

int plugin_init(struct plugin_name_args* plugin_info,
    struct plugin_gcc_version* version)
{
    struct register_pass_info my_pass_info;

    if (!plugin_default_version_check(version, &gcc_version))
        return 1;

    my_pass p(g);

    my_pass_info.pass = &p;
    my_pass_info.reference_pass_name = "cfg";
    my_pass_info.ref_pass_instance_number = 0;
    my_pass_info.pos_op = PASS_POS_INSERT_AFTER;

    register_callback(plugin_info->base_name,
        PLUGIN_PASS_MANAGER_SETUP,
        NULL,
        &my_pass_info);

    return 0;
}
