#include <gcc-plugin.h>                                                          
#include <plugin-version.h>                                                      
#include <tree.h>                                                                
#include <basic-block.h>                                                         
#include <gimple.h>                                                              
#include <tree-pass.h>                                                           
#include <context.h>                                                             
#include <function.h>                                                            
#include <gimple-iterator.h>                                                     


/******************************/
/****   TD2 - QUESTION 7   ****/
/******************************/

/* Build a filename (as a string) based on function name */
static char * cfgviz_generate_filename( function * fun, const char * suffix ){
        char * target_filename; 

        target_filename = (char *)xmalloc( 2048 * sizeof( char ) );

        snprintf( target_filename, 1024, "%s_%s_%d_%s.dot", 
                        current_function_name(),
                        LOCATION_FILE( fun->function_start_locus ),
                        LOCATION_LINE( fun->function_start_locus ),
                        suffix );

        return target_filename;
}

/* Dump the graphviz representation of function 'fun' in file 'out' */
static void cfgviz_internal_dump(function *fun, FILE *out){

        // Print the header line and open the main graph
        fprintf(out, "Digraph G{\n");

        /*****************************/
        /***** COMPLETE HERE ********/
        /*****************************/

	basic_block bb;

	FOR_ALL_BB_FN(bb,fun) {
		fprintf(out, "%d [label=\"BB %d\" shape=ellipse]\n", bb->index, bb->index);

	        edge e;
	        edge_iterator eiterator;

	        FOR_EACH_EDGE(e, eiterator, bb->succs) {
                        const char* edgeLabel = "";

	                if (e->flags == EDGE_TRUE_VALUE) {
		                edgeLabel = "true";
		        } 
		        else if (e->flags == EDGE_FALSE_VALUE) {
			        edgeLabel = "false";
			}

		        fprintf(out, "%d -> %d [color=red label=\"%s\"]\n", bb->index, e->dest->index, edgeLabel);
	        }

	}
	// Close the main graph
	fprintf(out, "}\n");
}

void cfgviz_dump(function * fun, const char * suffix){
        char* target_filename; 
        FILE* out;

        target_filename = cfgviz_generate_filename(fun, suffix) ;

        printf("[GRAPHVIZ] Generating CFG of function %s in file <%s>\n", current_function_name(), target_filename);

        out = fopen(target_filename,"w");

        cfgviz_internal_dump(fun, out);

        fclose(out);
        free(target_filename);
}

/******************************/
/**   TD2 - FIN QUESTION 7   **/
/******************************/

const pass_data my_pass_data = {
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

class my_pass : public gimple_opt_pass {
    public:
        my_pass (gcc::context *ctxt)
            :gimple_opt_pass (my_pass_data, ctxt)
        {}

        my_pass *clone () { return new my_pass(g); }

        bool gate (function *fun) { 
            return true; }

        unsigned int execute (function *fun) {
            printf("Executing my_pass with function %s\n", function_name(fun) );
            //char* suffix = "viz";
            cfgviz_dump(fun,"viz");
            return 0;
        }
};

int plugin_is_GPL_compatible;

int plugin_init(struct plugin_name_args * plugin_info,struct plugin_gcc_version * version){
    struct register_pass_info pass_info;
    my_pass p(g);
    pass_info.pass = &p;
    pass_info.reference_pass_name = "cfg";
    pass_info.ref_pass_instance_number = 0;
    pass_info.pos_op = PASS_POS_INSERT_AFTER;
    register_callback(plugin_info->base_name,PLUGIN_PASS_MANAGER_SETUP,NULL,&pass_info);
    return 0;
}
