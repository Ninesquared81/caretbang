#include <stdlib.h>
#include <stdbool.h>

#include "stack.h"
#define printf __mingw_printf

void *init_data_stack(struct data_stack *stack) {
  struct data_stack_block *first_block = malloc(sizeof *first_block);
  stack->top_block = NULL;
  stack->top_index = NULL;
  stack->size = 0;
  stack->meminfo.first_block = first_block;
  stack->meminfo.block_count = 1;
  return first_block;
}

void *init_metastack(struct metastack *stack) {
  struct metastack_block *first_block = malloc(sizeof *first_block);
  stack->top_block = NULL;
  stack->top_index = NULL;
  stack->size = 0;
  stack->meminfo.first_block = first_block;
  stack->meminfo.block_count = 1;  
  return first_block;
}

void *init_delim_stack(struct delim_stack *stack) {
  struct delim_stack_block *first_block = malloc(sizeof *first_block);
  stack->top_block = NULL;
  stack->top_index = NULL;
  stack->size = 0;
  stack->meminfo.first_block = first_block;
  stack->meminfo.block_count = 1;  
  return first_block;
}


struct data_stack_block *new_data_stack_block(struct data_stack *stack) {
  struct data_stack_block *block = malloc(sizeof *block);
  if (!block) return NULL; /* malloc() failed */

  block->prev = stack->top_block;
  if (stack->top_block) {
    block->next = stack->top_block->next;
    stack->top_block->next = block;
  }
  ++stack->meminfo.block_count;
  
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
  ++stack->meminfo.block_count;
  
  return block;
}

struct delim_stack_block *new_delim_stack_block(struct delim_stack *stack) {
  struct delim_stack_block *block = malloc(sizeof *block);
  if (!block) return NULL; /* malloc() failed */

  block->prev = stack->top_block;
  if (stack->top_block) {
    block->next = stack->top_block->next;
    stack->top_block->next = block;
  }
  ++stack->meminfo.block_count;
  
  return block;
}


void destroy_data_stack(struct data_stack *stack) {
  struct data_stack_block *current = stack->meminfo.first_block;
  
  struct data_stack_block *next;
  while (current) {
    next = current->next;
    free(current);
    current = next;
  }

  /* NULL dangling pointers */
  stack->meminfo.first_block = NULL;
  stack->meminfo.block_count = 0;
  stack->top_block = NULL;
  stack->top_index = 0;
}

void destroy_metastack(struct metastack *stack) {
  struct metastack_block *current = stack->meminfo.first_block;
  
  struct metastack_block *next;

  while (!IS_STACK_EMPTY(*stack)) {
    /* destroy each stack in metastack before freeing the memory for the entire structure */
    struct data_stack data_stack = pop_metastack(stack);
    destroy_data_stack(&data_stack);
  }  
  
  while (current) {
    /* could be stack blocks after 'top_block' (not freed until end to reduce malloc calls) */
    next = current->next;
    free(current);
    current = next;
  }

  /* NULL dangling pointers */
  stack->meminfo.first_block = NULL;
  stack->meminfo.block_count = 0;
  stack->top_block = NULL;
  stack->top_index = 0;
}

void destroy_delim_stack(struct delim_stack *stack) {
  struct delim_stack_block *current = stack->meminfo.first_block;
  
  struct delim_stack_block *next;
  while (current) {
    next = current->next;
    free(current);
    current = next;
  }

  /* NULL dangling pointers */
  stack->meminfo.first_block = NULL;
  stack->meminfo.block_count = 0;
  stack->top_block = NULL;
  stack->top_index = 0;
}


void *push_data_stack(struct data_stack *stack, uint8_t element) {
  if (IS_STACK_EMPTY(*stack)) {
    /* empty stack */
    struct data_stack_block *top_block = stack->meminfo.first_block;
    if (!top_block) {
      top_block = malloc(sizeof *stack->top_block);
      if (!top_block) return NULL;
    }
    top_block->prev = NULL;
    top_block->next = NULL;
    top_block->elements[0] = element;

    stack->top_block = top_block;
    stack->top_index = 1;
  }
  else if (stack->top_index < DATA_STACK_BLOCK_SIZE - 1) {
    /* normal case */
    stack->top_block->elements[++stack->top_index] = element;
  }
  else {
    /* push to new stack block  */
    stack->top_index = 0;
    struct data_stack_block *next_block = stack->top_block->next;
    if (!next_block) {
      next_block = new_data_stack_block(stack);
      if (!next_block) return NULL;
    }
    stack->top_block = next_block;
    next_block->elements[0] = element;
  }
  ++stack->size;
  return stack->top_block;
}

void *push_metastack(struct metastack *stack, struct data_stack data_stack) {
  if (IS_STACK_EMPTY(*stack)) {
    /* empty stack */
    struct metastack_block *top_block = stack->meminfo.first_block;
    if (!top_block) {
      top_block = malloc(sizeof *stack->top_block);
      if (!top_block) return NULL;
    }
    top_block->prev = NULL;
    top_block->next = NULL;
    top_block->stacks[0] = data_stack;

    stack->top_block = top_block;
    stack->top_index = 1;   
  } 
  else if (stack->top_index < METASTACK_BLOCK_SIZE - 1) {
    stack->top_block->stacks[++stack->top_index] = data_stack;
  }
  else {
    stack->top_index = 0;
    struct metastack_block *next_block = stack->top_block->next;
    if (!next_block) {
      next_block = new_metastack_block(stack);
      if (!next_block) return NULL;
    }
    stack->top_block = next_block;
    next_block->stacks[0] = data_stack;
  }
  ++stack->size;
  return stack->top_block;
}

void *push_delim_stack(struct delim_stack *stack, struct delim delim) {
  if (IS_STACK_EMPTY(*stack)) {
    /* empty stack */
    struct delim_stack_block *top_block = stack->meminfo.first_block;
    if (!top_block) {
      top_block = malloc(sizeof *stack->top_block);
      if (!top_block) return NULL;
    }
    top_block->prev = NULL;
    top_block->next = NULL;
    top_block->delims[0] = delim;

    stack->top_block = top_block;
    stack->top_index = 1;
  }
  else if (stack->top_index < DELIM_STACK_BLOCK_SIZE - 1) {
    stack->top_block->delims[++stack->top_index] = delim;
  }
  else {
    stack->top_index = 0;
    struct delim_stack_block *next_block = stack->top_block->next;
    if (!next_block) {
      next_block = new_delim_stack_block(stack);
      if (!next_block) return NULL;
    }
    stack->top_block = next_block;
    next_block->delims[0] = delim;
  }
  ++stack->size;
  return stack->top_block;
}


uint8_t pop_data_stack(struct data_stack *stack) {
  uint8_t element = stack->top_block->elements[stack->top_index];
  if (stack->top_index > 0) {
    /* normal case */
    --stack->top_index;
  }
  else {
    /* change block */
    stack->top_index = DATA_STACK_BLOCK_SIZE - 1;
    stack->top_block = stack->top_block->prev;
  }
  --stack->size;
  return element;
}

struct data_stack pop_metastack(struct metastack *stack) {
  struct data_stack data_stack = stack->top_block->stacks[stack->top_index];
  if (stack->top_index > 0) {
    /* normal case */
    --stack->top_index;
  }
  else {
    /* change block */
    stack->top_index = METASTACK_BLOCK_SIZE - 1;
    stack->top_block = stack->top_block->prev;
  }
  --stack->size;
  return data_stack;
}

struct delim pop_delim_stack(struct delim_stack *stack) {
  puts("hi");
  printf("top_block=%p, top_index=%zu, size=%zu, first_block=%p, block_count=%zu",
	 (void *)stack->top_block, stack->top_index, stack->size,
	 (void *)stack->meminfo.first_block, stack->meminfo.block_count);
  struct delim delim = stack->top_block->delims[stack->top_index];
  if (stack->top_index > 0) {
    /* normal case */
    --stack->top_index;
  }
  else {
    /* change block */
    stack->top_index = DELIM_STACK_BLOCK_SIZE - 1;
    stack->top_block = stack->top_block->prev;
  }
  --stack->size;
  return delim;
}


bool find_delim_stack(struct delim_stack *stack, struct delim *delim) {
  struct delim_stack_block *current = stack->top_block;
  size_t i = stack->top_index;
  while (current) {
    for ( ; i > 0; --i) {
      if (current->delims[i].type == delim->type) return true;
    }
    current = current->prev;
    i = DELIM_STACK_BLOCK_SIZE - 1;
  }
  return false;
}
