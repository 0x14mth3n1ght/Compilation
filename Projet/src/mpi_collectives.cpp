#include "../headers/mpi_collectives.h"

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

void split_on_mpi_collectives(basic_block bb, function *fun)
{
	FOR_EACH_BB_FN(bb, fun)
	{   
		bb->aux = (void*)LAST_AND_UNUSED_MPI_COLLECTIVE_CODE;
		if (split_bb_if_necessary(bb))
        	printf("\tSplit the block %02d\n", bb->index);

		for (gimple_stmt_iterator gsi = gsi_start_bb(bb); !gsi_end_p(gsi); gsi_next(&gsi)) {
			const enum mpi_collective_code code = print_if_mpi_coll(gsi_stmt(gsi));
			if (code != LAST_AND_UNUSED_MPI_COLLECTIVE_CODE)
				bb->aux = (void*)code;
		}
	} 
}
