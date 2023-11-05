#include "../headers/plugin.h"
#include <stack>

bitmap_head* bitmap_init(){
  basic_block bb;
  bitmap_head *frontiers = XNEWVEC (bitmap_head, last_basic_block_for_fn (cfun));
  FOR_ALL_BB_FN (bb, cfun)
    bitmap_initialize (&frontiers[bb->index], &bitmap_default_obstack);
  return frontiers;
}

void post_dom_frontiers(function *fun, bitmap_head *frontiers){
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

void removeloop_cfg2(function *fun, bitmap_head *all_preds)
{                                         
    std::stack<basic_block> stack;
    stack.push(ENTRY_BLOCK_PTR_FOR_FN(fun)->next_bb);

    while (!stack.empty()) {
        basic_block current_bb = stack.top();
        stack.pop();
        
        bitmap_set_bit(&all_preds[current_bb->index], current_bb->index);

        edge e;
        edge_iterator ei;
        FOR_EACH_EDGE(e, ei, current_bb->succs) 
        {
            basic_block dest_bb = e->dest;
            if (dest_bb == EXIT_BLOCK_PTR_FOR_FN(fun)) {
                continue;
            }

            edge e2;
            edge_iterator ei2;
            bool is_already_pred = false;
            FOR_EACH_EDGE(e2, ei2, current_bb->preds)
            {
                basic_block src_bb = e->src;
                if (bitmap_bit_p(&all_preds[src_bb->index], dest_bb->index)) {
                    is_already_pred = true;
                    break;
                }
            }

            // If the destination is already a predecessor, ignore it.
            if (is_already_pred) {
                continue;
            }
            // Else join the bitmaps and add the block to the stack to be explored later.
            bitmap_ior_into(&all_preds[dest_bb->index], &all_preds[current_bb->index]);
            stack.push(dest_bb);
        }
    }
    basic_block bb = NULL;
    FOR_EACH_BB_FN(bb, fun)
    {
        printf("\tBB %02d: ", bb->index);
        bitmap_print(stdout, &all_preds[bb->index], "", "\n");
    }
    printf("Done\n");
}
