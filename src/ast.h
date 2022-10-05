#ifndef AST_H
#define AST_H

#include "token.h"


struct ast_node {
  struct token token;
  size_t jump_index;
};

#endif
