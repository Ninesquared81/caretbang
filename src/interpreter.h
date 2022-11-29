#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"

void interpret(struct ast_list *ast);
void dump_stacks(void);

#endif
