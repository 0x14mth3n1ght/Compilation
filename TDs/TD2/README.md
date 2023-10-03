# TD2

## Q1

```bash
┌─[night@night-20b7s2ex01]─[~/gcc12]
└──╼ 6 fichiers, 24Kb)─$ find . -name "gcc-plugin.h"
./lib/gcc/x86_64-pc-linux-gnu/12.2.0/plugin/include/gcc-plugin.h
```

```bash
┌─[night@night-20b7s2ex01]─[~/gcc12]
└──╼ 6 fichiers, 24Kb)─$ cat ./lib/gcc/x86_64-pc-linux-gnu/12.2.0/plugin/include/gcc-plugin.h
/* Public header file for plugins to include.
   Copyright (C) 2009-2022 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#ifndef GCC_PLUGIN_H
#define GCC_PLUGIN_H

#ifndef IN_GCC
#define IN_GCC
#endif

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "backend.h"
#include "cfghooks.h"
#include "hard-reg-set.h"
#include "cfgrtl.h"
#include "cfganal.h"
#include "lcm.h"
#include "cfgbuild.h"
#include "cfgcleanup.h"
#include "plugin-api.h"
#include "ipa-ref.h"
#include "alias.h"
#include "flags.h"
#include "tree-core.h"
#include "fold-const.h"
#include "tree-check.h"
#include "plugin.h"

#endif /* GCC_PLUGIN_H */
```

**exemple cours**

```bash
#include <gcc-plugin.h>

int plugin_is_GPL_compatible;

int plugin_init(struct plugin_name_args * plugin_info, struct plugin_gcc_version * version){
    printf( "Plugin initialization:\n" ) ;
    printf( "\tbasever = %s\n", version->basever ) ;
    printf( "\tdatestamp = %s\n", version->datestamp ) ;
    printf( "\tdevphase = %s\n", version->devphase ) ;
    printf( "\trevision = %s\n", version->revision ) ;
    printf( "\tconfig = %s\n", version->configuration_arguments ) ;
    return 0;
}
```

```bash
g++ -I`gcc -print-file-name=plugin`/include -g -Wall -fno-rtti -shared -fPIC -o libplugin.so plugin.cpp
```

```bash
gcc -fplugin=./libplugin.so -c test.c
```

En `makefile`:

```bash
EXE= TP2_1 \
	TP2_2 \
	TP2_3 \
	TP2_5 \
	TP2_6 \
	TP2_7 \
	TP2_7_bis \
	TP2_8 

all: $(EXE)

CXX=~/gcc12/bin/g++
CC=~/gcc12/bin/gcc

MPICC=mpicc

PLUGIN_FLAGS=-I`$(CC) -print-file-name=plugin`/include -g -Wall -fno-rtti -shared -fPIC

CFLAGS=-g -O3


libplugin_%.so: plugin_%.cpp
	$(CXX) $(PLUGIN_FLAGS) $(GMP_CFLAGS) -o $@ $<

% : libplugin_%.so test.c
	OMPICC_MPICC=$(CC) $(MPICC) test.c $(CFLAGS) -o $@ -fplugin=./$< 

clean:
	rm -rf $(EXE)

clean_all: clean
	rm -rf libplugin*.so *.dot
```

```bash
┌─[night@night-20b7s2ex01]─[~/S5/CA_2023/TDs/TD2/CODE]
└──╼ 7 fichiers, 128Kb)─$ make
~/gcc12/bin/g++ -I`~/gcc12/bin/gcc -print-file-name=plugin`/include -g -Wall -fno-rtti -shared -fPIC  -o libplugin_TP2_1.so plugin_TP2_1.cpp
OMPI_MPICC=~/gcc12/bin/gcc mpicc test.c -g -O3 -o TP2_1 -fplugin=./libplugin_TP2_1.so 
Plugin initialization:
        basever = 12.2.0
        datestamp = 20220819
        devphase = 
        revision = 
        config = ../configure --prefix=/home/night/gcc12 --enable-languages=c,c++ --enable-plugin --disable-bootstrap --disable-multilib
```

## Q2

```bash
┌─[night@night-20b7s2ex01]─[~/gcc12]
└──╼ 6 fichiers, 24Kb)─$ grep -r pass_data
grep: libexec/gcc/x86_64-pc-linux-gnu/12.2.0/cc1 : fichiers binaires correspondent
grep: libexec/gcc/x86_64-pc-linux-gnu/12.2.0/lto1 : fichiers binaires correspondent
grep: libexec/gcc/x86_64-pc-linux-gnu/12.2.0/cc1plus : fichiers binaires correspondent
lib/gcc/x86_64-pc-linux-gnu/12.2.0/plugin/include/tree-pass.h:struct pass_data
```

Voir `plugin_TP2_2.cpp`


```bash
┌─[✗]─[night@night-20b7s2ex01]─[~/S5/CA_2023/TDs/TD2/CODE]
└──╼ 9 fichiers, 152Kb)─$ make
~/gcc12/bin/g++ -I`~/gcc12/bin/gcc -print-file-name=plugin`/include -g -Wall -fno-rtti -shared -fPIC  -o libplugin_TP2_2.so plugin_TP2_2.cpp
Dans le fichier inclus depuis plugin_TP2_2.cpp:2:
/home/night/gcc12/lib/gcc/x86_64-pc-linux-gnu/12.2.0/plugin/include/plugin-version.h:16:34: attention: « gcc_version » défini mais pas utilisé [-Wunused-variable]
   16 | static struct plugin_gcc_version gcc_version = {basever, datestamp,
      |                                  ^~~~~~~~~~~
OMPI_MPICC=~/gcc12/bin/gcc mpicc test.c -g -O3 -o TP2_2 -fplugin=./libplugin_TP2_2.so 
Executing my_pass with function mpi_call
Executing my_pass with function main
```

`execute` est éxécutée pour mpi_call et main (parallèle)

## Q3

```bash
┌─[night@night-20b7s2ex01]─[~/gcc12]
└──╼ 6 fichiers, 24Kb)─$ find . -name "function.h"
./lib/gcc/x86_64-pc-linux-gnu/12.2.0/plugin/include/function.h
```

Dans `function.h`

```c++
/* Returns the name of the current function.  */
extern const char *fndecl_name (tree);
extern const char *function_name (struct function *);
```

On implémente

```c++
const char *print_func_name(function *fun){
    const char *fname = function_name(fun);
    printf("in function %s\n",fname);
}
```

Dans `class my_pass: public gimple_opt_pass`

```c++
bool gate (function *fun) {
        print_func_name(fun);
        return true;
}
```

(l'appel dans gate **ou** execute fonctionne)

gate appelée avant l'éxécution de la passe

```bash
make
```

```bash
OMPI_MPICC=~/gcc12/bin/gcc mpicc test.c -g -O3 -o TP2_3 -fplugin=./libplugin_TP2_3.so
in function mpi_call
Executing my_pass with function mpi_call
in function main
Executing my_pass with function main
```

## Q4

Toujours dans `function.h`

```c++
extern const char *current_function_name (void);

extern void used_types_insert (tree);

#endif  /* GCC_FUNCTION_H */
```

On rajoute dans notre plugin:

```c++
const char *print_func_name2(){
    const char *fname = current_function_name();
    printf("in function %s (appel current_function_name)\n",fname);
    return fname;
}
```

Dans gate ou execute:

```c++
bool gate (function *fun) {
    print_func_name(fun);
    print_func_name2();
    return true;
}
```

```bash
make
```

```bash
Executing my_pass with function mpi_call
Executing my_pass with function main
~/gcc12/bin/g++ -I`~/gcc12/bin/gcc -print-file-name=plugin`/include -g -Wall -fno-rtti -shared -fPIC  -o libplugin_TP2_3.so plugin_TP2_3.cpp
OMPI_MPICC=~/gcc12/bin/gcc mpicc test.c -g -O3 -o TP2_3 -fplugin=./libplugin_TP2_3.so 
in function mpi_call (appel function_name)
in function mpi_call (appel current_function_name)
Executing my_pass with function mpi_call
in function mpi_call (appel function_name)
in function main (appel function_name)
in function main (appel current_function_name)
Executing my_pass with function main
in function main (appel function_name)
```
## Q5

```c++
void print_blocs_index(function *fun){
    basic_block bb;
    gimple_stmt_iterator gsi;
    gimple *stmt;

    FOR_EACH_BB_FN(bb,fun){
        gsi = gsi_start_bb(bb);
        stmt = gsi_stmt(gsi);
        printf("Block index %d : line %d\n",bb->index,gimple_lineno(stmt));
    }
}
```
