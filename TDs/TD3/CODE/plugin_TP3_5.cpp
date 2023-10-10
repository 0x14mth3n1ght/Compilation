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

const pass_data my_pass_data = {
    .type = GIMPLE_PASS,
    .name = "TD3 Q3",
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
            for (gimple_stmt_iterator gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi)) {
                const enum mpi_collective_code code = print_if_mpi_coll(gsi_stmt(gsi));
                if (code != LAST_AND_UNUSED_MPI_COLLECTIVE_CODE) {
                    printf("\tBB index: %d\n", bb->index);
                    bb->aux = (void*)code;
                }
            }
        }

        FOR_EACH_BB_FN(bb, fun)
        {
            bb->aux = (void*)NULL;
        }

        FOR_EACH_BB_FN(bb, fun)
        {
            printf("BB index: %d\n", bb->index);
            printf("BB aux:   %ld\n", (unsigned long)(bb->aux));
        }

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
