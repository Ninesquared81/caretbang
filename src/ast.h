#ifndef AST_H
#define AST_H

#include <stdlib.h>
#include "dynamic-array.h"

#define EMPTY_AST_NODE ((struct ast_node) {.type = CARET, .jump_index = 0})

#define ast_get_node(AST, INDEX) da_get(AST, sizeof(struct ast_node), INDEX)
#define ast_set_node(AST, NODE, INDEX) da_set(AST, NODE, sizeof NODE, INDEX)


struct ast_node {
  enum {
    ARROW_LEFT,
    ARROW_RIGHT,
    BANG,
    BKT_SQUARE_LEFT,
    BKT_SQUARE_RIGHT,
    CARET,
    COLON,
    COMMA,
    DOT,
    HASH,
    MINUS,
    PERCENT,
    PLUS,
    STAR,
    WHIRLPOOL,
  } type;
  size_t jump_index;
};

#endif
