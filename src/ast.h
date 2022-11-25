#ifndef AST_H
#define AST_H

#include <stddef.h>

#include "dynamic_array.h"
#include "sizelims.h"


struct ast_list {
    struct dynamic_array darray;
    size_t length;
};

struct ast_simple_node {
    enum {
	AUX_TO_MAIN	= '<',
	MAIN_TO_AUX	= '>',
	INCREMENT	= '!',
	PUSH_ZERO	= '^',
	DUPE		= ':',
	INPUT		= ',',
	PRINT		= '.',
	MINUS		= '-',
	PLUS		= '+',
	POP		= '*',
	SWAP		= '%',
	DISCOVER	= '@',
    } type;
};

struct ast_loop_node {
    struct ast_list body;
};

struct ast_node {
    enum {
	SIMPLE_NODE,
	LOOP_NODE,
    } tag;
    union {
	struct ast_simple_node sn;
	struct ast_loop_node ln;
    };
    struct {
	size_t row;
	size_t col;
    };
};

int ast_human_readable_name(const struct ast_node *node, char buf[NODE_NAME_MAX]);

void *init_ast_list(struct ast_list *ast);
void destroy_ast_list(struct ast_list *ast);

enum da_grow_rc grow_ast_list(struct ast_list *ast);

struct ast_node *get_ast_nodes(struct ast_list *ast);
size_t get_ast_size(struct ast_list *ast);

#endif
