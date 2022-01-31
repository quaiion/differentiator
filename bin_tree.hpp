#ifndef BIN_TREE_ACTIVE
#define BIN_TREE_ACTIVE

#define BIN_TREE_AUTO_QUICK_VERIFICATION

#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include "bin_node.hpp"
#include "common.hpp"

#define BINTR_STRINGS_EQUAL 0
#define BINTR_OS_RESERVED_ADDRESS 1

constexpr size_t DEFAULT_NODE_TBL_SIZE = 16;

struct bin_tree_t {

    bin_node_t *root;
    bin_node_t *keyhole;
    bool keybranch;
};

void bin_tree_ctor (bin_tree_t *tree);
void bin_tree_dtor (bin_tree_t *tree);

void bin_tree_free_node (bin_node_t *node);
void bin_tree_free_branch (bin_node_t *node);

enum VERIFICATION_CODES {DEFAULT, CYCLED, NODE_DISTRUCTED, UN_OP_BRANCH_FAULT, BIN_OP_BRANCH_FAULT,
                         CONST_BRANCH_FAULT, VAR_BRANCH_FAULT, UN_OP_TYPE_FAULT, BIN_OP_TYPE_FAULT, 
                         EXTERNAL_KEYHOLE};
bin_node_t *bin_tree_verify_qck (bin_tree_t *tree, VERIFICATION_CODES *ver_code = NULL);
bin_node_t *bin_tree_verify_slw (bin_tree_t *tree, VERIFICATION_CODES *ver_code = NULL);

void bin_tree_vis_dump (bin_tree_t *tree, char *file_name);
void bin_tree_form_dump (bin_tree_t *tree, char *file_name);

bin_node_t *bin_tree_create_blank_node ();
bin_node_t *bin_tree_create_node (long long data, NODE_TYPE type, bin_node_t *left, bin_node_t *right);
bin_node_t *bin_tree_create_leaf (long long data, NODE_TYPE type);

bin_tree_t *bin_tree_extract_branch (bin_tree_t *tree);
bin_node_t *bin_tree_NO_AV_copy_branch (bin_node_t *node);

#endif
