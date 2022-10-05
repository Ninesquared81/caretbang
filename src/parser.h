#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "ast.h"

size_t parse(struct token tokens[], size_t length, struct ast_node ast[]);

#endif
