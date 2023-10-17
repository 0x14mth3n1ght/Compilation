#include <gcc-plugin.h>
#include <plugin-version.h>
#include <tree.h>
#include <basic-block.h>
#include <gimple.h>
#include <tree-pass.h>
#include <context.h>
#include <function.h>
#include <gimple-iterator.h>

/* Global variable required for plugin to execute */
int plugin_is_GPL_compatible;

/* Global object (const) to represent my pass */
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

//TD3
/* Enum to represent the collective operations */
enum mpi_collective_code {
#define DEFMPICOLLECTIVES( CODE, NAME ) CODE,
#include "MPI_collectives.def"
        LAST_AND_UNUSED_MPI_COLLECTIVE_CODE
#undef DEFMPICOLLECTIVES
} ;

/* Name of each MPI collective operations */
#define DEFMPICOLLECTIVES( CODE, NAME ) NAME,
const char *const mpi_collective_name[] = {
#include "MPI_collectives.def"
} ;
#undef DEFMPICOLLECTIVES

const char *print_func_name(function * fun)
{
	const char * fname = function_name(fun);
	printf("\t ... in function %s\n", fname);
	return fname;
}

//TD4
//retourne une liste dom [dom0,dom1,...,dom13]
std::vector<std::vector<int>> list_get_dominators(function * fun)
{
    std::vector<std::vector<int>> dominators_list;

    basic_block bb;
    calculate_dominance_info(CDI_DOMINATORS);
    FOR_ALL_BB_FN(bb, fun)
    {
        auto_vec<basic_block> dominators;
        dominators = get_all_dominated_blocks(CDI_DOMINATORS, bb);
        std::vector<int> dominators_indices;

        for (int i = 0; i < dominators.length(); i++)
        {
            if (dominators[i]->index != bb->index)
            {
                dominators_indices.push_back(dominators[i]->index);
            }
        }
        dominators_list.push_back(dominators_indices);
    }
    return dominators_list;
}



/***************** TD5 *******************/
/*****************************************/

void td5_q1_frontier(function *fun){
    edge_iterator eit;
    edge e;
    struct basic_bloc_elem *cur_elem;
    std::vector<std::vector<int>> dominators = list_get_dominators(fun);
    
    bool changed = 1;
    while (changed){
        changed = 0;

        FOR_EACH_EDGE(e, eit, cur_elem->bb->succs){

        }
    }
}

void td5_q2_dfs_cfg2(function *fun)
{                                         
    vec <struct basic_bloc_elem *> pile;
    struct basic_bloc_elem *first;

    first = new_bb_elem();
    first->bb = ENTRY_BLOCK_PTR_FOR_FN(fun);

    pile.push(first);

    while (pile.length() != 0) 
    {
        edge_iterator eit;
        edge e;
        int edge_index;
        struct basic_bloc_elem *cur_elem;
        cur_elem = pile.pop();

        bitmap_set_bit(&cur_elem->already_seen, cur_elem->bb->index);

        edge_index = 0;
        FOR_EACH_EDGE(e, eit, cur_elem->bb->succs)
        {
            /* Already seen */
            if (bitmap_bit_p(&cur->already_seen, e->dest->index));
                    continue;

            bitmap_set_bit(&cur_elem->bb->aux->edges, edge_index);

            edge_index++;
            next elem = new_bb_elem();
        }
    }
}

/***********************PLUGIN**********************/
/***************************************************/
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
			print_func_name(fun);
			return true;
		}

		unsigned int execute (function *fun)
		{
			printf("plugin: execute...\n");

			//td_isol_print(/*TD*/2);
			//td2_through_the_cfg(fun);
			//cfgviz_dump( fun, "0_ini", /*TD*/2 ) ;

			//td_isol_print(/*TD*/3);
			//clean_aux_field(fun, LAST_AND_UNUSED_MPI_COLLECTIVE_CODE);
			//td3_mpi_in_blocks(fun);
			//td_isol_print(/*TD*/4);
			//td4_dom_and_postdom(fun);

			//td_isol_print(/*TD*/5);
			//td5_bitmap_and_pdf_it(fun);
            td5_q1_frontier(fun);
            //td5_q2_dfs_cfg2(fun);

			/******************************/
			/********** FIN TD5 ***********/
			/******************************/

			//clean_aux_field(fun, 0);

			//free_dominance_info(CDI_POST_DOMINATORS);
			//free_dominance_info(CDI_DOMINATORS);

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


