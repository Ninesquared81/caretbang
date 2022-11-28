#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "dstring.h"
#include "pos.h"


void parse(const struct dstring *source, struct ast_list *ast);
const char *parse_recursive(const char *source, const char *end,
			    struct ast_list *ast, struct pos *pos);


#endif
