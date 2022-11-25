#ifndef SIZELIMS_H
#define SIZELIMS_H

#include <limits.h>  // SIZE_MAX
#include <stddef.h>  // size_t


#define AST_LIST_MAX_BYTES (size_t)SIZE_MAX
#define AST_LIST_INITIAL_SIZE (size_t)64

#define DSTRING_MAX_BYTES (size_t)SIZE_MAX
#define DSTRING_INITIAL_SIZE (size_t)1024

#define DATA_STACK_BLOCK_SIZE (size_t)2048
#define DELIM_STACK_BLOCK_SIZE (size_t)64

#define NODE_NAME_MAX (size_t)32

#endif
