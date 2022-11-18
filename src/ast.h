#ifndef AST_H
#define AST_H

#include <stddef.h>

#include "dynamic_array.h"


typedef struct ast_list {
  struct dynamic_array _darray;
} ast_list;

struct ast_simple_node {
  enum {
    AUX_TO_MAIN,
    MAIN_TO_AUX,
    INCREMENT,
    PUSH_ZERO,
    INPUT,
    PRINT,
    MINUS,
    PLUS,
    POP,
    DUPE,
    SWAP,
    WHIRLPOOL,
  } type;
};

struct ast_loop_node {
  ast_list body;
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
  size_t index_in_source;
};


void *init_ast_list(ast_list *ast);
void destroy_ast_list(ast_list *ast);

struct ast_node *get_ast_nodes(ast_list *ast);
size_t get_ast_size(ast_list *ast);

enum da_grow_rc grow_ast_list(ast_list *ast);


#endif
