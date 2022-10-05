#ifndef SIZELIMS_H
#define SIZELIMS_H

#include <limits.h>
#include "token.h"

#define TOKENS_MAX UINT_MAX / sizeof(struct token)
#define AST_MAX TOKENS_MAX

#define DATA_STACK_BLOCK_SIZE 1024
#define METASTACK_BLOCK_SIZE 64
#define DELIM_STACK_BLOCK_SIZE METASTACK_BLOCK_SIZE

#endif
