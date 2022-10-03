#ifndef SIZELIMS_H
#define SIZELIMS_H

#include <limits.h>
#include "token.h"

#define TOKENS_MAX UINT_MAX / 16
#define DATA_STACK_BLOCK_SIZE 1024
#define METASTACK_BLOCK_SIZE 64
#define BRACKET_STACK_BLOCK_SIZE METASTACK_BLOCK_SIZE * 2 * 2

#endif
