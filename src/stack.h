#include <stdint.h>

#include "sizelims.h"


struct data_stack_block {
  struct data_stack_block *prev;
  struct data_stack_block *next;
  uint8_t elements[DATA_STACK_BLOCK_SIZE];
};


struct data_stack {
  struct {
    size_t instruction_start, instruction_end;
  } metadata;
  struct data_stack_block *start;
  struct data_stack_block *current_block;
  size_t top_index;
};


struct metastack_block {
  struct metastack_block *prev;
  struct netastack_block *next;
  struct data_stack stacks[METASTACK_BLOCK_SIZE];
};


struct metastack {
  struct metastack_block *start;
  struct metastack_block *current_block;
  size_t top_index;
};
