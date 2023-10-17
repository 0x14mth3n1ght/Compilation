#include <stdio.h>

#include "plugin_headers.h"

#include "bitmap_array.h"
#include "clear_aux.h"
#include "cfgviz.h"
#include "mpi_collectives.h"
#include "queue.h"

// GLOBALS ====================================================================

// Global variable needed to make a GCC plugin work.
// Setting it to `1` is not necessary but I disliked uninitialized variables.
int plugin_is_GPL_compatible = 1;

// Global data about the pass.
// Needed by the constructor for the pass class.
const pass_data td5_pass_data = {
    .type = GIMPLE_PASS,
    .name = "TD5",
    .optinfo_flags = OPTGROUP_NONE,
    .tv_id = TV_OPTIMIZE,
    .properties_required = 0,
    .properties_provided = 0,
    .properties_destroyed = 0,
    .todo_flags_start = 0,
    .todo_flags_finish = 0,
};

// HELPER FUNCTIONS FOR THE PASS ==============================================

// Fills the post dominance frontiers for each basic block in the function
// (only the non-virtual ones).
void fill_bitmaps_with_post_dominance_frontiers(
    const function* const fun, bitmap_array& post_dom_frontiers)
{
    basic_block bb = NULL;
    FOR_EACH_BB_FN(bb, fun)
    {
        // When there is only one successor, it cannot be a frontier.
        if (EDGE_COUNT(bb->succs) >= 2) {
            edge e;
            edge_iterator ei;
            FOR_EACH_EDGE(e, ei, bb->succs)
            {
                basic_block runner = e->dest;

                if (runner == EXIT_BLOCK_PTR_FOR_FN(fun))
                    continue;

                // Find the frontier.
                basic_block immediate_post_dominator
                    = get_immediate_dominator(CDI_POST_DOMINATORS, bb);
                while (runner != immediate_post_dominator) {
                    if (!bitmap_set_bit(
                            &post_dom_frontiers[runner->index], bb->index))
                        break;

                    runner
                        = get_immediate_dominator(CDI_POST_DOMINATORS, runner);
                }
            }
        }
    }

    // Clears self from post domination frontiers.
    bb = NULL;
    FOR_EACH_BB_FN(bb, fun)
    {
        bitmap_clear_bit(&post_dom_frontiers[bb->index], bb->index);
    }
}

// Fills the predecessors for each basic block in the function (only the
// non-virtual ones).
void fill_all_preds(const function* const fun, bitmap_array& all_preds)
{
    // The queue will be used in the breadth first traversal to build a
    // stack of the basic blocks to visit.
    Queue<basic_block> queue {};
    queue.push(ENTRY_BLOCK_PTR_FOR_FN(fun)->next_bb);

    // While there are nodes to visit, get the first and its informations.
    while (!queue.is_empty()) {
        basic_block current_bb = queue.pop();
        // Self counts as a predecessor, it should avoid surprises
        // like infinite loops.
        bitmap_set_bit(&all_preds[current_bb->index], current_bb->index);

        // For each successor that is not the exit of the function.
        edge e;
        edge_iterator ei;
        FOR_EACH_EDGE(e, ei, current_bb->succs)
        {
            // Get the informations about the destination.
            basic_block dest_bb = e->dest;

            // Ensure it is not the exit.
            if (dest_bb == EXIT_BLOCK_PTR_FOR_FN(fun)) {
                continue;
            }

            // Finding out if the destination is already a predecessor.
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

            // Else join the bitmaps and add the block to the queue to be
            // explored later.
            bitmap_ior_into(
                &all_preds[dest_bb->index], &all_preds[current_bb->index]);
            queue.push(dest_bb);
        }
    }
}

// `true` if `outer` contains `inner` (is a superset of).
bool bitmap_contains(const bitmap outer, const bitmap inner)
{
    // Allocate only once.
    static bitmap and_bm = bitmap_alloc(NULL);
    bitmap_and(and_bm, outer, inner);
    return bitmap_equal_p(and_bm, inner);
}

// Traversal of the graph using the auxiliary informations to mark the rank of
// each node.
//
// The first block will always have a rank of 0 and the last real block will
// always have the maximum rank.
//
// The `ranks` array must have the same size as `all_preds` (ergo the
// number of basic blocks).
//
// This rank marking is a little special: instead of adding 1 when the MPI code
// is not `LAST_AND_UNUSED_MPI_COLLECTIVE_CODE`, the code itself is added.
// This directly sorts the nodes by rank.
void mark_rank(
    const function* const fun, const bitmap_array& all_preds, uint64_t ranks[])
{
    size_t idx = 0;
    FOR_EACH_BITMAP_ARRAY_ELEM(all_preds, idx) { ranks[idx] = 0; }

    bitmap_head visited;
    bitmap_initialize(&visited, &bitmap_default_obstack);

    Queue<basic_block> queue {};
    queue.push(ENTRY_BLOCK_PTR_FOR_FN(fun)->next_bb);

    while (!queue.is_empty()) {
        basic_block current_bb = queue.pop();
        uint64_t current_rank = ranks[current_bb->index];

        bitmap_set_bit(&visited, current_bb->index);
        if (!bitmap_contains(&visited, &all_preds[current_bb->index])) {
            bitmap_clear_bit(&visited, current_bb->index);
            continue;
        }

        edge e;
        edge_iterator ei;
        FOR_EACH_EDGE(e, ei, current_bb->succs)
        {
            basic_block dest_bb = e->dest;

            // If the destination is the exit or a predecessor, ignore it.
            if (dest_bb == EXIT_BLOCK_PTR_FOR_FN(fun)
                || bitmap_bit_p(&all_preds[current_bb->index], dest_bb->index)) {
                continue;
            }

            queue.push(dest_bb);

            // MPI Collective code of the destination basic block.
            uint64_t dest_old_rank = ranks[dest_bb->index];
            if (current_rank > dest_old_rank) {
                ranks[dest_bb->index] = current_rank;
            }

            if (dest_bb->aux != (void*)LAST_AND_UNUSED_MPI_COLLECTIVE_CODE) {
                ranks[dest_bb->index] += reinterpret_cast<uint64_t>(dest_bb->aux)
                    + static_cast<uint64_t>(LAST_AND_UNUSED_MPI_COLLECTIVE_CODE);
            }
        }
    }
}

// Register the ranks that are used as well as the nodes they contains and
// the predecessors for each groups.
//
// `ranks` must have been filled with `mark_rank` previously.
//
// NOTE: this function will only consider basic blocks with an MPI collective
// call inside them.
void fill_node_sets_and_rank_set_and_all_preds_by_rank(const function* const fun,
    const bitmap_array& all_preds,
    const uint64_t ranks[],
    bitmap_array& node_sets_by_rank,
    bitmap_head& rank_set,
    bitmap_array& all_preds_by_rank)
{
    // Filling all the relevant bitmaps for each node.
    basic_block bb = NULL;
    FOR_EACH_BB_FN(bb, fun)
    {
        const uint64_t rank = ranks[bb->index];
        bitmap_set_bit(&rank_set, rank);
        // Uncomment when you want to only include the basic blocks with MPI
        // calls in them.
        if (bb->aux == (void*)LAST_AND_UNUSED_MPI_COLLECTIVE_CODE) {
            continue;
        }
        bitmap_set_bit(&node_sets_by_rank[rank], bb->index);
        bitmap_ior_into(&all_preds_by_rank[rank], &all_preds[bb->index]);
    }

    // Ensuring a node is not marked as a predecessor of itself.
    bb = NULL;
    FOR_EACH_BB_FN(bb, fun)
    {
        const uint64_t rank = ranks[bb->index];
        bitmap_clear_bit(&all_preds_by_rank[rank], bb->index);
    }
}

// Fills the bitmaps in `groups_post_dom` with the postdominators for the node
// that have the corresponding rank.
//
// `ranks` must have been filled previously by `mark_rank`.
void fill_bitmap_with_post_dominance_for_group(const function* const fun,
    const bitmap_array& all_preds_by_rank,
    const bitmap_head& rank_set,
    const bitmap_array& node_sets_by_rank,
    bitmap_array& groups_post_dom)
{
    size_t rank = 0;
    // For each rank, complete the array of nodes that are post-dominated
    // by this rank.
    FOR_EACH_BITMAP_ARRAY_ELEM(node_sets_by_rank, rank)
    {
        // Ignore ranks that do not contains any node.
        if (!bitmap_bit_p((bitmap)&rank_set, rank)) {
            continue;
        }

        Queue<basic_block> queue {};

        basic_block node = NULL;
        FOR_EACH_BB_FN(node, fun)
        {
            // Only consider nodes that are of the current rank.
            if (!bitmap_bit_p(&node_sets_by_rank[rank], node->index)) {
                continue;
            }

            // When there is only one predecessor that fits the conditions it
            // is obviously post-dominated by the current `rank`.
            if (EDGE_COUNT(node->preds) == 1) {
                const basic_block pred = EDGE_PRED(node, 0)->src;
                // Ensures the predecessor is not the entry block to `fun`.
                if (pred != ENTRY_BLOCK_PTR_FOR_FN(fun)
                    // Ensures the predecessor is not a looping node.
                    && bitmap_bit_p(&all_preds_by_rank[rank], pred->index)
                    // Ensures the predecessor is not of the same rank.
                    && !bitmap_bit_p(&node_sets_by_rank[rank], pred->index)) {
                    // Add the predecessor to the queue to visit.
                    queue.push(pred);
                    continue;
                }
            }
        }
        // Now the queue is full with nodes that are post-dominated by the
        // current rank.

        node = NULL;
        FOR_EACH_BB_FN(node, fun)
        {
            // Only consider nodes that are of the current rank.
            if (!bitmap_bit_p(&node_sets_by_rank[rank], node->index)) {
                continue;
            }

            // Already treated above.
            if (EDGE_COUNT(node->preds) == 1) {
                continue;
            }

            edge e_preds = NULL;
            edge_iterator ei_preds;
            FOR_EACH_EDGE(e_preds, ei_preds, node->preds)
            {
                const basic_block pred = e_preds->src;

                // Ignore the starting block for the function.
                if (pred == ENTRY_BLOCK_PTR_FOR_FN(fun)) {
                    continue;
                }

                // Ignore the predecessor if it has the same rank.
                if (bitmap_bit_p(&node_sets_by_rank[rank], pred->index)) {
                    continue;
                }

                // Ignore nodes that are looping ones.
                if (!bitmap_bit_p(&all_preds_by_rank[rank], pred->index)) {
                    continue;
                }

                queue.push(pred);
            }
        }
        // When this point is reached `queue` is full with direct predecessors
        // to the current rank.

        while (!queue.is_empty()) {
            const basic_block pred = queue.pop();

            edge e_succs = NULL;
            edge_iterator ei_succs {};
            bool all_post_dom_or_in_group = true;
            FOR_EACH_EDGE(e_succs, ei_succs, pred->succs)
            {
                const basic_block succ = e_succs->dest;

                // The exit block does not belong to any rank group.
                if (succ == EXIT_BLOCK_PTR_FOR_FN(fun)) {
                    all_post_dom_or_in_group = false;
                    break;
                }

                // If the successor is of the same rank as `node` or
                // post-dominated by the group with rank `rank`, there is
                // still a chance `pred` is post-dominated by the group too.
                if (bitmap_bit_p(&groups_post_dom[rank], succ->index)
                    || bitmap_bit_p(&node_sets_by_rank[rank], succ->index)) {
                    continue;
                }

                // Coming here means `succ` is not post-dominated by `rank`
                // nor of the same rank as `node`: it is another branch
                // and so not in the same group.
                all_post_dom_or_in_group = false;
                break;
            }

            // When all successors have the same rank as `node` or are
            // post-dominated by `rank` it means `pred` is post-dominated by
            // `rank` too.
            if (all_post_dom_or_in_group) {
                bitmap_set_bit(&groups_post_dom[rank], pred->index);

                edge e_pred = NULL;
                edge_iterator ei_pred {};
                FOR_EACH_EDGE(e_pred, ei_pred, pred->preds)
                {
                    const basic_block src = e_pred->src;
                    if (src != ENTRY_BLOCK_PTR_FOR_FN(fun)
                        && bitmap_bit_p(&all_preds_by_rank[rank], src->index)) {
                        queue.push(src);
                    }
                }
            }
        }
    }
}

// Fills `groups_frontiers` with the post-dominance frontiers for each rank.
//
// After this function has been called, the `groups_frontier` array will
// contain the nodes that are post-dominated by the corresponding rank.
void fill_bitmap_with_post_dominance_frontier_for_group(
    const function* const fun,
    const bitmap_array& all_preds_by_rank,
    const bitmap_head& rank_set,
    const bitmap_array& node_sets_by_rank,
    const bitmap_array& post_dom_frontiers,
    const bitmap_array& groups_post_dom,
    bitmap_array& groups_post_dom_frontier)
{
    size_t rank = 0;
    // For each rank, complete the array of nodes that are post-dominated
    // by this rank.
    FOR_EACH_BITMAP_ARRAY_ELEM(node_sets_by_rank, rank)
    {
        // Ignore ranks that do not contains any node.
        if (!bitmap_bit_p((bitmap)&rank_set, rank)) {
            continue;
        }

        // First gather the post-dominance frontiers of each node into the
        // post-dominance frontier for the group.
        basic_block node = NULL;
        FOR_EACH_BB_FN(node, fun)
        {
            bitmap_ior_into(&groups_post_dom_frontier[rank],
                &post_dom_frontiers[node->index]);
        }

        node = NULL;
        FOR_EACH_BB_FN(node, fun)
        {
            if (!bitmap_bit_p(&all_preds_by_rank[rank], node->index)
                || bitmap_bit_p(&groups_post_dom[rank], node->index)
                || bitmap_bit_p(&node_sets_by_rank[rank], node->index)) {
                bitmap_clear_bit(&groups_post_dom_frontier[rank], node->index);
            }
        }
    }
}

// Fills `to_warn_blocks` with the basic block indexes that could be problematic
// when running the program with respect to MPI collective calls.
void fill_bitmap_to_warn_blocks(
    const bitmap_array& groups_post_dom_frontier, bitmap_head& to_warn_blocks)
{
    size_t rank = 0;
    FOR_EACH_BITMAP_ARRAY_ELEM(groups_post_dom_frontier, rank)
    {
        bitmap_ior_into(
            (bitmap)&to_warn_blocks, &groups_post_dom_frontier[rank]);
    }
}

// Runs all the helpers for the pass, in the correct order and setup-ing
// everything correctly.
void run_all_helpers(function* fun)
{
    calculate_dominance_info(CDI_POST_DOMINATORS);

    const size_t basic_block_count = (size_t)last_basic_block_for_fn(fun);

    // Q1.
    bitmap_array post_dom_frontiers { basic_block_count };
    fill_bitmaps_with_post_dominance_frontiers(fun, post_dom_frontiers);
    printf("Q1. Post dominance frontier for a node.\n");
    basic_block bb = NULL;
    FOR_EACH_BB_FN(bb, fun)
    {
        printf("\tBB %02d: ", bb->index);
        bitmap_print(stdout, &post_dom_frontiers[bb->index], "", "\n");
    }
    printf("Done\n");

    // Q2.
    bitmap_array all_preds { basic_block_count };
    fill_all_preds(fun, all_preds);
    printf("Q2. CFG'.\n");
    bb = NULL;
    FOR_EACH_BB_FN(bb, fun)
    {
        printf("\tBB %02d: ", bb->index);
        bitmap_print(stdout, &all_preds[bb->index], "", "\n");
    }
    printf("Done\n");

    // Q3.
    uint64_t ranks[basic_block_count];
    mark_rank(fun, all_preds, ranks);
    printf("Q3. Rank.\n");
    bb = NULL;
    FOR_EACH_BB_FN(bb, fun)
    {
        printf("\tBB %02d: rank %02lld\n", bb->index, ranks[bb->index]);
    }
    printf("Done\n");

    // Filling informations about the ranks.
    const uint64_t rank_count = ranks[basic_block_count - 1] + 1;
    bitmap_array all_preds_by_rank { (size_t)rank_count };
    bitmap_array node_sets_by_rank { (size_t)rank_count };
    bitmap_head rank_set;
    bitmap_initialize(&rank_set, &bitmap_default_obstack);
    fill_node_sets_and_rank_set_and_all_preds_by_rank(
        fun, all_preds, ranks, node_sets_by_rank, rank_set, all_preds_by_rank);
    printf("Informations about the ranks.\n");
    bitmap_print(stdout, &rank_set, "\trank set: ", "\n");
    size_t i = 0;
    FOR_EACH_BITMAP_ARRAY_ELEM(node_sets_by_rank, i)
    {
        if (!bitmap_bit_p(&rank_set, i)) {
            continue;
        }
        printf("\trank [%02ld]\n", i);
        bitmap_print(stdout, &node_sets_by_rank[i], "\t\tnodes: ", "\n");
        bitmap_print(stdout, &all_preds_by_rank[i], "\t\tpreds: ", "\n");
    }
    printf("Done\n");

    // Q4.
    bitmap_array groups_post_dom { (size_t)rank_count };
    fill_bitmap_with_post_dominance_for_group(
        fun, all_preds_by_rank, rank_set, node_sets_by_rank, groups_post_dom);
    printf("Q4. Post dominance for a group.\n");
    i = 0;
    FOR_EACH_BITMAP_ARRAY_ELEM(node_sets_by_rank, i)
    {
        if (!bitmap_bit_p(&rank_set, i)) {
            continue;
        }
        printf("\trank [%02ld] ", i);
        bitmap_print(stdout, &groups_post_dom[i], "", "\n");
    }
    printf("Done\n");

    // Q5.
    bitmap_array groups_post_dom_frontier { (size_t)rank_count };
    fill_bitmap_with_post_dominance_frontier_for_group(fun,
        all_preds_by_rank,
        rank_set,
        node_sets_by_rank,
        post_dom_frontiers,
        groups_post_dom,
        groups_post_dom_frontier);
    printf("Q5. Post dominance frontier for a group.\n");
    i = 0;
    FOR_EACH_BITMAP_ARRAY_ELEM(node_sets_by_rank, i)
    {
        if (!bitmap_bit_p(&rank_set, i)) {
            continue;
        }
        printf("\trank [%02ld] ", i);
        bitmap_print(stdout, &groups_post_dom_frontier[i], "", "\n");
    }
    printf("Done\n");

    // Q6.
    printf("Q6. First warnings.\n");
    i = 0;
    FOR_EACH_BITMAP_ARRAY_ELEM(node_sets_by_rank, i)
    {
        if (!bitmap_bit_p(&rank_set, i)) {
            continue;
        }
        printf("\trank [%02ld] ", i);

        bitmap_print(stdout, &node_sets_by_rank[i], "(Nodes: ", "): ");

        if (bitmap_count_bits(&groups_post_dom_frontier[i]) > 0) {
            printf("ISSUE WITH MPI CALLS\n");
        } else

        {
            printf("no issue with mpi calls\n");
        }
    }
    printf("Done\n");

    // I think I did Q7 at the same time as Q5 and I don't really understand
    // what an iterated PDF is anyway so for now let's do Q8.

    // Q8.
    printf("Q8. Better warnings.\n");
    bitmap_head to_warn_blocks;
    bitmap_initialize(&to_warn_blocks, &bitmap_default_obstack);
    fill_bitmap_to_warn_blocks(groups_post_dom_frontier, to_warn_blocks);
    bb = NULL;
    FOR_EACH_BB_FN(bb, fun)
    {
        // Ignores basic blocks that are not to be warned about.
        if (!bitmap_bit_p(&to_warn_blocks, bb->index)) {
            continue;
        }

        const gimple_stmt_iterator gsi = gsi_last_bb(bb);
        const gimple* stmt = gsi_stmt(gsi);
        const location_t loc = gimple_location(stmt);

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
        warning_at(loc,
            OPT_fplugin_,
            "Basic block %<%d%> causes a possibly invalid split in the MPI "
            "collectives call sequence",
            bb->index);
#pragma GCC diagnostic pop
    }
    printf("Done\n");

    // Freeing things.
    free_dominance_info(CDI_POST_DOMINATORS);
}

// PASS CLASS =================================================================

// GCC Plugin pass implementation.
class td5_pass : public gimple_opt_pass {
public:
    // Constructor using the global `pass_data` and the passed context
    // directly in the `gimple_opt_pass` constructor.
    td5_pass(gcc::context* ctxt)
        : gimple_opt_pass(td5_pass_data, ctxt)
    {
    }

    // Needed because GCC will make a copy of the given pass pointer to ensure
    // it will live long enough for its internal use.
    td5_pass* clone() { return new td5_pass(g); }

    // Returns `true` when the pass should be run of the given function.
    bool gate(function*) { return true; }

    // Execute the pass on the given function `fun`.
    //
    // Returns `0` when all was well.
    //
    // Cleanup of the basic block's auxiliary data must be done before this
    // function is finished.
    unsigned int execute(function* fun)
    {
        printf("Function: '%s'\n", function_name(fun));

        // Splitting to have only one MPI call in each basic block.
        split_on_mpi_collectives(fun);
        set_aux_to_mpi_code(fun);

        run_all_helpers(fun);

        // Final cleanup.
        cfgviz_dump(fun, "td5");
        clear_all_basic_blocks_in_fn(fun, NULL);

        return 0;
    }
};

// PLUGIN INIT ================================================================

// GCC Plugin entry point. Returns `0` when everything happened correctly.
// This function setups the passes and pragmas.
int plugin_init(
    struct plugin_name_args* plugin_info, struct plugin_gcc_version* version)
{
    // Ensure the plugin's version and GCC's version are in sync.
    if (!plugin_default_version_check(version, &gcc_version))
        return 1;

    // `g` is the global GCC context object.
    td5_pass my_pass { g };

    // Information about the pass positioning necessary for GCC to know when
    // to call it.
    struct register_pass_info my_pass_info = {
        .pass = &my_pass,
        .reference_pass_name = "cfg",
        .ref_pass_instance_number = 0,
        .pos_op = PASS_POS_INSERT_AFTER,
    };

    // Registering the pass inside GCC plugin system.
    register_callback(
        plugin_info->base_name, PLUGIN_PASS_MANAGER_SETUP, NULL, &my_pass_info);

    return 0;
}
