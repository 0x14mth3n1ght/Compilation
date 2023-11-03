#include <gcc-plugin.h>
#include <plugin-version.h>
#include <tree.h>
#include <basic-block.h>
#include <gimple.h>
#include <tree-pass.h>
#include <context.h>
#include <function.h>
#include <gimple-iterator.h>
#include <dominance.h>
#include <bitmap.h>                                   
#include <stack>                                                                                    

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


/***************** TD5 *******************/
/*****************************************/

bitmap_head* bitmap_init(){
  basic_block bb;
  bitmap_head *bitmap_to_initialize = XNEWVEC (bitmap_head, last_basic_block_for_fn (cfun));
  FOR_ALL_BB_FN (bb, cfun)
    bitmap_initialize (&bitmap_to_initialize[bb->index], &bitmap_default_obstack);
  return bitmap_to_initialize;
}

void mark_edge(basic_block bb)
{
        edge e;
        edge_iterator ei;

        ((bb_data *) bb->aux)->mark1 = 1;
        bitmap_set_bit(&((bb_data *) bb->aux)->seens, bb->index);
        FOR_EACH_EDGE(e, ei, bb->succs)
        {
                if (((edge_data *) e->aux)->loop)
                {
                        continue;
                }
                if (bitmap_bit_p(&((bb_data *) bb->aux)->seens, e->dest->index))
                {
                        ((edge_data *) e->aux)->loop = true;
                }
                if (! ((bb_data *) e->dest->aux)->mark1)
                {
                        bitmap_ior_into(&((bb_data *) e->dest->aux)->seens,
                                        &((bb_data *) bb->aux)->seens);
                        mark_edge(e->dest);
                }
        }
}

/***********************PLUGIN**********************/
/***************************************************/

int plugin_is_GPL_compatible;

const pass_data my_pass_data =
{
	GIMPLE_PASS, /* type */
	"NEW_PASS", /* name */
	OPTGROUP_NONE, /* optinfo_flags */
	TV_OPTIMIZE, /* tv_id */
	0, /* properties_required */
	0, /* properties_provided */
	0, /* properties_destroyed */
	0, /* todo_flags_start */
	0, /* todo_flags_finish */
};     

/* My new pass inheriting from regular gimple pass */
class my_pass : public gimple_opt_pass
{
	public:
		my_pass (gcc::context *ctxt)
			: gimple_opt_pass (my_pass_data, ctxt)
		{}

		my_pass *clone ()
		{
			return new my_pass(g);
		}

		bool gate (function *fun)
		{
			//function_isol_print(fun);
			printf("plugin: gate... \n");
			return true;
		}

		unsigned int execute (function *fun)
		{
			printf("plugin: execute...\n");
			printf("Function: '%s'\n", function_name(fun));

			basic_block bb = NULL;
            split_on_mpi_collectives(bb,fun);
			calculate_dominance_info(CDI_POST_DOMINATORS);

            //bitmap_head *all_preds = bitmap_init();
            bb = ENTRY_BLOCK_PTR_FOR_FN(fun)->next_bb;
    		mark_edge(bb);

			/******************************/
			/********** FIN TD5 ***********/
			/******************************/

			free_dominance_info(CDI_POST_DOMINATORS);
			clear_all_bb_aux(fun);
			
			return 0;
		}
}; 

/* Main entry point for plugin */
int plugin_init(struct plugin_name_args * plugin_info, struct plugin_gcc_version * version)
{
	struct register_pass_info my_pass_info;
    printf( "plugin_init: Entering...\n" ) ;

	if(!plugin_default_version_check(version, &gcc_version)) 
		return 1;

	printf( "plugin_init: Check ok...\n" ) ;
	my_pass p(g);

	/* Fill info on my pass 
	 (insertion after the pass building the CFG) */
	my_pass_info.pass = &p;
	my_pass_info.reference_pass_name = "cfg";
	my_pass_info.ref_pass_instance_number = 0;
	my_pass_info.pos_op = PASS_POS_INSERT_AFTER;

	/* Add my pass to the pass manager */
	register_callback(plugin_info->base_name, 
			PLUGIN_PASS_MANAGER_SETUP, 
			NULL, 
			&my_pass_info);

	printf( "plugin_init: Pass added...\n" ) ;

	return 0;
}


