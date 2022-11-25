#ifndef REPORTING_H
#define REPORTING_H

#include "ast.h"

int report_location(const struct ast_node *node);
int parse_error(const struct ast_node *node, const char *restrict message, ...);

int empty_stack_error(const struct ast_node *node);
int stack_size_error(const struct ast_node *node, size_t expected_size, size_t actual_size);

#endif
