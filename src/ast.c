#include "ast.h"

#include <stdio.h>
#include <stdlib.h>

#include "dynamic_array.h"
#include "sizelims.h"
#include "debug.h"


int ast_human_readable_name(const struct ast_node *node, char buf[NODE_NAME_MAX]) {
    switch (node->tag) {
    case SIMPLE_NODE:
	return snprintf(buf, NODE_NAME_MAX, "%c", +node->sn.type);
    case LOOP_NODE:
	return snprintf(buf, NODE_NAME_MAX, "[ ... ]");
    default:
	exit(compiler_error("Inexhaustive handling of node->tag.\n"));
    }
}

void *init_ast_list(struct ast_list *ast) {
    return init_dynamic_array(&ast->darray, AST_LIST_INITIAL_SIZE);
}

void destroy_ast_list(struct ast_list *ast) {
    destroy_dynamic_array(&ast->darray);
}

enum da_grow_rc grow_ast_list(struct ast_list *ast) {
    return grow_dynamic_array(&ast->darray, AST_LIST_MAX_BYTES);
}

struct ast_node *get_ast_nodes(struct ast_list *ast) {
    return ast->darray.elements;
}

size_t get_ast_size_bytes(struct ast_list *ast) {
    return ast->darray.size;
}

size_t get_ast_size_logical(struct ast_list *ast) {
    return ast->darray.size / sizeof(struct ast_node);
}
