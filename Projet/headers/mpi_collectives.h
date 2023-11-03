#ifndef MPI_COLLECTIVES
#define MPI_COLLECTIVES

// Codes and functions to detect and do operations based on the presence of
// MPI collectives in basic blocks.
#pragma once

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

enum domination_status
{
        NOT_DOMINATED,
        DOMINATED,
        NOT_APPLICABLE
};

struct bb_data
{
        mpi_collective_code mpi_code;
        bitmap_head dom;
        bitmap_head post_dom;
        bitmap_head dom_front;
        bitmap_head post_dom_front;
        int collective_rank[LAST_AND_UNUSED_MPI_COLLECTIVE_CODE];
        bitmap_head seens;
        int mark1; // for graph parkour
        int mark2;
        domination_status dom_status;
};

struct edge_data
{
        // exclude adge that make loop
        // excluding edge tag with loop remove all loop from the graph
        // used to calculate collectives ranks
        bool loop;
};


// Split the basic blocks of the function if they contains at least two
// MPI collectives.

bool split_bb_if_necessary(basic_block bb)

void split_on_mpi_collectives(basic_block bb, function* fun);

#endif MPI_COLLECTIVES
