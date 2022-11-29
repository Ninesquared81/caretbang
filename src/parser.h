#ifndef PARSER_H
#define PARSER_H

#include "ast.h"
#include "dstring.h"
#include "pos.h"


void parse(const struct dstring *source, struct ast_list *ast);
const char *parse_recursive(const char *source_start, const char *source_end,
			    struct ast_list *ast, struct pos *pos);
const char *parse_comment(const char *source_start, const char *source_end,
			  struct pos *pos);

#endif
