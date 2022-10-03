#include <stdlib.h>

#include "stack.h"

struct data_stack_block *new_data_stack_block(struct data_stack *stack) {
  struct data_stack_block *block = malloc(sizeof *block);
  if (!block) return NULL; /* malloc() failed */

  block->prev = stack->top_block;
  if (stack->top_block) {
    block->next = stack->top_block->next;
    stack->top_block->next = block;
  }
  
  return block;
}

struct metastack_block *new_metastack_block(struct metastack *stack) {
  struct metastack_block *block = malloc(sizeof *block);
  if (!block) return NULL; /* malloc() failed */

  block->prev = stack->top_block;
  if (stack->top_block) {
    block->next = stack->top_block->next;
    stack->top_block->next = block;
  }
  
  return block;
}

struct bracket_stack_block *new_bracket_stack_block(struct bracket_stack *stack) {
  struct bracket_stack_block *block = malloc(sizeof *block);
  if (!block) return NULL; /* malloc() failed */

  block->prev = stack->top_block;
  if (stack->top_block) {
    block->next = stack->top_block->next;
    stack->top_block->next = block;
  }

  return block;
}

void destroy_data_stack(struct data_stack *stack) {
  struct data_stack_block *current = stack->start;
  
  struct data_stack_block *next;
  while (current) {
    next = current->next;
    free(current);
    current = next;
  }

  /* NULL dangling pointers */
  stack->start = NULL;
  stack->top_block = NULL;
  stack->top_index = 0;
}

void destory_metastack(struct metastack *stack) {
  struct metastack_block *current = stack->start;
  
  struct metastack_block *next;
  if (current) { /* not a guard clause since we still need to NULL dangling pointers */
    while (current->prev != stack->top_block) {
      next = current->next;
      /* stack might not be empty */
      destroy_data_stack(current);
      free(current);
      current = next;
    }
  }
  
  while (current) {
    /* could be stack blocks after 'top_block' (not freed until end to reduce malloc calls) */
    next = current->next;
    free(current);
    current = next;
  }

  /* NULL dangling pointers */
  stack->start = NULL;
  stack->top_block = NULL;
  stack->top_index = 0;
}

void destroy_bracket_stack(struct bracket_stack *stack) {
  struct bracket_stack_block *current = stack->start;
  
  struct bracket_stack_block *next;
  while (current) {
    next = current->next;
    free(current);
    current = next;
  }

  /* NULL dangling pointers */
  stack->start = NULL;
  stack->top_block = NULL;
  stack->top_inde = 0;
}

void push_data_stack(struct data_stack *stack, uint8_t element) {
  if (stack->top_index < DATA_STACK_BLOCK_SIZE - 1) {
    /* normal case */
    stack->top_block->elements[++stack->top_index] = element;
  }
  else {
    /* push to new stack block  */
    stack->top_index = 0;
    struct data_stack_block *next_block = stack->top_block->next;
    if (!next_block) {
      next_block = new_data_stack_block();
    }
    stack->top_block = next_block;
    next_block->elements[0] = element;
  }
}

void push_metastack(struct metastack *stack, struct data_stack data_stack) {
  if (stack->top_index < METASTACK_BLOCK_SIZE - 1) {
    stack->top_block->stacks[++stack_top_index] = data_stack;
  }
  else {
    stack->top_index = 0;
    struct data_stack_block *next_block = stack->top_block->next;
    if (!next_block) {
      next_block = new_metastack_block();
    }
    stack->top_block = next_block;
    next_block->stacks[0] = stack;
  }
}

void push_bracket_stack(struct bracket_stack *stack, enum bracket_type bracket) {
  if (stack->top_index < BRACKET_STACK_BLOCK_SIZE - 1) {
    stack->top_block->brackets[++stack->top_index] = bracket;
  }
  else {
    stack->top_index = 0;
    struct bracket_stack_block *next_block = stack->top_block->next;
    if (!next_block) {
      next_block = new_bracket_stack_block();
    }
    stack->top_block = next_block;
    next_block->brackets[0] = bracket;
  }
}

uint8_t pop_data_stack(struct data_stack *stack) {
  uint8_t element = stack->top_block->elements[stack->top_index];
  if (stack->top_index > 0) {
    /* normal case */
    stack->top_index--;
  }
  else {
    /* change block */
    stack->top_index = DATA_STACK_SIZE - 1;
    stack->top_block = stack->top_block->prev;
  }
  return element;
}

struct data_stack pop_metastack(struct metastack *stack) {
  struct data_stack data_stack = stack->top_block->stacks[stack->top_index];
  if (stack->top_index > 0) {
    /* normal case */
    stack->top_index--;
  }
  else {
    /* change block */
    stack->top_index = METASTACK_SIZE - 1;
    stack->top_block = stack->top_block->prev;
  }
  return data_stack;
}

enum bracket_type pop_bracket_stack(struct bracket_stack *stack) {
  enum bracket bracket = stack->top_block->brackets[stack->top_index];
  if (stack->top_index > 0) {
    /* normal case */
    stack->top_index--;
  }
  else {
    /* change block */
    stack->top_index = BRACKET_STACK_SIZE - 1;
    stack->top_block = stack->top_block->prev;
  }
  return bracket;
}
