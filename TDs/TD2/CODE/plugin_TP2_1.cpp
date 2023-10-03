#include <gcc-plugin.h>

int plugin_is_GPL_compatible;

int plugin_init(struct plugin_name_args * plugin_info,struct plugin_gcc_version * version){
    printf( "Plugin initialization:\n" ) ;
    printf( "\tbasever = %s\n", version->basever ) ;    
    printf( "\tdatestamp = %s\n", version->datestamp ) ;
    printf( "\tdevphase = %s\n", version->devphase ) ;
    printf( "\trevision = %s\n", version->revision ) ;
    printf( "\tconfig = %s\n", version->configuration_arguments ) ;
    return 0;
}
