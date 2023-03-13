#ifndef DEFFERENTIATOR_ACTIVE
#define DEFFERENTIATOR_ACTIVE

#define EXPR_TREE_AUTO_QUICK_VERIFICATION

#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <ctype.h>
#include "../bintree/bin_tree.hpp"

constexpr int MAX_SYMB_SEQUNCE = 6;

bin_tree_t *load_expression ();
bin_tree_t *create_diff_expr_tree (bin_tree_t *tree);
void optimize_expr_tree (bin_tree_t *tree);

#endif

// TODO: добавить макровставки в частные верификаторы
// TODO: проработать буквенные константы (мультивары и дифференцирование)
