#include "diftor.hpp"

#define FUNC_SECURITY(identificator, node, condition)                           \
    do {                                                                        \
                                                                                \
        if (condition(identificator)) {                                         \
                                                                                \
            printf ("Tree formation failed: impossible expression sequence");   \
            free (node);                                                        \
            return NULL;                                                        \
        }                                                                       \
    } while (0)

#define TREE_COLLAPSE(identificator, node)                                      \
    do {                                                                        \
                                                                                \
        if (identificator == NULL) {                                            \
                                                                                \
            free (node);                                                        \
            return NULL;                                                        \
        }                                                                       \
    } while (0)

#define LEFT_ROUND_BRAC(pptr) **pptr != '('
#define RIGHT_ROUND_BRAC(pptr) **pptr != ')'
#define LEFT_SQUARE_BRAC(pptr) **pptr != '['
#define RIGHT_SQUARE_BRAC(pptr) **pptr != ']'
#define UNKNOWN_OPERATION(identificator) identificator == -1
#define SEQUENCE_MAX_EXCEEDED(ptr) ptr == NULL

static bin_tree_t *init_expression_tree (char *expr_buffer);
static bin_node_t *scan_operation (char **expr_ptr);
static char *draw_symb_sequence (char *expr_ptr);
static char *download_buffer (FILE *expression_file);
static bin_node_t *create_diff_branch (bin_node_t *node);

bin_tree_t *init_expression_tree (char *expr_buffer) {

    bin_tree_t tree_ = {};
    bin_tree_t *tree = &tree_;
    bin_tree_ctor (tree);
    
    char *expr_ptr = expr_buffer;
    tree->root = scan_operation (&expr_ptr);
    if (tree->root == NULL) {

        return NULL;
    }

    return tree;
}

static bin_node_t *scan_operation (char **p_expr_ptr) {

    bin_node_t *node = (bin_node_t *) calloc (1, sizeof (bin_node_t));

    if (**p_expr_ptr != '(') {

        if (isdigit (**p_expr_ptr)) {

            node->data = atoll (*p_expr_ptr);
            *p_expr_ptr += num_of_digits_llint (node->data);

            FUNC_SECURITY (p_expr_ptr, node, RIGHT_ROUND_BRAC);
            *p_expr_ptr += 1;
            node->type = CONSTANT;
            node->left = NULL;
            node->right = NULL;
            return node;

        } else {

            char *symb_seq = draw_symb_sequence (p_expr_ptr);
            FUNC_SECURITY (symb_seq, node, SEQUENCE_MAX_EXCEEDED);

            if (symb_seq [0] == '\0') {

                free (symb_seq);
                if (**p_expr_ptr != '+' && **p_expr_ptr != '-') {

                    printf ("Tree formation failed: impossible expression sequence");
                    free (node);
                    return NULL;

                } else {

                    if (*(*p_expr_ptr)++ == '+') {

                        node->data = PLUS;

                    } else {

                        node->data = MINUS;
                    }
                    FUNC_SECURITY (p_expr_ptr, node, LEFT_ROUND_BRAC);
                    *p_expr_ptr += 1;

                    node->left = scan_operation (p_expr_ptr);
                    TREE_COLLAPSE (node->left, node);
                    node->right = NULL;
                    node->type = UNARY_OPERATION;
                }

                FUNC_SECURITY (p_expr_ptr, node, RIGHT_ROUND_BRAC);
                *p_expr_ptr += 1;

                return node;
            }
            if (symb_seq [1] == '\0') {

                node->data = (long long) symb_seq [0];
                free (symb_seq);

                FUNC_SECURITY (p_expr_ptr, node, RIGHT_ROUND_BRAC);
                *p_expr_ptr += 1;

                node->right = NULL;
                node->left = NULL;
                node->type = VARIABLE;
                return node;
            }
            
            node->data = str_lin_search (OPERATION_SYMBS, NUM_OF_UNARY_OPERATIONS + NUM_OF_BINARY_OPERATIONS, symb_seq);
            free (symb_seq);
            FUNC_SECURITY (node->data, node, UNKNOWN_OPERATION);
            FUNC_SECURITY (p_expr_ptr, node, LEFT_ROUND_BRAC);
            *p_expr_ptr += 1;

            if (node->data == LOG) {

                node->type = BINARY_OPERATION;
                FUNC_SECURITY (p_expr_ptr, node, LEFT_SQUARE_BRAC);
                *p_expr_ptr += 1;

                node->left = scan_operation (p_expr_ptr);
                TREE_COLLAPSE (node->left, node);
                FUNC_SECURITY (p_expr_ptr, node, RIGHT_SQUARE_BRAC);
                *p_expr_ptr += 1;

                FUNC_SECURITY (p_expr_ptr, node, LEFT_ROUND_BRAC);
                *p_expr_ptr += 1;

                node->right = scan_operation (p_expr_ptr);
                TREE_COLLAPSE (node->right, node);
                FUNC_SECURITY (p_expr_ptr, node, RIGHT_ROUND_BRAC);
                *p_expr_ptr += 1;

                return node;
            }

            node->type = UNARY_OPERATION;
            node->left = scan_operation (p_expr_ptr);
            TREE_COLLAPSE (node->left, node);
            node->right = NULL;

            FUNC_SECURITY (p_expr_ptr, node, RIGHT_ROUND_BRAC);
            *p_expr_ptr += 1;

            return node;
        }
    }

    node->type = BINARY_OPERATION;
    *p_expr_ptr += 1;

    node->left = scan_operation (p_expr_ptr);
    TREE_COLLAPSE (node->left, node);
    FUNC_SECURITY (p_expr_ptr, node, RIGHT_ROUND_BRAC);
    *p_expr_ptr += 1;

    node->data = first_symb_str_lin_search (OPERATION_SYMBS, NUM_OF_UNARY_OPERATIONS + NUM_OF_BINARY_OPERATIONS, *(*p_expr_ptr)++);
    FUNC_SECURITY (node->data, node, UNKNOWN_OPERATION);
    FUNC_SECURITY (p_expr_ptr, node, LEFT_ROUND_BRAC);
    *p_expr_ptr += 1;

    node->right = scan_operation (p_expr_ptr);
    TREE_COLLAPSE (node->right, node);
    FUNC_SECURITY (p_expr_ptr, node, RIGHT_ROUND_BRAC);
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
    char *buffer = (char *) malloc ((file_size + 1) * sizeof (char));

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

    bin_tree_t diff_tree_ = {};
    bin_tree_t *diff_tree = &diff_tree_;
    bin_tree_ctor (diff_tree);

    if (tree->root == NULL) {

        diff_tree->root = NULL;

    } else {

        diff_tree->root = create_diff_branch (tree->root);
    }
    
    return diff_tree;
}

static bin_node_t *create_diff_branch (bin_node_t *node) {               // Вопрос на будущее - в таком случае лучше сделать как я или избегать свитч в свитче, делая замысел кода менее очевидным?

    bin_node_t *diff_node = bin_tree_create_blank_node ();

    switch (node->type) {

        case CONSTANT: {

            diff_node->type = CONSTANT;
            diff_node->data = 0;
            diff_node->left = diff_node->right = NULL;

            break;
        }

        case VARIABLE: {

            diff_node->type = CONSTANT;
            diff_node->data = 1;
            diff_node->left = diff_node->right = NULL;

            break;
        }

        case UNARY_OPERATION: {

            switch (node->data) {

                case PLUS: {

                    diff_node->type = UNARY_OPERATION;
                    diff_node->data = PLUS;
                    diff_node->left = create_diff_branch (node->left);
                    diff_node->right = NULL;

                    break;
                }

                case MINUS: {

                    diff_node->type = UNARY_OPERATION;
                    diff_node->data = MINUS;
                    diff_node->left = create_diff_branch (node->left);
                    diff_node->right = NULL;

                    break;
                }

                case SIN: {

                    diff_node->type = BINARY_OPERATION;
                    diff_node->data = MULT;

                    bin_node_t *arg_copy = bin_tree_NO_AV_copy_branch (node->left);
                    diff_node->left = bin_tree_create_node (COS, UNARY_OPERATION, arg_copy, NULL)
                    diff_node->right = create_diff_branch (node->left);

                    break;
                }

                case COS: {

                    diff_node->type = BINARY_OPERATION;
                    diff_node->data = MULT;

                    bin_node_t *arg_copy = bin_tree_NO_AV_copy_branch (node->left);
                    bin_node_t *sin_arg_copy = bin_tree_create_node (SIN, UNARY_OPERATION, arg_copy, NULL);
                    diff_node->left = bin_tree_create_node (MINUS, UNARY_OPERATION, sin_arg_copy, NULL);
                    diff_node->right = create_diff_branch (node->left);

                    break;
                }

                case TAN: {

                    diff_node->type = BINARY_OPERATION;
                    diff_node->data = MULT;

                    bin_node_t *arg_copy = bin_tree_NO_AV_copy_branch (node->left);
                    bin_node_t *cos_arg_copy = bin_tree_create_node (COS, UNARY_OPERATION, arg_copy, NULL);
                    bin_node_t *val_eq_2 = bin_tree_create_leaf (2, CONSTANT);
                    bin_node_t *sq_cos = bin_tree_create_node (POW, BINARY_OPERATION, cos_arg_copy, val_eq_2);
                    bin_node_t *val_eq_1 = bin_tree_create_leaf (1, CONSTANT);
                    diff_node->left = bin_tree_create_node (DIV, BINARY_OPERATION, val_eq_1, sq_cos);
                    diff_node->right = create_diff_branch (node->left);

                    break;
                }

                case COT: {

                    diff_node->type = BINARY_OPERATION;
                    diff_node->data = MULT;

                    bin_node_t *arg_copy = bin_tree_NO_AV_copy_branch (node->left);
                    bin_node_t *sin_arg_copy = bin_tree_create_node (SIN, UNARY_OPERATION, arg_copy, NULL);
                    bin_node_t *val_eq_2 = bin_tree_create_leaf (2, CONSTANT);
                    bin_node_t *sq_sin = bin_tree_create_node (POW, BINARY_OPERATION, sin_arg_copy, val_eq_2);
                    bin_node_t *val_eq_1 = bin_tree_create_leaf (1, CONSTANT);
                    bin_node_t *frac_expr = bin_tree_create_node (DIV, BINARY_OPERATION, val_eq_1, sq_sin);
                    diff_node->left = bin_tree_create_node (MINUS, UNARY_OPERATION, frac_expr, NULL);
                    diff_node->right = create_diff_branch (node->left);

                    break;
                }

                case ASIN: {

                    diff_node->type = BINARY_OPERATION;
                    diff_node->data = MULT;

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
                    diff_node->left = bin_tree_create_node (DIV, BINARY_OPERATION, val_eq_1_3, sqrt_expr);
                    diff_node->right = create_diff_branch (node->left);

                    break;
                }

                case ACOS: {

                    diff_node->type = BINARY_OPERATION;
                    diff_node->data = MULT;

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
                    diff_node->left = bin_tree_create_node (MINUS, UNARY_OPERATION, frac_expr, NULL);
                    diff_node->right = create_diff_branch (node->left);

                    break;
                }

                case ATAN: {

                    diff_node->type = BINARY_OPERATION;
                    diff_node->data = MULT;

                    bin_node_t *arg_copy = bin_tree_NO_AV_copy_branch (node->left);
                    bin_node_t *val_eq_2 = bin_tree_create_leaf (2, CONSTANT);
                    bin_node_t *sq_arg_copy = bin_tree_create_node (POW, BINARY_OPERATION, arg_copy, val_eq_2);
                    bin_node_t *val_eq_1_1 = bin_tree_create_leaf (1, CONSTANT);
                    bin_node_t *one_pl_sq = bin_tree_create_node (PLUS, BINARY_OPERATION, val_eq_1_1, sq_arg_copy);
                    bin_node_t *val_eq_1_2 = bin_tree_create_leaf (1, CONSTANT);
                    diff_node->left = bin_tree_create_node (DIV, BINARY_OPERATION, val_eq_1_2, one_pl_sq);
                    diff_node->right = create_diff_branch (node->left);

                    break;
                }

                case ACOT: {

                    diff_node->type = BINARY_OPERATION;
                    diff_node->data = MULT;

                    bin_node_t *arg_copy = bin_tree_NO_AV_copy_branch (node->left);
                    bin_node_t *val_eq_2 = bin_tree_create_leaf (2, CONSTANT);
                    bin_node_t *sq_arg_copy = bin_tree_create_node (POW, BINARY_OPERATION, arg_copy, val_eq_2);
                    bin_node_t *val_eq_1_1 = bin_tree_create_leaf (1, CONSTANT);
                    bin_node_t *one_pl_sq = bin_tree_create_node (PLUS, BINARY_OPERATION, val_eq_1_1, sq_arg_copy);
                    bin_node_t *val_eq_1_2 = bin_tree_create_leaf (1, CONSTANT);
                    bin_node_t *frac_expr = bin_tree_create_node (DIV, BINARY_OPERATION, val_eq_1_2, one_pl_sq);
                    diff_node->left = bin_tree_create_node (MINUS, UNARY_OPERATION, frac_expr, NULL);
                    diff_node->right = create_diff_branch (node->left);

                    break;
                }

                case LN: {

                    diff_node->type = BINARY_OPERATION;
                    diff_node->data = MULT;

                    bin_node_t *arg_copy = bin_tree_NO_AV_copy_branch (node->left);
                    bin_node_t *val_eq_1 = bin_tree_create_leaf (1, CONSTANT);
                    diff_node->left = bin_tree_create_node (DIV, BINARY_OPERATION, val_eq_1, arg_copy);
                    diff_node->right = create_diff_branch (node->left);

                    break;
                }

                case EXP: {

                    diff_node->type = BINARY_OPERATION;
                    diff_node->data = MULT;
                    diff_node->left = bin_tree_NO_AV_copy_branch (node);
                    diff_node->right = create_diff_branch (node->left);

                    break;
                }
            }

            break;
        }

        case BINARY_OPERATION: {

            switch (node->data) {

                case PLUS: {

                    diff_node->type = BINARY_OPERATION;
                    diff_node->data = PLUS;
                    diff_node->left = create_diff_branch (node->left);
                    diff_node->right = create_diff_branch (node->right);

                    break;
                }

                case MINUS: {

                    diff_node->type = BINARY_OPERATION;
                    diff_node->data = MINUS;
                    diff_node->left = create_diff_branch (node->left);
                    diff_node->right = create_diff_branch (node->right);

                    break;
                }

                case MULT: {

                    diff_node->type = BINARY_OPERATION;
                    diff_node->data = PLUS;

                    bin_node_t *arg_1_copy = bin_tree_NO_AV_copy_branch (node->left);
                    bin_node_t *arg_2_copy = bin_tree_NO_AV_copy_branch (node->right);
                    bin_node_t *arg_1_diff = create_diff_branch (node->left);
                    bin_node_t *arg_2_diff = create_diff_branch (node->right);
                    diff_node->left = bin_tree_create_node (MULT, BINARY_OPERATION, arg_1_copy, arg_2_diff);
                    diff_node->right = bin_tree_create_node (MULT, BINARY_OPERATION, arg_2_copy, arg_1_diff);

                    break;
                }

                case DIV: {

                    diff_node->type = BINARY_OPERATION;
                    diff_node->data = DIV;

                    bin_node_t *arg_1_copy = bin_tree_NO_AV_copy_branch (node->left);
                    bin_node_t *arg_2_copy_1 = bin_tree_NO_AV_copy_branch (node->right);
                    bin_node_t *arg_1_diff = create_diff_branch (node->left);
                    bin_node_t *arg_2_diff = create_diff_branch (node->right);
                    bin_node_t *minuend = bin_tree_create_node (MULT, BINARY_OPERATION, arg_1_diff, arg_2_copy_1);
                    bin_node_t *subtrahend = bin_tree_create_node (MULT, BINARY_OPERATION, arg_1_copy, arg_2_diff);
                    diff_node->left = bin_tree_create_node (MINUS, BINARY_OPERATION, minuend, subtrahend);
                    bin_node_t *val_eq_2 = bin_tree_create_leaf (2, CONSTANT);
                    bin_node_t *arg_2_copy_2 = bin_tree_NO_AV_copy_branch (node->right);
                    diff_node->right = bin_tree_create_node (POW, BINARY_OPERATION, arg_2_copy_2, val_eq_2);

                    break;
                }

                case POW: {

                    

                    break;
                }

                case LOG: {


                    break;
                }
            }

            break;
        }
    }

    return diff_node;
}
