#include "bin_tree.hpp"

static size_t vis_dump_node (bin_node_t *node, FILE *instr_file, size_t ident = 0);
static void form_dump_node (bin_node_t *node, FILE *instr_file, bool prev_bracket);
static bin_node_t *qck_node_verify (bin_node_t *node, VERIFICATION_CODES *ver_code);
static bin_node_t *slw_node_verify (bin_node_t *node, size_t *node_idx, bin_node_t **node_tbl, size_t *node_tbl_size, VERIFICATION_CODES *ver_code);
static bin_node_t *node_tbl_search (bin_node_t *node, size_t node_idx, bin_node_t **node_tbl);
static bin_node_t **node_tbl_resize (bin_node_t **node_tbl, size_t *node_tbl_size);

void bin_tree_ctor (bin_tree_t *tree) {

    assert (tree);

    tree->root = NULL;
    tree->keyhole = NULL;
    tree->keybranch = true;
}

void bin_tree_dtor (bin_tree_t *tree) {

    assert (tree);

#ifdef BIN_TREE_AUTO_QUICK_VERIFICATION

    if (bin_tree_verify_qck (tree) != NULL) {

        printf ("\nBinary tree: dtor autoverification failed, operation terminated\n");
        return;
    }

#endif

    bin_tree_free_branch (tree->root);
    tree->root = (bin_node_t *) BINTR_OS_RESERVED_ADDRESS;
    tree->keyhole = (bin_node_t *) BINTR_OS_RESERVED_ADDRESS;
}

void bin_tree_free_branch (bin_node_t *node) {

    if (node == NULL) {

        return;
    }

    bin_tree_free_branch (node->left);
    bin_tree_free_branch (node->right);

    bin_tree_free_node (node);
}

void bin_tree_free_node (bin_node_t *node) {

    assert (node);

    node->data = 0;
    node->right = (bin_node_t *) BINTR_OS_RESERVED_ADDRESS;
    node->left = (bin_node_t *) BINTR_OS_RESERVED_ADDRESS;
    node->type = DISTRUCTED;
    free (node);
}

bin_node_t *bin_tree_create_blank_node () {

    bin_node_t *node = (bin_node_t *) calloc (1, sizeof (bin_node_t));
    return node;
}

bin_node_t *bin_tree_create_node (long long data, NODE_TYPE type, bin_node_t *left, bin_node_t *right) {

    bin_node_t *node = bin_tree_create_blank_node ();
    node->data = data;
    node->type = type;
    node->left = left;
    node->right = right;

    return node;
}

bin_node_t *bin_tree_create_leaf (long long data, NODE_TYPE type) {

    bin_node_t *node = bin_tree_create_node (data, type, NULL, NULL);
    return node;
}

void bin_tree_vis_dump (bin_tree_t *tree, const char *file_name) {

    assert (tree);
    assert (file_name);

#ifdef BIN_TREE_AUTO_QUICK_VERIFICATION

    VERIFICATION_CODES ver_code = DEFAULT;
    if (bin_tree_verify_qck (tree, &ver_code) != NULL) {

        printf ("\nBinary tree: visual dump autoverification failed, operation terminated, ver_code %d\n", ver_code);
        return;
    }

#endif

    FILE *instr_file = fopen ("vis_instr.gv", "w");
    fprintf (instr_file, "digraph vis{\n\trankdir=HR;");
    vis_dump_node (tree->root, instr_file);
    fprintf (instr_file, "\n}");
    fclose (instr_file);

    char *cmd = (char *) calloc (MAX_FILE_NAME + 30, sizeof (char));
    sprintf (cmd, "dot -Tpng vis_instr.gv -o %s", file_name);
    system (cmd);
    free (cmd);
    system ("del vis_instr.gv");
}

static size_t vis_dump_node (bin_node_t *node, FILE *instr_file, size_t ident /* = 0 */) {      // Функция получает на вход НОМЕР текущего шага рекурсии (т.е. НАИМЕНЬШИЙ незанятый идентификатор), возвращает НАИБОЛЬШИЙ занятый идентификатор

    assert (node);
    assert (instr_file);

    switch (node->type) {

        case CONSTANT: {

            fprintf (instr_file, "\n\t%llu [shape=\"egg\",style=\"filled\",fillcolor=\"yellow\",label=\"%lld\"];", ident, node->data);
            return ident;
        }

        case VARIABLE: {

            fprintf (instr_file, "\n\t%llu [shape=\"egg\",style=\"filled\",fillcolor=\"orange\",label=\"%c\"];", ident, (int) node->data);
            return ident;
        }

        case UNARY_OPERATION: {

            fprintf (instr_file, "\n\t%llu [shape=record,label=\" { <dat> %s | { <left> left | <right> --- }} \"];", ident, OPERATION_SYMBS [node->data]);

            size_t ident_2 = vis_dump_node (node->left, instr_file, ident + 1);
            fprintf (instr_file, "\n\t%llu:<left> -> %llu:<dat>;", ident, ident + 1);

            return ident_2 + 1;
        }

        case BINARY_OPERATION: {

            fprintf (instr_file, "\n\t%llu [shape=record,label=\" { <dat> %s | { <left> left | <right> right }} \"];", ident, OPERATION_SYMBS [node->data]);

            size_t ident_2 = vis_dump_node (node->left, instr_file, ident + 1);
            size_t ident_3 = vis_dump_node (node->right, instr_file, ident_2 + 1);
            fprintf (instr_file, "\n\t%llu:<left> -> %llu;", ident, ident + 1);
            fprintf (instr_file, "\n\t%llu:<right> -> %llu;", ident, ident_2 + 1);

            return ident_3;
        }

        case DISTRUCTED: {                      // На случай компиляции в опасном режиме

            fprintf (instr_file, "\n\t%llu [shape=\"round\",style=\"filled\",fillcolor=\"red\",label=\" { <dat> DISTRUCTED } \"];", ident);
            return ident;
        }
    }

    return 0;
}

void bin_tree_form_dump (bin_tree_t *tree, const char *file_name) {

    assert (tree);
    assert (file_name);

#ifdef BIN_TREE_AUTO_QUICK_VERIFICATION

    if (bin_tree_verify_qck (tree) != NULL) {

        printf ("\nBinary tree: formula dump autoverification failed, operation terminated\n");
        return;
    }

#endif

    FILE *instr_file = fopen ("form_instr.tex", "w");
    fprintf (instr_file, "\\documentclass{minimal}\n\\usepackage{mathtools}\n\\begin{document}\n\\begin{displaymath}\n");
    form_dump_node (tree->root, instr_file, true);
    fprintf (instr_file, "\n\\end{displaymath}\n\\end{document}\n");
    fclose (instr_file);

    char *ren_cmd = (char *) calloc (MAX_FILE_NAME + 24, sizeof (char));
    sprintf (ren_cmd, "ren form_instr.pdf %s", file_name);

    system ("latex form_instr.tex");
    system ("dvipdfm form_instr.dvi");
    system ("del form_instr.dvi form_instr.aux form_instr.log form_instr.tex");
    system (ren_cmd);
    free (ren_cmd);
}

static void form_dump_node (bin_node_t *node, FILE *instr_file, bool prev_bracket) {

    assert (node);
    assert (instr_file);

    switch (node->type) {

        case CONSTANT: {

            if (node->data < 0 && prev_bracket == false) {

                fprintf (instr_file, "(%lld)", node->data);

            } else {

                fprintf (instr_file, "%lld", node->data);
            }
            return;
        }

        case VARIABLE: {

            fprintf (instr_file, "%c", (int) node->data);
            return;
        }

        case UNARY_OPERATION: {

            bool plus_or_minus = false, int_brackets = true;
            if (node->data == PLUS || node->data == MINUS) {

                plus_or_minus = true;
            }
            if (node->left->type == VARIABLE || node->left->type == CONSTANT ||
                (plus_or_minus && (node->left->type != BINARY_OPERATION ||
                (node->left->data != PLUS && node->left->data != MINUS)))) {

                int_brackets = false;
            }

            if (plus_or_minus) {

                if (prev_bracket == false) {
                    
                    fprintf (instr_file, "(");
                }
            } else {

                fprintf (instr_file, "\\");
            }
            fprintf (instr_file, "%s", OPERATION_SYMBS [node->data]);
            if (int_brackets) {

                fprintf (instr_file, "(");
            }
            form_dump_node (node->left, instr_file, int_brackets);
            if (int_brackets) {

                fprintf (instr_file, ")");
            }
            if (plus_or_minus && prev_bracket == false) {

                fprintf (instr_file, ")");
            }

            return;
        }

        case BINARY_OPERATION: {

            switch (node->data) {

                case PLUS: {

                    form_dump_node (node->left, instr_file, prev_bracket);
                    fprintf (instr_file, "+");
                    form_dump_node (node->right, instr_file, false);

                    return;
                }

                case MINUS: {

                    form_dump_node (node->left, instr_file, prev_bracket);
                    fprintf (instr_file, "-");
                    if (node->right->type == BINARY_OPERATION &&
                        (node->right->data == PLUS || node->right->data == MINUS)) {

                        fprintf (instr_file, "(");
                        form_dump_node (node->right, instr_file, true);
                        fprintf (instr_file, ")");

                    } else {

                        form_dump_node (node->right, instr_file, false);
                    }

                    return;
                }

                case MULT: {

                    bool left_int_brackets = false, right_int_brackets = false;
                    if (node->left->type == BINARY_OPERATION &&
                        (node->left->data == PLUS || node->left->data == MINUS)) {

                        left_int_brackets = true;
                    }
                    if (node->right->type == BINARY_OPERATION &&
                        (node->right->data == PLUS || node->right->data == MINUS)) {

                        right_int_brackets = true;
                    }

                    if (left_int_brackets) {

                        fprintf (instr_file, "(");
                        form_dump_node (node->left, instr_file, true);
                        fprintf (instr_file, ")");

                    } else {

                        form_dump_node (node->left, instr_file, prev_bracket);
                    }

                    if (right_int_brackets == false && node->right->type != VARIABLE) {

                        fprintf (instr_file, "\\cdot");
                    }
                    
                    if (right_int_brackets) {

                        fprintf (instr_file, "(");
                        form_dump_node (node->right, instr_file, true);
                        fprintf (instr_file, ")");

                    } else {

                        form_dump_node (node->right, instr_file, false);
                    }

                    return;
                }

                case DIV: {

                    fprintf (instr_file, "\\frac{");
                    form_dump_node (node->left, instr_file, true);
                    fprintf (instr_file, "}{");
                    form_dump_node (node->right, instr_file, true);
                    fprintf (instr_file, "}");

                    return;
                }

                case POW: {

                    // TODO: хэндлер частного случая с применимостью записи радикалом

                    if (node->left->type == VARIABLE || node->left->type == CONSTANT) {

                        form_dump_node (node->left, instr_file, false);

                    } else {

                        fprintf (instr_file, "(");
                        form_dump_node (node->left, instr_file, true);
                        fprintf (instr_file, ")");
                    }

                    fprintf (instr_file, "^{");
                    form_dump_node (node->right, instr_file, true);
                    fprintf (instr_file, "}");

                    return;
                }

                case LOG: {

                    fprintf (instr_file, "\\log_{");
                    form_dump_node (node->left, instr_file, true);
                    
                    if (node->right->type == VARIABLE || node->right->type == CONSTANT) {

                        fprintf (instr_file, "}");
                        form_dump_node (node->right, instr_file, false);

                    } else {

                        fprintf (instr_file, "}(");
                        form_dump_node (node->right, instr_file, true);
                        fprintf (instr_file, ")");
                    }

                    return;
                }
            }

            return;                                         // Чтобы компидятор не ругался - все равно эта строчка "недосягаема" 
        }

        case DISTRUCTED: {                                  // На случай компиляции в опасном режиме

            fprintf (instr_file, "__DISTRUCTED__");
            return;
        }
    }
}

bin_node_t *bin_tree_verify_qck (bin_tree_t *tree, VERIFICATION_CODES *ver_code /* = NULL */) {

    assert (tree);

    if (tree->root == NULL && tree->keyhole != NULL) {

        if (ver_code) {

            *ver_code = EXTERNAL_KEYHOLE;
        }

        return tree->keyhole;
    }

    if (ver_code) {

        *ver_code = DEFAULT;
    }
    return qck_node_verify (tree->root, ver_code);
}

static bin_node_t *qck_node_verify (bin_node_t *node, VERIFICATION_CODES *ver_code) {

    if (node == NULL) {

        return NULL;
    }

    switch (node->type) {

        case DISTRUCTED: {

            if (ver_code) {

                *ver_code = NODE_DISTRUCTED;
            }

            return node;
        }

        case UNARY_OPERATION: {

            if (node->left == NULL || node->right != NULL) {

                if (ver_code) {

                    *ver_code = UN_OP_BRANCH_FAULT;
                }
                return node;
            }
            if (int_lin_search (UNARY_OPERATIONS, NUM_OF_UNARY_OPERATIONS, node->data) == -1) {

                if (ver_code) {

                    *ver_code = UN_OP_TYPE_FAULT;
                }
                return node;
            }

            break;
        }

        case BINARY_OPERATION: {

            if (node->left == NULL || node->right == NULL) {

                if (ver_code) {

                    *ver_code = BIN_OP_BRANCH_FAULT;
                }
                return node;
            }
            if (int_lin_search (BINARY_OPERATIONS, NUM_OF_BINARY_OPERATIONS, node->data) == -1) {

                if (ver_code) {

                    *ver_code = BIN_OP_TYPE_FAULT;
                }
                return node;
            }

            break;
        }

        case CONSTANT: {

            if (node->left != NULL || node->right != NULL) {

                if (ver_code) {

                    *ver_code = CONST_BRANCH_FAULT;
                }
                return node;
            }

            break;
        }

        case VARIABLE: {

            if (node->left != NULL || node->right != NULL) {

                if (ver_code) {

                    *ver_code = VAR_BRANCH_FAULT;
                }
                return node;
            }

            break;
        }
    }

    bin_node_t *right_check = qck_node_verify (node->right, ver_code);
    if (right_check != NULL) {

        return right_check;
    }

    return qck_node_verify (node->left, ver_code);
}

bin_node_t *bin_tree_verify_slw (bin_tree_t *tree, VERIFICATION_CODES *ver_code /* = NULL */) {

    assert (tree);

    if (tree->root == NULL && tree->keyhole != NULL) {

        if (ver_code) {

            *ver_code = EXTERNAL_KEYHOLE;
        }

        return tree->keyhole;
    }

    bin_node_t **node_tbl = (bin_node_t **) calloc (DEFAULT_NODE_TBL_SIZE, sizeof (bin_node_t *));
    size_t node_tbl_size = DEFAULT_NODE_TBL_SIZE;
    size_t node_idx = 0;
    if (ver_code) {

        *ver_code = DEFAULT;
    }

    bin_node_t *res = slw_node_verify (tree->root, &node_idx, node_tbl, &node_tbl_size, ver_code);
    
    free (node_tbl);
    return res;
}

static bin_node_t *slw_node_verify (bin_node_t *node, size_t *node_idx, bin_node_t **node_tbl, size_t *node_tbl_size, VERIFICATION_CODES *ver_code) {

    assert (node_tbl);
    assert (node_idx);
    assert (node_tbl_size);

    if (node == NULL) {

        return NULL;
    }

    switch (node->type) {

        case DISTRUCTED: {

            if (ver_code) {

                *ver_code = NODE_DISTRUCTED;
            }

            return node;
        }

        case UNARY_OPERATION: {

            if (node->left == NULL || node->right != NULL) {

                if (ver_code) {

                    *ver_code = UN_OP_BRANCH_FAULT;
                }
                return node;
            }
            if (int_lin_search (UNARY_OPERATIONS, NUM_OF_UNARY_OPERATIONS, node->data) == -1) {

                if (ver_code) {

                    *ver_code = UN_OP_TYPE_FAULT;
                }
                return node;
            }

            break;
        }

        case BINARY_OPERATION: {

            if (node->left == NULL || node->right == NULL) {

                if (ver_code) {

                    *ver_code = BIN_OP_BRANCH_FAULT;
                }
                return node;
            }
            if (int_lin_search (BINARY_OPERATIONS, NUM_OF_BINARY_OPERATIONS, node->data) == -1) {

                if (ver_code) {

                    *ver_code = BIN_OP_TYPE_FAULT;
                }
                return node;
            }

            break;
        }

        case CONSTANT: {

            if (node->left != NULL || node->right != NULL) {

                if (ver_code) {

                    *ver_code = CONST_BRANCH_FAULT;
                }
                return node;
            }

            break;
        }

        case VARIABLE: {

            if (node->left != NULL || node->right != NULL) {

                if (ver_code) {

                    *ver_code = VAR_BRANCH_FAULT;
                }
                return node;
            }

            break;
        }
    }
    if (node_tbl_search (node, *node_idx, node_tbl)) {

        if (ver_code) {

            *ver_code = CYCLED;
        }
        return node;
    }

    if (*node_idx == *node_tbl_size) {

        node_tbl = node_tbl_resize (node_tbl, node_tbl_size);
    }

    node_tbl [(*node_idx) ++] = node;

    bin_node_t *right_check = slw_node_verify (node->right, node_idx, node_tbl, node_tbl_size, ver_code);
    if (right_check != NULL) {

        return right_check;
    }

    return slw_node_verify (node->left, node_idx, node_tbl, node_tbl_size, ver_code);
}

static bin_node_t *node_tbl_search (bin_node_t *node, size_t node_idx, bin_node_t **node_tbl) {

    for (size_t i = 0; i < node_idx; ++ i) {

        if (node_tbl [i] == node) {
            
            return node_tbl [i];
        }
    }

    return NULL;
}

static bin_node_t **node_tbl_resize (bin_node_t **node_tbl, size_t *node_tbl_size) {

    *node_tbl_size *= 2;
    return ((bin_node_t **) realloc (node_tbl, *node_tbl_size));
}

bin_tree_t *bin_tree_extract_branch (bin_tree_t *tree) {

    assert (tree);

#ifdef BIN_TREE_AUTO_QUICK_VERIFICATION

    if (bin_tree_verify_qck (tree) != NULL) {

        printf ("\nBinary tree: branch copying autoverification failed, operation terminated\n");
        return NULL;
    }

#endif

    bin_tree_t extr_tree_ = {};
    bin_tree_t *extr_tree = &extr_tree_;
    bin_tree_ctor (extr_tree);

    extr_tree->root = bin_tree_NO_AV_copy_branch (tree->keyhole);
    return extr_tree;
}

bin_node_t *bin_tree_NO_AV_copy_branch (bin_node_t *node) {

    if (node == NULL) {

        return NULL;
    }

    bin_node_t *node_copy = bin_tree_create_node (node->data, node->type,
    bin_tree_NO_AV_copy_branch (node->left), bin_tree_NO_AV_copy_branch (node->right));
    
    return node_copy;
}
