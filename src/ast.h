#ifndef AST_H
#define AST_H

#include <stdlib.h>


struct ast_list {
  size_t size;
  size_t length;
  struct ast_node *nodes;
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
  struct ast_list body;
};

enum grow_ast_list_ret {
  GROW_SUCCESS,
  GROW_ALLOC_ERROR,
  GROW_SIZE_ERROR,
};

void *init_ast_list(struct ast_list *ast);
void destroy_ast_list(struct ast_list *ast);

enum grow_ast_list_ret grow_ast_list(struct ast_list *ast);


#endif
