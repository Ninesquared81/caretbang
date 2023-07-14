#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <stdbool.h>

#include "ast.h"

void interpret(struct ast_list *ast, bool should_print_stacks);
void dump_stacks(void);

#endif
