#include <stdint.h>

#include "sizelims.h"


struct data_stack_block {
  struct data_stack_block *prev;
  struct data_stack_block *next;
  uint8_t elements[DATA_STACK_BLOCK_SIZE];
};


struct data_stack {
  struct data_stack_block *start;
  struct data_stack_block *top_block;
  size_t top_index;
};


struct metastack_block {
  struct metastack_block *prev;
  struct metastack_block *next;
  struct data_stack stacks[METASTACK_BLOCK_SIZE];
};


struct metastack {
  struct metastack_block *start;
  struct metastack_block *top_block;
  size_t top_index;
};

struct symbol {
  enum {
    SYM_T_CURLY,
    SYM_T_SQUARE,
    SYM_T_HASH,
  } type;
  size_t index;
};

struct symbol_stack_block {
  struct symbol_stack_block *prev;
  struct symbol_stack_block *next;
  struct symbol symbols[SYMBOL_STACK_BLOCK_SIZE];
};

struct symbol_stack {
  struct symbol_stack_block *start;
  struct symbol_stack_block *top_block;
  size_t top_index;
};

struct data_stack_block *new_data_stack_block(struct data_stack *stack);
struct metastack_block *new_metastack_block(struct metastack *stack);
struct symbol_stack_block *new_symbol_stack_block(struct symbol_stack *stack);

void destroy_data_stack(struct data_stack *stack);
void destroy_metastack(struct metastack *stack);
void destroy_symbol_stack(struct symbol_stack *stack);

void push_data_stack(struct data_stack *stack, uint8_t element);
void push_metastack(struct metastack *stack, struct data_stack data_stack);
void push_symbol_stack(struct symbol_stack *stack, struct symbol bracket);

uint8_t pop_data_stack(struct data_stack *stack);
struct data_stack pop_metastack(struct metastack *stack);
struct symbol pop_symbol_stack(struct symbol_stack *stack);
