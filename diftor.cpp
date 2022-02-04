#include "diftor.hpp"

static bin_tree_t *init_expression_tree (char * const expr_buffer);
static bin_node_t *scan_operation (char **expr_ptr);
static char *draw_symb_sequence (char **expr_ptr);
static char *download_buffer (FILE *expression_file);
static bin_node_t *create_diff_branch (bin_node_t *node);
static void optimize_expr_branch (bin_node_t *node);

static void suppress_left_branch_to_constant (bin_node_t *node, long long cnst);
static void suppress_right_branch_to_constant (bin_node_t *node, long long cnst);
static void pull_up_left_branch (bin_node_t *node);
static void pull_up_right_branch (bin_node_t *node);

static int func_secure_lrb (char **pptr);
static int func_secure_rrb (char **pptr);
static int func_secure_lsb (char **pptr);
static int func_secure_rsb (char **pptr);
static int func_secure_uo (long long id);
static int func_secure_mse (char *pptr);
static int func_secure_pm (char **pptr);

#define NO_LEFT_ROUND_BRAC(pptr) **pptr != '('
#define NO_RIGHT_ROUND_BRAC(pptr) **pptr != ')'
#define NO_LEFT_SQUARE_BRAC(pptr) **pptr != '['
#define NO_RIGHT_SQUARE_BRAC(pptr) **pptr != ']'
#define UNKNOWN_OPERATION(identificator) identificator == -1
#define SEQUENCE_MAX_EXCEEDED(ptr) ptr == NULL
#define NOT_A_UN_PLUS_OR_MINUS(pptr) **pptr != '+' && **pptr != '-'
#define BRANCH_COLLAPSED(node_ptr) node_ptr == NULL

#define PRINT_ERROR                                                                         \
    do {                                                                                    \
                                                                                            \
        printf ("\nTree formation failed: impossible expression sequence (%d, %s, %s)\n",   \
                __LINE__, __PRETTY_FUNCTION__, __FILE__);                                   \
                                                                                            \
    } while (0)

static bin_tree_t *init_expression_tree (char * const expr_buffer) {

    bin_tree_t *tree = (bin_tree_t *) calloc (1, sizeof (bin_tree_t));
    bin_tree_ctor (tree);
    
    char *expr_ptr = expr_buffer;
    if (!func_secure_lrb (&expr_ptr)) {

        PRINT_ERROR;
        bin_tree_free_branch (tree->root);
        return NULL;
    }
    expr_ptr += 1;
    tree->root = scan_operation (&expr_ptr);

    if (tree->root != NULL) {

        if (!func_secure_rrb (&expr_ptr)) {

            PRINT_ERROR;
            bin_tree_free_branch (tree->root);
            return NULL;
        }

    } else {

        return NULL;
    }

    return tree;
}

static bin_node_t *scan_operation (char **p_expr_ptr) {

    bin_node_t *node = (bin_node_t *) calloc (1, sizeof (bin_node_t));

    if (**p_expr_ptr != '(') {

        if (isdigit (**p_expr_ptr)) {

            node->data = atoll (*p_expr_ptr);
            node->type = CONSTANT;
            node->left = NULL;
            node->right = NULL;
            
            *p_expr_ptr += num_of_digits_llint (node->data);
            return node;

        } else {

            char *symb_seq = draw_symb_sequence (p_expr_ptr);
            if (!func_secure_mse (symb_seq)) {

                PRINT_ERROR;
                bin_tree_free_branch (node);
                return NULL;
            }

            if (symb_seq [0] == '\0') {

                free (symb_seq);
                if (!func_secure_pm (p_expr_ptr)) {

                    PRINT_ERROR;
                    bin_tree_free_branch (node);
                    return NULL;
                }

                if (*(*p_expr_ptr)++ == '+') {

                    node->data = PLUS;

                } else {

                    node->data = MINUS;
                }

                if (!func_secure_lrb (p_expr_ptr)) {

                    PRINT_ERROR;
                    bin_tree_free_branch (node);
                    return NULL;
                }
                *p_expr_ptr += 1;
                node->left = scan_operation (p_expr_ptr);
                if (!func_secure_rrb (p_expr_ptr)) {

                    PRINT_ERROR;
                    bin_tree_free_branch (node);
                    return NULL;
                }
                if (BRANCH_COLLAPSED(node->left)) {

                    bin_tree_free_branch (node);
                    return NULL;
                }
                node->right = NULL;
                node->type = UNARY_OPERATION;

                *p_expr_ptr += 1;
                return node;
            }
            if (symb_seq [1] == '\0') {

                node->data = (long long) symb_seq [0];
                node->right = NULL;
                node->left = NULL;
                node->type = VARIABLE;

                free (symb_seq);
                return node;
            }
            
            node->data = str_lin_search (OPERATION_SYMBS, NUM_OF_UNARY_OPERATIONS + NUM_OF_BINARY_OPERATIONS, symb_seq);
            if (!func_secure_uo (node->data)) {

                PRINT_ERROR;
                bin_tree_free_branch (node);
                return NULL;
            }

            if (node->data == LOG) {

                node->type = BINARY_OPERATION;

                if (!func_secure_lsb (p_expr_ptr)) {

                    PRINT_ERROR;
                    bin_tree_free_branch (node);
                    return NULL;
                }
                *p_expr_ptr += 1;
                node->left = scan_operation (p_expr_ptr);
                if (BRANCH_COLLAPSED (node->left)) {

                    bin_tree_free_branch (node);
                    return NULL;
                }
                if (!func_secure_rsb (p_expr_ptr)) {

                    PRINT_ERROR;
                    bin_tree_free_branch (node);
                    return NULL;
                }
                *p_expr_ptr += 1;

                if (!func_secure_lrb (p_expr_ptr)) {

                    PRINT_ERROR;
                    bin_tree_free_branch (node);
                    return NULL;
                }
                *p_expr_ptr += 1;
                node->right = scan_operation (p_expr_ptr);
                if (BRANCH_COLLAPSED (node->right)) {

                    bin_tree_free_branch (node);
                    return NULL;
                }
                if (!func_secure_rrb (p_expr_ptr)) {

                    PRINT_ERROR;
                    bin_tree_free_branch (node);
                    return NULL;
                }

                free (symb_seq);
                *p_expr_ptr += 1;
                return node;
            }

            if (!func_secure_lrb (p_expr_ptr)) {

                PRINT_ERROR;
                bin_tree_free_branch (node);
                return NULL;
            }
            *p_expr_ptr += 1;
            node->left = scan_operation (p_expr_ptr);
            node->right = NULL;
            node->type = UNARY_OPERATION;
            if (BRANCH_COLLAPSED (node->left)) {

                bin_tree_free_branch (node);
                return NULL;
            }
            if (!func_secure_rrb (p_expr_ptr)) {

                PRINT_ERROR;
                bin_tree_free_branch (node);
                return NULL;
            }

            free (symb_seq);
            *p_expr_ptr += 1;
            return node;
        }
    }
    
    node->type = BINARY_OPERATION;
    *p_expr_ptr += 1;

    node->left = scan_operation (p_expr_ptr);
    if (BRANCH_COLLAPSED (node->left)) {

        bin_tree_free_branch (node);
        return NULL;
    }
    if (!func_secure_rrb (p_expr_ptr)) {

        PRINT_ERROR;
        bin_tree_free_branch (node);
        return NULL;
    }
    *p_expr_ptr += 1;

    node->data = first_symb_str_lin_search (OPERATION_SYMBS, NUM_OF_UNARY_OPERATIONS + NUM_OF_BINARY_OPERATIONS, *(*p_expr_ptr)++);
    if (!func_secure_uo (node->data)) {

        PRINT_ERROR;
        bin_tree_free_branch (node);
        return NULL;
    }
    if (!func_secure_lrb (p_expr_ptr)) {

        PRINT_ERROR;
        bin_tree_free_branch (node);
        return NULL;
    }
    *p_expr_ptr += 1;

    node->right = scan_operation (p_expr_ptr);
    if (BRANCH_COLLAPSED (node->right)) {

        bin_tree_free_branch (node);
        return NULL;
    }
    if (!func_secure_rrb (p_expr_ptr)) {

        PRINT_ERROR;
        bin_tree_free_branch (node);
        return NULL;
    }
    *p_expr_ptr += 1;

    return node;
}

static char *draw_symb_sequence (char **p_expr_ptr) {

    char *symb_seq = (char *) calloc (MAX_SYMB_SEQUNCE + 1, sizeof (char));
    int symbs = 0;
    for ( ; isalpha (**p_expr_ptr) && symbs < MAX_SYMB_SEQUNCE + 1; ++symbs, ++(*p_expr_ptr)) {

        symb_seq [symbs] = **p_expr_ptr;
    }

    if (symbs == MAX_SYMB_SEQUNCE + 1) {

        free (symb_seq);
        return NULL;
    }

    return symb_seq;
}

bin_tree_t *load_expression () {

    char *file_name = (char *) calloc (MAX_FILE_NAME + 1, sizeof (char));
    FILE *expression_file = NULL;
    while (true) {

        printf ("\nEnter the name of an expression file (with .qdf)\n");
        scanf ("%64s", file_name);
        flush_input_buffer (file_name [0]);

        int file_name_len = strlen (file_name);
        if (file_name [file_name_len - 4] != '.' ||
            file_name [file_name_len - 3] != 'q' ||
            file_name [file_name_len - 2] != 'd' ||
            file_name [file_name_len - 1] != 'f' ||
            file_name_len < 4) {

            printf ("\nWrong file format, try again\n");
            continue;
        }

        expression_file = fopen (file_name, "r");
        if (expression_file) {

            break;
        }

        printf ("\nNo such file in the directory, try again\n");
    }
    free (file_name);

    char *buffer = download_buffer (expression_file);
    fclose (expression_file);

    bin_tree_t *tree = init_expression_tree (buffer);

    free (buffer);
    return tree;
}

static char *download_buffer (FILE *expression_file) {

    size_t file_size = get_file_size (expression_file);
    char *buffer = (char *) calloc ((file_size + 1), sizeof (char));

    fread (buffer, sizeof (char), file_size, expression_file);
    return buffer;
}

bin_tree_t *create_diff_expr_tree (bin_tree_t *tree) {

    assert (tree);

#ifdef EXPR_TREE_AUTO_QUICK_VERIFICATION

    if (bin_tree_verify_qck (tree) != NULL) {

        printf ("\nExpression tree: differentiation autoverification failed, operation terminated\n");
        return NULL;
    }

#endif

    bin_tree_t *diff_tree = (bin_tree_t *) calloc (1, sizeof (bin_tree_t));
    bin_tree_ctor (diff_tree);

    if (tree->root == NULL) {

        diff_tree->root = NULL;

    } else {

        diff_tree->root = create_diff_branch (tree->root);
    }
    
    return diff_tree;
}

static bin_node_t *create_diff_branch (bin_node_t *node) {

    assert (node);

    bin_node_t *diff_node = NULL;

    switch (node->type) {

        case CONSTANT: {

            diff_node = bin_tree_create_leaf (0, CONSTANT);
            break;
        }

        case VARIABLE: {

            diff_node = bin_tree_create_leaf (1, CONSTANT);
            break;
        }

        case UNARY_OPERATION: {

            switch (node->data) {

                case PLUS: {

                    bin_node_t *diff_arg = create_diff_branch (node->left);                                 // Стоит ли для такой "почти дупликации" по 2 штуки выделять отдельные статик функции?
                    diff_node = bin_tree_create_node (PLUS, UNARY_OPERATION, diff_arg, NULL);

                    break;
                }

                case MINUS: {

                    bin_node_t *diff_arg = create_diff_branch (node->left);
                    diff_node = bin_tree_create_node (MINUS, UNARY_OPERATION, diff_arg, NULL);

                    break;
                }

                case SIN: {

                    bin_node_t *arg_copy = bin_tree_NO_AV_copy_branch (node->left);
                    bin_node_t *multiplier_1 = bin_tree_create_node (COS, UNARY_OPERATION, arg_copy, NULL);
                    bin_node_t *multiplier_2 = create_diff_branch (node->left);
                    diff_node = bin_tree_create_node (MULT, BINARY_OPERATION, multiplier_1, multiplier_2);

                    break;
                }

                case COS: {

                    bin_node_t *arg_copy = bin_tree_NO_AV_copy_branch (node->left);
                    bin_node_t *sin_arg_copy = bin_tree_create_node (SIN, UNARY_OPERATION, arg_copy, NULL);
                    bin_node_t *multiplier_1 = bin_tree_create_node (MINUS, UNARY_OPERATION, sin_arg_copy, NULL);
                    bin_node_t *multiplier_2 = create_diff_branch (node->left);
                    diff_node = bin_tree_create_node (MULT, BINARY_OPERATION, multiplier_1, multiplier_2);

                    break;
                }

                case TAN: {

                    bin_node_t *arg_copy = bin_tree_NO_AV_copy_branch (node->left);
                    bin_node_t *cos_arg_copy = bin_tree_create_node (COS, UNARY_OPERATION, arg_copy, NULL);
                    bin_node_t *val_eq_2 = bin_tree_create_leaf (2, CONSTANT);
                    bin_node_t *sq_cos = bin_tree_create_node (POW, BINARY_OPERATION, cos_arg_copy, val_eq_2);
                    bin_node_t *val_eq_1 = bin_tree_create_leaf (1, CONSTANT);
                    bin_node_t *multiplier_1 = bin_tree_create_node (DIV, BINARY_OPERATION, val_eq_1, sq_cos);
                    bin_node_t *multiplier_2 = create_diff_branch (node->left);
                    diff_node = bin_tree_create_node (MULT, BINARY_OPERATION, multiplier_1, multiplier_2);

                    break;
                }

                case COT: {

                    bin_node_t *arg_copy = bin_tree_NO_AV_copy_branch (node->left);
                    bin_node_t *sin_arg_copy = bin_tree_create_node (SIN, UNARY_OPERATION, arg_copy, NULL);
                    bin_node_t *val_eq_2 = bin_tree_create_leaf (2, CONSTANT);
                    bin_node_t *sq_sin = bin_tree_create_node (POW, BINARY_OPERATION, sin_arg_copy, val_eq_2);
                    bin_node_t *val_eq_1 = bin_tree_create_leaf (1, CONSTANT);
                    bin_node_t *frac_expr = bin_tree_create_node (DIV, BINARY_OPERATION, val_eq_1, sq_sin);
                    bin_node_t *multiplier_1 = bin_tree_create_node (MINUS, UNARY_OPERATION, frac_expr, NULL);
                    bin_node_t *multiplier_2 = create_diff_branch (node->left);
                    diff_node = bin_tree_create_node (MULT, BINARY_OPERATION, multiplier_1, multiplier_2);

                    break;
                }

                case ASIN: {

                    bin_node_t *arg_copy = bin_tree_NO_AV_copy_branch (node->left);
                    bin_node_t *val_eq_2_1 = bin_tree_create_leaf (2, CONSTANT);
                    bin_node_t *sq_arg_copy = bin_tree_create_node (POW, BINARY_OPERATION, arg_copy, val_eq_2_1);
                    bin_node_t *val_eq_1_1 = bin_tree_create_leaf (1, CONSTANT);
                    bin_node_t *one_min_sq = bin_tree_create_node (MINUS, BINARY_OPERATION, val_eq_1_1, sq_arg_copy);
                    bin_node_t *val_eq_1_2 = bin_tree_create_leaf (1, CONSTANT);
                    bin_node_t *val_eq_2_2 = bin_tree_create_leaf (2, CONSTANT);
                    bin_node_t *half = bin_tree_create_node (DIV, BINARY_OPERATION, val_eq_1_2, val_eq_2_2);
                    bin_node_t *sqrt_expr = bin_tree_create_node (POW, BINARY_OPERATION, one_min_sq, half);
                    bin_node_t *val_eq_1_3 = bin_tree_create_leaf (1, CONSTANT);
                    bin_node_t *multiplier_1 = bin_tree_create_node (DIV, BINARY_OPERATION, val_eq_1_3, sqrt_expr);
                    bin_node_t *multiplier_2 = create_diff_branch (node->left);
                    diff_node = bin_tree_create_node (MULT, BINARY_OPERATION, multiplier_1, multiplier_2);

                    break;
                }

                case ACOS: {

                    bin_node_t *arg_copy = bin_tree_NO_AV_copy_branch (node->left);
                    bin_node_t *val_eq_2_1 = bin_tree_create_leaf (2, CONSTANT);
                    bin_node_t *sq_arg_copy = bin_tree_create_node (POW, BINARY_OPERATION, arg_copy, val_eq_2_1);
                    bin_node_t *val_eq_1_1 = bin_tree_create_leaf (1, CONSTANT);
                    bin_node_t *one_min_sq = bin_tree_create_node (MINUS, BINARY_OPERATION, val_eq_1_1, sq_arg_copy);
                    bin_node_t *val_eq_1_2 = bin_tree_create_leaf (1, CONSTANT);
                    bin_node_t *val_eq_2_2 = bin_tree_create_leaf (2, CONSTANT);
                    bin_node_t *half = bin_tree_create_node (DIV, BINARY_OPERATION, val_eq_1_2, val_eq_2_2);
                    bin_node_t *sqrt_expr = bin_tree_create_node (POW, BINARY_OPERATION, one_min_sq, half);
                    bin_node_t *val_eq_1_3 = bin_tree_create_leaf (1, CONSTANT);
                    bin_node_t *frac_expr = bin_tree_create_node (DIV, BINARY_OPERATION, val_eq_1_3, sqrt_expr);
                    bin_node_t *multiplier_1 = bin_tree_create_node (MINUS, UNARY_OPERATION, frac_expr, NULL);
                    bin_node_t *multiplier_2 = create_diff_branch (node->left);
                    diff_node = bin_tree_create_node (MULT, BINARY_OPERATION, multiplier_1, multiplier_2);

                    break;
                }

                case ATAN: {

                    bin_node_t *arg_copy = bin_tree_NO_AV_copy_branch (node->left);
                    bin_node_t *val_eq_2 = bin_tree_create_leaf (2, CONSTANT);
                    bin_node_t *sq_arg_copy = bin_tree_create_node (POW, BINARY_OPERATION, arg_copy, val_eq_2);
                    bin_node_t *val_eq_1_1 = bin_tree_create_leaf (1, CONSTANT);
                    bin_node_t *one_pl_sq = bin_tree_create_node (PLUS, BINARY_OPERATION, val_eq_1_1, sq_arg_copy);
                    bin_node_t *val_eq_1_2 = bin_tree_create_leaf (1, CONSTANT);
                    bin_node_t *multiplier_1 = bin_tree_create_node (DIV, BINARY_OPERATION, val_eq_1_2, one_pl_sq);
                    bin_node_t *multiplier_2 = create_diff_branch (node->left);
                    diff_node = bin_tree_create_node (MULT, BINARY_OPERATION, multiplier_1, multiplier_2);

                    break;
                }

                case ACOT: {

                    bin_node_t *arg_copy = bin_tree_NO_AV_copy_branch (node->left);
                    bin_node_t *val_eq_2 = bin_tree_create_leaf (2, CONSTANT);
                    bin_node_t *sq_arg_copy = bin_tree_create_node (POW, BINARY_OPERATION, arg_copy, val_eq_2);
                    bin_node_t *val_eq_1_1 = bin_tree_create_leaf (1, CONSTANT);
                    bin_node_t *one_pl_sq = bin_tree_create_node (PLUS, BINARY_OPERATION, val_eq_1_1, sq_arg_copy);
                    bin_node_t *val_eq_1_2 = bin_tree_create_leaf (1, CONSTANT);
                    bin_node_t *frac_expr = bin_tree_create_node (DIV, BINARY_OPERATION, val_eq_1_2, one_pl_sq);
                    bin_node_t *multiplier_1 = bin_tree_create_node (MINUS, UNARY_OPERATION, frac_expr, NULL);
                    bin_node_t *multiplier_2 = create_diff_branch (node->left);
                    diff_node = bin_tree_create_node (MULT, BINARY_OPERATION, multiplier_1, multiplier_2);

                    break;
                }

                case LN: {

                    bin_node_t *arg_copy = bin_tree_NO_AV_copy_branch (node->left);
                    bin_node_t *val_eq_1 = bin_tree_create_leaf (1, CONSTANT);
                    bin_node_t *multiplier_1 = bin_tree_create_node (DIV, BINARY_OPERATION, val_eq_1, arg_copy);
                    bin_node_t *multiplier_2 = create_diff_branch (node->left);
                    diff_node = bin_tree_create_node (MULT, BINARY_OPERATION, multiplier_1, multiplier_2);

                    break;
                }

                case EXP: {

                    bin_node_t *multiplier_1 = bin_tree_NO_AV_copy_branch (node);
                    bin_node_t *multiplier_2 = create_diff_branch (node->left);
                    diff_node = bin_tree_create_node (MULT, BINARY_OPERATION, multiplier_1, multiplier_2);

                    break;
                }
            }

            break;
        }

        case BINARY_OPERATION: {

            switch (node->data) {

                case PLUS: {

                    bin_node_t *summand_1 = create_diff_branch (node->left);
                    bin_node_t *summand_2 = create_diff_branch (node->right);
                    diff_node = bin_tree_create_node (PLUS, BINARY_OPERATION, summand_1, summand_2);

                    break;
                }

                case MINUS: {

                    bin_node_t *minuend = create_diff_branch (node->left);
                    bin_node_t *subtrahend = create_diff_branch (node->right);
                    diff_node = bin_tree_create_node (MINUS, BINARY_OPERATION, minuend, subtrahend);

                    break;
                }

                case MULT: {

                    bin_node_t *arg_1_copy = bin_tree_NO_AV_copy_branch (node->left);
                    bin_node_t *arg_2_copy = bin_tree_NO_AV_copy_branch (node->right);
                    bin_node_t *arg_1_diff = create_diff_branch (node->left);
                    bin_node_t *arg_2_diff = create_diff_branch (node->right);
                    bin_node_t *summand_1 = bin_tree_create_node (MULT, BINARY_OPERATION, arg_1_copy, arg_2_diff);
                    bin_node_t *summand_2 = bin_tree_create_node (MULT, BINARY_OPERATION, arg_2_copy, arg_1_diff);
                    diff_node = bin_tree_create_node (PLUS, BINARY_OPERATION, summand_1, summand_2);

                    break;
                }

                case DIV: {

                    bin_node_t *divisible_copy = bin_tree_NO_AV_copy_branch (node->left);
                    bin_node_t *divisor_copy_1 = bin_tree_NO_AV_copy_branch (node->right);
                    bin_node_t *divisible_diff = create_diff_branch (node->left);
                    bin_node_t *divisor_diff = create_diff_branch (node->right);
                    bin_node_t *minuend = bin_tree_create_node (MULT, BINARY_OPERATION, divisible_diff, divisor_copy_1);
                    bin_node_t *subtrahend = bin_tree_create_node (MULT, BINARY_OPERATION, divisible_copy, divisor_diff);
                    bin_node_t *final_divisible = bin_tree_create_node (MINUS, BINARY_OPERATION, minuend, subtrahend);
                    bin_node_t *val_eq_2 = bin_tree_create_leaf (2, CONSTANT);
                    bin_node_t *divisor_copy_2 = bin_tree_NO_AV_copy_branch (node->right);
                    bin_node_t *final_divisor = bin_tree_create_node (POW, BINARY_OPERATION, divisor_copy_2, val_eq_2);
                    diff_node = bin_tree_create_node (DIV, BINARY_OPERATION, final_divisible, final_divisor);

                    break;
                }

                case POW: {

                    bin_node_t *basis_copy = bin_tree_NO_AV_copy_branch (node->left);
                    bin_node_t *power_copy = bin_tree_NO_AV_copy_branch (node->right);
                    bin_node_t *ln_basis_copy = bin_tree_create_node (LN, UNARY_OPERATION, basis_copy, NULL);
                    bin_node_t *ln_mult_power_copy = bin_tree_create_node (MULT, BINARY_OPERATION, ln_basis_copy, power_copy);
                    bin_node_t *exp_expr = bin_tree_create_node (EXP, UNARY_OPERATION, ln_mult_power_copy, NULL);
                    diff_node = create_diff_branch (exp_expr);

                    break;
                }

                case LOG: {

                    bin_node_t *basis_copy = bin_tree_NO_AV_copy_branch (node->left);
                    bin_node_t *arg_copy = bin_tree_NO_AV_copy_branch (node->right);
                    bin_node_t *ln_basis_copy = bin_tree_create_node (LN, UNARY_OPERATION, basis_copy, NULL);
                    bin_node_t *ln_arg_copy = bin_tree_create_node (LN, UNARY_OPERATION, arg_copy, NULL);
                    bin_node_t *div_expr = bin_tree_create_node (DIV, BINARY_OPERATION, ln_arg_copy, ln_basis_copy);
                    diff_node = create_diff_branch (div_expr);

                    break;
                }
            }

            break;
        }

        case DISTRUCTED: {                      // На случай компиляции в опасном режиме

            diff_node = bin_tree_create_node (0, DISTRUCTED, NULL, NULL);
        }
    }

    return diff_node;
}

void optimize_expr_tree (bin_tree_t *tree) {

    assert (tree);

#ifdef EXPR_TREE_AUTO_QUICK_VERIFICATION

    if (bin_tree_verify_qck (tree) != NULL) {

        printf ("\nExpression tree: differentiation autoverification failed, operation terminated\n");
        return;
    }

#endif

    if (tree->root == NULL) {

        return;

    } else {

        optimize_expr_branch (tree->root);
    }
}

static void optimize_expr_branch (bin_node_t *node) {

    assert (node);

    if (node->left == NULL) {

        return;

    } else if (node->right == NULL) {

        // TODO: вариаты оптимизации унарных функций

        optimize_expr_branch (node->left);
        return;
    }

    optimize_expr_branch (node->left);
    optimize_expr_branch (node->right);

    if (node->left->type == CONSTANT && node->right->type == CONSTANT) {

        switch (node->data) {

            case PLUS: {

                node->type = CONSTANT;
                node->data = node->left->data + node->right->data;
                bin_tree_free_node (node->right);
                bin_tree_free_node (node->left);
                node->right = node->left = NULL;

                return;
            }

            case MINUS: {

                node->type = CONSTANT;
                node->data = node->left->data - node->right->data;
                bin_tree_free_node (node->right);
                bin_tree_free_node (node->left);
                node->right = node->left = NULL;

                return;
            }

            case MULT: {

                node->type = CONSTANT;
                node->data = node->left->data * node->right->data;
                bin_tree_free_node (node->right);
                bin_tree_free_node (node->left);
                node->right = node->left = NULL;

                return;
            }

            case DIV: {

                if (node->right->data == 0) {

                    return;
                }

                long long gr_com_div = find_gcd (node->right->data, node->left->data);
                node->left->data = node->left->data / gr_com_div;
                node->right->data = node->right->data / gr_com_div;

                break;
            }

            case POW: {

                if (node->right->data < 0) {

                    node->data = DIV;
                    bin_node_t *divisible = bin_tree_create_leaf (1, CONSTANT);
                    node->right->data = -1 * node->right->data;
                    bin_node_t *divisor = bin_tree_create_node (POW, BINARY_OPERATION, node->left, node->right);
                    node->left = divisible;
                    node->right = divisor;

                    break;

                } else {

                    node->type = CONSTANT;
                    node->data = find_positive_pow (node->left->data, node->right->data);
                    bin_tree_free_node (node->right);
                    bin_tree_free_node (node->left);
                    node->right = node->left = NULL;

                    return;
                }
            }

            // TODO: варианты оптимизации логарифма
        }
    }

    // TODO: свертка переменных в простейших случаях

    if (node->right->type == CONSTANT) {

        if (node->right->data == 0) {                           // Тут тоже можноюыло свечу вколотить, но я подумал, что if будет нагляднее

            switch (node->data) {

                case PLUS:
                case MINUS: {

                    pull_up_left_branch (node);
                    return;
                }

                case MULT: {

                    suppress_left_branch_to_constant (node, 0);
                    return;
                }

                case POW: {

                    suppress_left_branch_to_constant (node, 1);
                    return;
                }
            }
        }

        if (node->right->data == 1) {

            if (node->data == LOG) {

                suppress_left_branch_to_constant (node, 0);
                return;
            }
            if (node->data == POW ||
                node->data == MULT ||
                node->data == DIV) {

                pull_up_left_branch (node);
                return;
            }
        }

        if (node->right->data == -1 &&
            (node->data == MULT ||
            node->data == DIV)) {

            bin_tree_free_node (node->right);
            node->right = NULL;
            node->data = MINUS;
            node->type = UNARY_OPERATION;

            return;
        }
    }

    if (node->left->type == CONSTANT) {

        if (node->left->data == 0) {

            switch (node->data) {

                case MULT:
                case DIV:
                case POW: {

                    suppress_right_branch_to_constant (node, 0);
                    return;
                }

                case PLUS: {

                    pull_up_right_branch (node);
                    return;
                }

                case MINUS: {

                    bin_tree_free_node (node->left);
                    node->left = node->right;
                    node->right = NULL;
                    node->type = UNARY_OPERATION;

                    return;
                }
            }
        }

        if (node->left->data == 1) {

            if (node->data == MULT) {

                pull_up_right_branch (node);
                return;
            }
            if (node->data == POW) {

                suppress_right_branch_to_constant (node, 1);
                return;
            }
        }

        if (node->left->data == -1 && node->data == MULT) {

            bin_tree_free_node (node->left);
            node->left = node->right;
            node->right = NULL;
            node->type = UNARY_OPERATION;
            node->data = MINUS;

            return;
        }
    }
}

static void suppress_left_branch_to_constant (bin_node_t *node, long long cnst) {

    bin_tree_free_branch (node->left);
    bin_tree_free_node (node->right);
    node->right = NULL;
    node->left = NULL;
    node->data = cnst;
    node->type = CONSTANT;
}

static void suppress_right_branch_to_constant (bin_node_t *node, long long cnst) {

    bin_tree_free_branch (node->right);
    bin_tree_free_node (node->left);
    node->right = NULL;
    node->left = NULL;
    node->data = cnst;
    node->type = CONSTANT;
}

static void pull_up_left_branch (bin_node_t *node) {

    node->data = node->left->data;
    node->type = node->left->type;
    bin_tree_free_node (node->right);
    bin_node_t *temp = node->left;
    node->left = temp->left;
    node->right = temp->right;
    bin_tree_free_node (temp);
}

static void pull_up_right_branch (bin_node_t *node) {

    node->data = node->right->data;
    node->type = node->right->type;
    bin_tree_free_node (node->left);
    bin_node_t *temp = node->right;
    node->left = temp->left;
    node->right = temp->right;
    bin_tree_free_node (temp);
}

static int func_secure_lrb (char **pptr) {

    if (NO_LEFT_ROUND_BRAC (pptr)) {

        return 0;

    } else {

        return 1;
    }
}

static int func_secure_rrb (char **pptr) {

    if (NO_RIGHT_ROUND_BRAC (pptr)) {

        return 0;
    }

    return 1;
}

static int func_secure_lsb (char **pptr) {

    if (NO_LEFT_SQUARE_BRAC (pptr)) {

        return 0;
    }

    return 1;
}

static int func_secure_rsb (char **pptr) {

    if (NO_RIGHT_SQUARE_BRAC (pptr)) {

        return 0;
    }
    
    return 1;
}

static int func_secure_uo (long long id) {

    if (UNKNOWN_OPERATION (id)) {

        return 0;
    }
    
    return 1;
}

static int func_secure_mse (char *ptr) {

    if (SEQUENCE_MAX_EXCEEDED (ptr)) {

        return 0;
    }

    return 1;
}

static int func_secure_pm (char **pptr) {

    if (NOT_A_UN_PLUS_OR_MINUS (pptr)) {

        return 0;
    }

    return 1;
}

#undef PRINT_ERROR
#undef NO_LEFT_ROUND_BRAC
#undef NO_RIGHT_ROUND_BRAC
#undef NO_LEFT_SQUARE_BRAC
#undef NO_RIGHT_SQUARE_BRAC
#undef UNKNOWN_OPERATION
#undef SEQUENCE_MAX_EXCEEDED
#undef NOT_A_UN_PLUS_OR_MINUS
