## Rappel - jusqu'au TD4

`plugin_TP5_0.cpp`

![0](CODE/main_test3.c_8_0_ini.png)

![1](CODE/main_test3.c_8_1_mpi.png)

![2](CODE/main_test3.c_8_2_split.png)

## Q1

```bash
┌─[night@night-20b7s2ex01]─[~/gcc12]
└──╼ 6 fichiers, 24Kb)─$ find . -name "bitmap.h"
./lib/gcc/x86_64-pc-linux-gnu/12.2.0/plugin/include/bitmap.h
```

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
