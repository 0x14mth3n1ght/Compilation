# TD1

## Partie 1

```
wget https://ftp.gnu.org/gnu/gcc/gcc-12.2.0/gcc-12.2.0.tar.gz
tar -xvf gcc-12.2.0.tar.gz
mkdir gcc-12.2.0/build; cd gcc-12.2.0/build 
../configure --prefix=/home/night/gcc-12.2.0 
--enable-languages=c,c++,fortran --enable-plugin --disable-bootstrap --disable-multilib
make install
~/gcc-12.2.0/bin/gcc
```

```
┌─[✗]─[night@night-20b7s2ex01]─[~/gcc12.2.0]
└──╼ 6 fichiers, 24Kb)─$ ./bin/gcc -v
Utilisation des specs internes.
COLLECT_GCC=./bin/gcc
COLLECT_LTO_WRAPPER=/home/night/gcc12.2.0/libexec/gcc/x86_64-pc-linux-gnu/12.2.0/lto-wrapper
Cible : x86_64-pc-linux-gnu
Configuré avec: ../configure --prefix=/home/night/gcc12.2.0 --enable-languages=c,c++,fortran --enable-plugin --disable-bootstrap --disable-multilib
Modèle de thread: posix
Algorithmes de compression LTO supportés: zlib zstd
gcc version 12.2.0 (GCC) 
```

### Q1

```bash
gcc -o bin main.c vector.c compute.c
```

1+2 = 0?

### Q2

```bash
gcc vector.c -c -Wall -O3
gcc compute.c -c -Wall -O3
Dans le fichier inclus depuis compute.c:9:
timer.h: Dans la fonction « usecs »:
timer.h:14:9: attention: déclaration implicite de la fonction « gettimeofday » [-Wimplicit-function-declaration]
   14 |         gettimeofday(&t,NULL);
      |         ^~~~~~~~~~~~
compute.c: Dans la fonction « print_results »:
compute.c:30:42: attention: format « %g » attend un argument de type « double » mais l'argument 4 a le type « unsigned int » [-Wformat=]
   30 |                 printf("\t%d\t \t%d\t \t%g\n",
      |                                         ~^
      |                                          |
      |                                          double
      |                                         %d
   31 |                                 v1[i], v2[i], v3[i]);
      |                                               ~~~~~
      |                                                 |
      |                                                 unsigned int
gcc main.c vector.o compute.o -o bin -Wall -O3
main.c: Dans la fonction « main »:
main.c:31:15: attention: variable inutilisée « useless » [-Wunused-variable]
   31 |         float useless = 0;
```

-> remplacer %g par %d dans compute.c: 1+2= 3

### Q3

~ 7.6s

### Q4

~0.9s avec -O3

### Q5

```bash
gcc -o libvector.so -shared -fPIC  vector.c
~/gcc12.2.0/bin/gcc -shared -o libvector.so *o
~/gcc12.2.0/bin/gcc -o bin main.c -L. -l:libvector.s
export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH
ldd ./bin
./bin
```

### Q6

```bash
~/gcc12.2.0/bin/gcc -c -fpic vector.c compute.c
```

## Partie 2

### Q1-2

```bash
CC=gcc
FLAGS=-Wall -O3

all:
        $(CC) vector.c -c $(FLAGS)
        $(CC) compute.c -c $(FLAGS)
        $(CC) main.c vector.o compute.o -o bin $(FLAGS)

clean:
        rm -rf *o *bin
```

## Partie 3

### Q1

```bash
gcc -g main
gdb a.out
```

### Q2
-> erreur sur memset nb dans floor_mean
```bash
memset(&nb,nb,sizeof(unsigned int)
```

### Q3
```bash
unsigned int factorial(unsigned int val)
```
-> if val <=1
-> commenter factorial(-1)

### Q4
```bash
gdb -q ./main
b launch_fibonacci
r
```

```bash
          // fibo_values=0x007fffffffe218  →  0x0000000000000000, max=0x6
 →   57         fibo_values->max = max;
     58         fibo_values->n_minus_1 = 1;
     59         fibo_values->n_minus_2 = 2;
     60         fibonacci(fibo_values, max);
     61  }
     62  
─────────────────────────────────────────────────────────────────────────────────── threads ────
[#0] Id 1, Name: "main", stopped 0x401253 in launch_fibonacci (), reason: BREAKPOINT
───────────────────────────────────────────────────────────────────────────────────── trace ────
[#0] 0x401253 → launch_fibonacci(fibo_values=0x0, max=0x6)
[#1] 0x4013d9 → main(argc=0x1, argv=0x7fffffffe388)
gef➤  print fibo_values  -> max
Cannot access memory at address 0xc
gef➤  print fibo_values
$1 = (Fibo *) 0x0
```

`up: se place avant call fibo`

```bash
gef➤  up
#1  0x00000000004013d9 in main (argc=0x1, argv=0x7fffffffe388) at main.c:105
105             launch_fibonacci(fibo_values, n);
gef➤  list
100
101
102             // Exercice 4 & 5: fibonacci
103             Fibo *fibo_values = NULL;
104       int n = 6;
105             launch_fibonacci(fibo_values, n);
106             printf("4) fibonacci value F%d = %d\n", n, fibo_values->result);
107
108             return 0;
109     }
gef➤  print fibo_values 
$2 = (Fibo *) 0x0
```

**Correction**:

`Fibo *fibo_values = malloc(sizeof(Fibo));`

```bash
┌─[night@night-20b7s2ex01]─[~/S5/CA_2023/TDs/TD1/CODE/3-BUGS]
└──╼ 2 fichiers, 20Kb)─$ ./main 
******************************************************
          ENSIIE - module CA - TD1: part3 GDB         
          mean floored - factorial - fibonacci        
******************************************************
1) mean value = 0
2) factorial value = 24
3) Another factorial value = 24
4) fibonacci value F6 = 18
```

### Q5

```bash
     77  int main (int argc, char **argv) {
     78         unsigned int i, value;
 →   79         print_info();
     80  
     81  
     82         // Exercice 1: mean (florred) of 100 values
     83         unsigned int *list = malloc(sizeof(unsigned int)*100);
     84         for (i=0; i<100; i++) {
─────────────────────────────────────────────────────────────────────────────────── threads ────
[#0] Id 1, Name: "main", stopped 0x401305 in main (), reason: BREAKPOINT
───────────────────────────────────────────────────────────────────────────────────── trace ────
[#0] 0x401305 → main(argc=0x1, argv=0x7fffffffe388)
────────────────────────────────────────────────────────────────────────────────────────────────
gef➤  watch fibo_values->result
Hardware watchpoint 2: fibo_values->result

```

## Partie IV - Passes

### Q1

```bash
gcc -o main -fdump-tree-all main.c
ls
```

### Q2

```bash
┌─[night@night-20b7s2ex01]─[~/S5/CA_2023/TDs/TD1/CODE/4-SAXPY]
└──╼ 30 fichiers, 116Kb)─$ ls
main                    main.c.021t.ssa               main.c.101t.adjust_alignment
main.c                  main.c.022t.walloca1          main.c.238t.veclower
main.c.005t.original    main.c.025t.waccess1          main.c.239t.cplxlower0
main.c.006t.gimple      main.c.029t.fixup_cfg2        main.c.241t.switchlower_O0
main.c.009t.omplower    main.c.030t.local-fnsummary1  main.c.248t.isel
main.c.010t.lower       main.c.031t.einline           main.c.251t.waccess3
main.c.013t.eh          main.c.049t.profile_estimate  main.c.252t.optimized
main.c.015t.cfg         main.c.053t.release_ssa       main.c.339t.statistics
main.c.017t.ompexp      main.c.054t.local-fnsummary2  main.c.340t.earlydebug
main.c.020t.fixup_cfg1  main.c.094t.fixup_cfg3        main.c.341t.debug
┌─[night@night-20b7s2ex01]─[~/S5/CA_2023/TDs/TD1/CODE/4-SAXPY]
└──╼ 30 fichiers, 116Kb)─$ ls | wc -l
30
```

### Q3

```bash
gcc -o main -fdump-tree-all -O1 main
```

Il ya encore plus de fichiers (110)

### Q4

```bash
─[night@night-20b7s2ex01]─[~/gcc12.2.0]
└──╼ 6 fichiers, 24Kb)─$ ls
bin  include  lib  lib64  libexec  share
┌─[night@night-20b7s2ex01]─[~/gcc12.2.0]
└──╼ 6 fichiers, 24Kb)─$ find . -name "passes.def"
./lib/gcc/x86_64-pc-linux-gnu/12.2.0/plugin/include/passes.def
```

```bash
/*
 Macros that should be defined when using this file:
   INSERT_PASSES_AFTER (PASS)
   PUSH_INSERT_PASSES_WITHIN (PASS)
   POP_INSERT_PASSES ()
   NEXT_PASS (PASS)
   TERMINATE_PASS_LIST (PASS)
 */

 /* All passes needed to lower the function into shape optimizers can
    operate on.  These passes are always run first on the function, but
    backend might produce already lowered functions that are not processed
    by these passes.  */
  INSERT_PASSES_AFTER (all_lowering_passes)
  NEXT_PASS (pass_warn_unused_result);
  NEXT_PASS (pass_diagnose_omp_blocks);
  NEXT_PASS (pass_diagnose_tm_blocks);
  NEXT_PASS (pass_omp_oacc_kernels_decompose);
  NEXT_PASS (pass_lower_omp);
  NEXT_PASS (pass_lower_cf);
```

Il contient des passes, des instructions POP_INSERT,PUSH_INSERT
qui sont utilisées par les fichiers générés par `-fdump-tree-all`

Ce sont des représentations intermédiaires du code compilé.

ex: `main.c.006t.gimple`

```bash
void saxpy (int * res, int *x, int *y int a ,int size)
{

	int i;
	i = 0;
	goto <D.2917>:
	<D.2916>:
	_1 = (long unsigned int) i;
	...
	i = i+1
	<D.2917>
	if (i<size) goto <D.2916>; else goto <D.2914>:
	<D.2914>
}

__attribute__((access ("^1[ ]", )))
int maint (int argc, char **argv)
{
...
}
```

### Q5

- `lower__complex__O0`

```bash
┌─[✗]─[night@night-20b7s2ex01]─[~/S5/CA_2023/gcc-12.2.0]
└──╼ 79 fichiers, 15Mb)─$ grep -r "lower_complex_O0"
gcc/tree-pass.h:extern gimple_opt_pass *make_pass_lower_complex_O0 (gcc::context *ctxt);
```

-> dans `tree-complex.cc`

```bash
namespace {

const pass_data pass_data_lower_complex_O0 =
{
  GIMPLE_PASS, /* type */
  "cplxlower0", /* name */
  OPTGROUP_NONE, /* optinfo_flags */
  TV_NONE, /* tv_id */
  PROP_cfg, /* properties_required */
  PROP_gimple_lcx, /* properties_provided */
  0, /* properties_destroyed */
  0, /* todo_flags_start */
  TODO_update_ssa, /* todo_flags_finish */
};

class pass_lower_complex_O0 : public gimple_opt_pass
{
public:
  pass_lower_complex_O0 (gcc::context *ctxt)
    : gimple_opt_pass (pass_data_lower_complex_O0, ctxt)
  {}

  /* opt_pass methods: */
  virtual bool gate (function *fun)
    {
      /* With errors, normal optimization passes are not run.  If we don't
	 lower complex operations at all, rtl expansion will abort.  */
      return !(fun->curr_properties & PROP_gimple_lcx);
    }

  virtual unsigned int execute (function *) { return tree_lower_complex (); }

}; // class pass_lower_complex_O0
```

- `lower__vector`

```
─[night@night-20b7s2ex01]─[~/S5/CA_2023/gcc-12.2.0]
└──╼ 79 fichiers, 15Mb)─$ grep -r "lower_vector"
gcc/tree-pass.h:extern gimple_opt_pass *make_pass_lower_vector (gcc::context *ctxt);
```

-> dans `gcc/tree-vect-generic.cc`

```bash
namespace {

const pass_data pass_data_lower_vector =
{
  GIMPLE_PASS, /* type */
  "veclower", /* name */
  OPTGROUP_VEC, /* optinfo_flags */
  TV_NONE, /* tv_id */
  PROP_cfg, /* properties_required */
  PROP_gimple_lvec, /* properties_provided */
  0, /* properties_destroyed */
  0, /* todo_flags_start */
  TODO_update_ssa, /* todo_flags_finish */
};

class pass_lower_vector : public gimple_opt_pass
{
public:
  pass_lower_vector (gcc::context *ctxt)
    : gimple_opt_pass (pass_data_lower_vector, ctxt)
  {}

  /* opt_pass methods: */
  virtual bool gate (function *fun)
    {
      return !(fun->curr_properties & PROP_gimple_lvec);
    }

  virtual unsigned int execute (function *)
    {
      return expand_vector_operations ();
    }

}; // class pass_lower_vector

} // anon namespace

gimple_opt_pass *
make_pass_lower_vector (gcc::context *ctxt)
{
  return new pass_lower_vector (ctxt);
}
```
