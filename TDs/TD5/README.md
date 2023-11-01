## Rappel - jusqu'au TD4

`plugin_TP5_0.cpp` -> correction `split_on_mpi_collectives` dans plugin q1.

Le *split* permet de modifier le CFG pour séparer les blocs contenant plusieurs appels MPI : en ajoutant le bloc 13 (cf **TD3 q8**)

![0](CODE/main_test3.c_8_main_q7.png)

## Q1

(Chercher dans les sources **et pas dans l'installation** ...)

```bash
┌─[night@night-20b7s2ex01]─[~/S5/gcc-12.2.0]
└──╼ 79 fichiers, 15Mb)─$ find . -name "bitmap.h"
./gcc/bitmap.h
```

`bitmap.h`

```c
/*
Often the most suitable representation during construction of the set
is not the best choice for the usage of the set.  For such cases, the
"view" of the set can be changed from one representation to the other.
This is an O(E) operation:

     * from list to tree view	: bitmap_tree_view
     * from tree to list view	: bitmap_list_view

*/
static inline void
bitmap_initialize (bitmap head, bitmap_obstack *obstack CXX_MEM_STAT_INFO)
{
  head->first = head->current = NULL;
  head->indx = head->tree_form = 0;
  head->padding = 0;
  head->alloc_descriptor = 0;
  head->obstack = obstack;
  if (GATHER_STATISTICS)
    bitmap_register (head PASS_MEM_STAT);
}
```

Comme indiqué dans l'énoncé on recherche `df_md_local_compute` car l'algo calculant la frontière de post dominance avec les bitmaps est dedans:

```bash
┌─[night@night-20b7s2ex01]─[~/S5/gcc-12.2.0]
└──╼ 79 fichiers, 15Mb)─$ grep -r df_md_local_compute
gcc/ChangeLog-2018:     (df_md_local_compute): Likewise.
gcc/ChangeLog-2010:     * df-problems.c (df_md_local_compute): Likewise.
gcc/ChangeLog-2010:     df_md_bb_local_compute_process_def, df_md_local_compute,
gcc/ChangeLog-2010:     df_md_bb_local_compute_process_def, df_md_local_compute, df_md_reset,
gcc/ChangeLog-2013:     df_live_verify_solution_start, df_md_local_compute): Likewise.
gcc/ChangeLog-2013:     df_md_bb_local_compute, df_md_local_compute,
gcc/ChangeLog-2009:     (df_md_local_compute): Only include live registers in init.
gcc/ChangeLog-2009:     (df_md_local_compute): Only include live registers in init.
gcc/ChangeLog-2009:     (df_rd_local_compute, df_md_local_compute): Inline them.
gcc/ChangeLog-2009:     (df_md_local_compute): Only include live registers in init.
gcc/ChangeLog-2009:     df_md_bb_local_compute, df_md_local_compute, df_md_reset,
gcc/df-problems.cc:df_md_local_compute (bitmap all_blocks)
gcc/df-problems.cc:  df_md_local_compute,        /* Local compute function.  */
grep: build/gcc/cc1 : fichiers binaires correspondent
grep: build/gcc/lto-dump : fichiers binaires correspondent
grep: build/gcc/df-problems.o : fichiers binaires correspondent
grep: build/gcc/lto1 : fichiers binaires correspondent
grep: build/gcc/cc1plus : fichiers binaires correspondent
```

L'algorithme est effecuté dans `compute_dominance_frontiers`: il faut modifier pour la **post_dominance**:

```c
static void
df_md_local_compute (bitmap all_blocks)
{
  unsigned int bb_index, df_bb_index;
  bitmap_iterator bi1, bi2;
  basic_block bb;
  bitmap_head *frontiers;

  bitmap_initialize (&seen_in_insn, &bitmap_default_obstack);

  EXECUTE_IF_SET_IN_BITMAP (all_blocks, 0, bb_index, bi1)
    {
      df_md_bb_local_compute (bb_index);
    }

  bitmap_release (&seen_in_insn);

  frontiers = XNEWVEC (bitmap_head, last_basic_block_for_fn (cfun));
  FOR_ALL_BB_FN (bb, cfun)
    bitmap_initialize (&frontiers[bb->index], &bitmap_default_obstack);

  compute_dominance_frontiers (frontiers);

  /* Add each basic block's kills to the nodes in the frontier of the BB.  */
  EXECUTE_IF_SET_IN_BITMAP (all_blocks, 0, bb_index, bi1)
    {
      bitmap kill = &df_md_get_bb_info (bb_index)->kill;
      EXECUTE_IF_SET_IN_BITMAP (&frontiers[bb_index], 0, df_bb_index, bi2)
	{
	  basic_block bb = BASIC_BLOCK_FOR_FN (cfun, df_bb_index);
	  if (bitmap_bit_p (all_blocks, df_bb_index))
	    bitmap_ior_and_into (&df_md_get_bb_info (df_bb_index)->init, kill,
				 df_get_live_in (bb));
	}
    }

  FOR_ALL_BB_FN (bb, cfun)
    bitmap_clear (&frontiers[bb->index]);
  free (frontiers);
}
```

Donc on recherche cette fonction:

```bash
┌─[✗]─[night@night-20b7s2ex01]─[~/S5/gcc-12.2.0]
└──╼ 79 fichiers, 15Mb)─$ grep -r compute_dominance_frontiers
gcc/ChangeLog-2004:     * cfganal.c (compute_dominance_frontiers_1): Likewise.
gcc/ChangeLog-2004:     (compute_dominance_frontiers_1): Likewise.
gcc/ChangeLog-2004:     * cfganal.c (compute_dominance_frontiers_1) Ditto.
gcc/ChangeLog-2004:     * tree-flow.h (compute_dominance_frontiers): Move prototype...
gcc/ChangeLog-2004:     * tree-cfg.c (compute_dominance_frontiers_1,
gcc/ChangeLog-2004:     compute_dominance_frontiers): Move from here...
gcc/cfganal.cc:compute_dominance_frontiers (bitmap_head *frontiers)
gcc/cfganal.cc:   frontier information as returned by compute_dominance_frontiers.
```

`cfganal.cc`

( Entre parenthèses //pas utile

Donc il faut modifier dans ce **parcours en profondeur** ce qui marchait pour la dominance -> frontière post dominance

Plus bas : on se sert de cette fonction pour la **post-dominance**

```c++
void compute_dominance_frontiers (bitmap_head *frontiers)
{
  timevar_push (TV_DOM_FRONTIERS);

  edge p;
  edge_iterator ei;
  basic_block b;
  FOR_EACH_BB_FN (b, cfun)
    {
      if (EDGE_COUNT (b->preds) >= 2)
	{
	  basic_block domsb = get_immediate_dominator (CDI_DOMINATORS, b);
	  FOR_EACH_EDGE (p, ei, b->preds)
	    {
	      basic_block runner = p->src;
	      if (runner == ENTRY_BLOCK_PTR_FOR_FN (cfun))
		continue;

	      while (runner != domsb)
		{
		  if (!bitmap_set_bit (&frontiers[runner->index], b->index))
		    break;
		  runner = get_immediate_dominator (CDI_DOMINATORS, runner);
		}
	    }
	}
    }

  timevar_pop (TV_DOM_FRONTIERS);
}
```

**On change** (cf TD4):

  - `b->preds` en `b->succs`
  - `CDI_DOMINATORS` en `CDI_POST_DOMINATORS`
  - `ENTRY_BLOCK_PTR_FOR_FN` en `EXIT_BLOCK_PTR_FOR_FN`

```bash
┌─[night@night-20b7s2ex01]─[~/S5/CA_2023/TDs/TD5/CODE]
└──╼ 9 fichiers, 180Kb)─$ make
~/gcc12/bin/g++ -I`~/gcc12/bin/gcc -print-file-name=plugin`/include -g -Wall -fno-rtti -shared -fPIC  -o libplugin_TP5_1.so plugin_TP5_1.cpp
OMPI_MPICC=~/gcc12/bin/gcc mpicc test3.c -g -O3 -o TP5_1 -fplugin=./libplugin_TP5_1.so 
plugin_init: Entering...
plugin_init: Check ok...
plugin_init: Pass added...
plugin: gate... 
plugin: execute...
Function: 'main'
        MPI COLLECTIVE: 'MPI_Init' (code: 0)
        MPI COLLECTIVE: 'MPI_Barrier' (code: 4)
        Split the block 04
        MPI COLLECTIVE: 'MPI_Barrier' (code: 4)
        MPI COLLECTIVE: 'MPI_Barrier' (code: 4)
        MPI COLLECTIVE: 'MPI_Barrier' (code: 4)
        MPI COLLECTIVE: 'MPI_Finalize' (code: 1)
        BB 02: 
        BB 03: 10
        BB 04: 3
        BB 13: 
        BB 05: 13
        BB 06: 13
        BB 07: 6
        BB 08: 3
        BB 09: 6, 13
        BB 10: 6, 13
        BB 11: 10
        BB 12: 13
[GRAPHVIZ] Generating CFG of function main in file <main_test3.c_8_main_q7.dot>
```

## Q2

On obtient `CFG'` (ici sans 9->10) en stockant dans une pile les prédecesseurs sans boucle

```bash
[GRAPHVIZ] Generating CFG of function main in file <main_test3.c_8_q1.dot>
~/gcc12/bin/g++ -I`~/gcc12/bin/gcc -print-file-name=plugin`/include -g -Wall -fno-rtti -shared -fPIC  -o libplugin_TP5_2.so plugin_TP5_2.cpp
OMPI_MPICC=~/gcc12/bin/gcc mpicc test3.c -g -O3 -o TP5_2 -fplugin=./libplugin_TP5_2.so 
plugin_init: Entering...
plugin_init: Check ok...
plugin_init: Pass added...
plugin: gate... 
plugin: execute...
Function: 'main'
        MPI COLLECTIVE: 'MPI_Init' (code: 0)
        MPI COLLECTIVE: 'MPI_Barrier' (code: 4)
        Split the block 04
        MPI COLLECTIVE: 'MPI_Barrier' (code: 4)
        MPI COLLECTIVE: 'MPI_Barrier' (code: 4)
        MPI COLLECTIVE: 'MPI_Barrier' (code: 4)
        MPI COLLECTIVE: 'MPI_Finalize' (code: 1)
        BB 02: 2
        BB 03: 2, 3, 10
        BB 04: 2, 3, 4, 10
        BB 13: 2, 3, 4, 10, 13
        BB 05: 2, 3, 4, 5, 10, 13
        BB 06: 2, 3, 4, 6, 10, 13
        BB 07: 2, 3, 4, 6, 7, 10, 13
        BB 08: 2, 3, 8, 10
        BB 09: 2, 3, 4, 5, 6, 9, 10, 13
        BB 10: 2, 10
        BB 11: 2, 10, 11
        BB 12: 2, 3, 4, 6, 7, 8, 10, 11, 12, 13
```
