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
