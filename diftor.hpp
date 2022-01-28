#ifndef DEFFERENTIATOR_ACTIVE
#define DEFFERENTIATOR_ACTIVE

#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <ctype.h>
#include "bin_tree.hpp"

constexpr int MAX_SYMB_SEQUNCE = 6;
bin_tree_t *init_expression_tree (char *expr_buffer);
bin_tree_t *load_expression ();

#endif

/*

корни, квадраты в ранней подписи
обработка ошибок при формировании дерева

*/
