#include <gcc-plugin.h>
#include <tree.h>
#include <tree-pass.h>
#include <context.h>
#include <function.h>

//Q2.2 remplir la pass appelée par plugin_init
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

//Q2.3: a) my_pass hérite de opt_pass
class my_pass : public gimple_opt_pass {
    public:
        my_pass (gcc::context *ctxt)
            :gimple_opt_pass (my_pass_data, ctxt)
        {}

        my_pass *clone () { return new my_pass(g); }

        //Q2.3 b) implémentation gate & execute
        bool gate (function *fun) { return true; }

        unsigned int execute (function *fun) {
            printf("Executing my_pass with function %s\n", function_name(fun) );
            return 0;
        }
};


int plugin_is_GPL_compatible;

int plugin_init(struct plugin_name_args * plugin_info,struct plugin_gcc_version * version){
    //Q2.1) définition de la passe
    struct register_pass_info pass_info;
    my_pass p(g);
    pass_info.pass = &p;
    pass_info.reference_pass_name = "omplower";
    pass_info.ref_pass_instance_number = 0;
    pass_info.pos_op = PASS_POS_INSERT_BEFORE;
    //Q2.4) enregistrement de la passe
    register_callback(plugin_info->base_name,PLUGIN_PASS_MANAGER_SETUP,NULL,&pass_info);
    return 0;
}