#ifndef STACK_H
#define STACK_H

#include <stdint.h>
#include <stdbool.h>

#include "sizelims.h"

#define IS_STACK_EMPTY(STACK) ((STACK).size == 0)

struct memory_info {
  void *first_block;
  size_t block_count;
};


struct data_stack_block {
  struct data_stack_block *prev;
  struct data_stack_block *next;
  uint8_t elements[DATA_STACK_BLOCK_SIZE];
};


struct data_stack {
  struct data_stack_block *top_block;
  size_t top_index;
  size_t size;
  struct memory_info meminfo;
};


struct metastack_block {
  struct metastack_block *prev;
  struct metastack_block *next;
  struct data_stack stacks[METASTACK_BLOCK_SIZE];
};


struct metastack {
  struct metastack_block *top_block;
  size_t top_index;
  size_t size;
  struct memory_info meminfo;
};

struct delim {
  enum {
    LOOP_START,
    STACK_START,
  } type;
  size_t index;
};

struct delim_stack_block {
  struct delim_stack_block *prev;
  struct delim_stack_block *next;
  struct delim delims[DELIM_STACK_BLOCK_SIZE];
};

struct delim_stack {
  struct delim_stack_block *top_block;
  size_t top_index;
  size_t size;
  struct memory_info meminfo;
};

struct data_stack_block *new_data_stack_block(struct data_stack *stack);
struct metastack_block *new_metastack_block(struct metastack *stack);
struct delim_stack_block *new_delim_stack_block(struct delim_stack *stack);

void destroy_data_stack(struct data_stack *stack);
void destroy_metastack(struct metastack *stack);
void destroy_delim_stack(struct delim_stack *stack);

void push_data_stack(struct data_stack *stack, uint8_t element);
void push_metastack(struct metastack *stack, struct data_stack data_stack);
void push_delim_stack(struct delim_stack *stack, struct delim delim);

uint8_t pop_data_stack(struct data_stack *stack);
struct data_stack pop_metastack(struct metastack *stack);
struct delim pop_delim_stack(struct delim_stack *stack);

bool find_delim_stack(struct delim_stack *stack, struct delim *delim);

#endif
