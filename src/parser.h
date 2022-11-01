#ifndef PARSER_H
#define PARSER_H

#include "ast.h"

size_t parse(char *source, struct ast_node ast[]);

#endif
