#ifndef SIZELIMS_H
#define SIZELIMS_H

#include <limits.h>  // UINT_MAX
#include <stddef.h>  // size_t


#define AST_LIST_MAX_BYTES (size_t)UINT_MAX
#define AST_LIST_INITIAL_SIZE (size_t)256u

#define DSTRING_MAX_BYTES (size_t)UINT_MAX
#define DSTRING_INITIAL_SIZE (size_t)1024u

#define DATA_STACK_BLOCK_SIZE (size_t)2048u
#define DELIM_STACK_BLOCK_SIZE (size_t)64u

#define NODE_NAME_MAX (size_t)16u

#endif
