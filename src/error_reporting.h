#ifndef ERROR_REPORTING_H
#define ERROR_REPORTING_H

#include "ast.h"

int empty_stack_error(struct ast_node *node);
int stack_size_error(struct ast_node *node);

#endif
