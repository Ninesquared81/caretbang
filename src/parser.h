#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "dstring.h"


void parse(const struct dstring *restrict source, struct ast_list *restrict ast);
const char *parse_recursive(const char *restrict source, const char *restrict end,
			    struct ast_list *restrict ast);


#endif
