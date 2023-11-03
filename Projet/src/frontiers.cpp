#include "../headers/plugin.h"

bitmap_head* bitmap_init(){
  basic_block bb;
  bitmap_head *frontiers = XNEWVEC (bitmap_head, last_basic_block_for_fn (cfun));
  FOR_ALL_BB_FN (bb, cfun)
    bitmap_initialize (&frontiers[bb->index], &bitmap_default_obstack);
  return frontiers;
}

void post_dom_frontier(function *fun, bitmap_head *frontiers){
	edge p;
	edge_iterator ei;
	basic_block b;

	FOR_EACH_BB_FN (b, fun)
	{
    	if (EDGE_COUNT (b->succs) >= 2)
    	{
			basic_block domsb = get_immediate_dominator (CDI_POST_DOMINATORS, b);
			FOR_EACH_EDGE (p, ei, b->succs)
	    	{
        		basic_block runner = p->dest;
				if (runner == EXIT_BLOCK_PTR_FOR_FN (fun))
					continue;

        		while (runner != domsb)
        		{
          			if (!bitmap_set_bit (&frontiers[runner->index], b->index))
						break;
         			runner = get_immediate_dominator (CDI_POST_DOMINATORS, runner);
				}
	    	}
    	}
  	}

	b = NULL;
    FOR_EACH_BB_FN(b, fun)
    {
        bitmap_clear_bit(&frontiers[b->index], b->index);
    	printf("\tBB %02d: ", b->index);
    	bitmap_print(stdout, &frontiers[b->index], "", "\n");
  	}
}
