#include "diftor.hpp"
#include <stdio.h>

int main () {

    bin_tree_t *init_expr_tree = load_expression ();
    bin_tree_vis_dump (init_expr_tree, "dump1.png");
    bin_tree_form_dump (init_expr_tree, "dump1.pdf");

    optimize_expr_tree (init_expr_tree);
    bin_tree_vis_dump (init_expr_tree, "dump2.png");
    bin_tree_form_dump (init_expr_tree, "dump2.pdf");

    bin_tree_t *diff_expr_tree = create_diff_expr_tree (init_expr_tree);
    bin_tree_vis_dump (diff_expr_tree, "dump3.png");
    bin_tree_form_dump (diff_expr_tree, "dump3.pdf");

    optimize_expr_tree (diff_expr_tree);
    bin_tree_vis_dump (diff_expr_tree, "dump4.png");
    bin_tree_form_dump (diff_expr_tree, "dump4.pdf");

    bin_tree_dtor (init_expr_tree);
    bin_tree_dtor (diff_expr_tree);

    return 0;
}