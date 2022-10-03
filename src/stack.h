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

enum bracket_type {
  BKT_T_CURLY,
  BKT_T_SQUARE,
  BKT_T_NONE
};

struct bracket_stack_block {
  struct bracket_stack_block *prev;
  struct bracket_stack_block *next;
  enum bracket_type brackets[BRACKET_STACK_BLOCK_SIZE];
};

struct bracket_stack {
  struct bracket_stack_block *start;
  struct bracket_stack_block *top_block;
  size_t top_index;
};

struct data_stack_block *new_data_stack_block(struct data_stack *stack);
struct metastack_block *new_metastack_block(struct metastack *stack);
struct bracket_stack_block *new_bracket_stack_block(struct bracket_stack *stack);

void destroy_data_stack(struct data_stack *stack);
void destory_metastack(struct metastack *stack);
void destroy_bracket_stack(struct bracket_stack *stack);

void push_data_stack(struct data_stack *stack, uint8_t element);
void push_metastack(struct metastack *stack, struct data_stack data_stack);
void push_bracket_stack(struct bracket_stack *stack, enum bracket_type bracket);

uint8_t pop_data_stack(struct data_stack *stack);
struct data_stack pop_metastack(struct metastack *stack);
enum bracket_type pop_bracket_stack(struct bracket_stack *stack);
