#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "interpreter.h"
#include "stack.h"

static struct data_stack main_stack = {0};
static struct data_stack auxiliary_stack = {0};
static struct metastack metastack = {0};

void kill_main_stack(void);
void kill_auxiliary_stack(void);
void kill_metastack(void);

void initialize_stacks(void);
void new_main_stack(void);
void return_main_stack(void);

void interpret(struct ast_node ast[], size_t length) {
  initialize_stacks();
  for (size_t i = 0; i < length; ++i) {
    struct ast_node node = ast[i];
    switch (node.type) {
    case ARROW_LEFT: {
      if (IS_STACK_EMPTY(auxiliary_stack)) {
	fprintf(stderr, "Tried to operate on empty stack.\n");
	exit(EXIT_FAILURE);
      }
      uint8_t element = pop_data_stack(&auxiliary_stack);
      push_data_stack(&main_stack, element);
      break;
    }
    case ARROW_RIGHT: {
      if (IS_STACK_EMPTY(main_stack)) {
	fprintf(stderr, "Tried to operate on empty stack.\n");
	exit(EXIT_FAILURE);
      }
      uint8_t element = pop_data_stack(&main_stack);
      push_data_stack(&auxiliary_stack, element);
      break;
    }
    case BANG: {
      if (IS_STACK_EMPTY(main_stack)) {
	fprintf(stderr, "Tried to operate on empty stack.\n");
	exit(EXIT_FAILURE);
      }      
      uint8_t element = pop_data_stack(&main_stack);
      push_data_stack(&main_stack, element + 1);
      break;
    }
    case BKT_CURLY_LEFT:
      new_main_stack();
      break;
    case BKT_CURLY_RIGHT: {
      return_main_stack();
      size_t temp = node.jump_index;
      node.jump_index = i + 1;
      i = temp - 1;
      break;
    }
    case BKT_SQUARE_LEFT: {
      if (IS_STACK_EMPTY(main_stack)) {
	fprintf(stderr, "Tried to operate on empty stack.\n");
	exit(EXIT_FAILURE);
      }      
      uint8_t element = pop_data_stack(&main_stack);
      if (!element) {
	i = node.jump_index - 1;
      }
      break;
    }
    case BKT_SQUARE_RIGHT:
      i = node.jump_index - 1;
      break;
    case CARET:
      push_data_stack(&main_stack, 0);
      break;
    case COLON: {
      if (IS_STACK_EMPTY(main_stack)) {
	fprintf(stderr, "Tried to operate on empty stack.\n");
	exit(EXIT_FAILURE);
      }      
      uint8_t element = pop_data_stack(&main_stack);
      push_data_stack(&main_stack, element);
      push_data_stack(&main_stack, element);
      break;
    }
    case COMMA: {
      int ch = getchar();
      if (feof(stdin)) {
	ch = 0;
	clearerr(stdin);
      }
      push_data_stack(&main_stack, ch);
      break;
    }
    case DOT:
      if (IS_STACK_EMPTY(main_stack)) {
	fprintf(stderr, "Tried to operate on empty stack.\n");
	exit(EXIT_FAILURE);
      }
      putchar(pop_data_stack(&main_stack));
      break;
    case HASH:
      new_main_stack();
      ast[ast[node.jump_index].jump_index].jump_index = i;
      i = node.jump_index - 1;
      break;
    case MINUS: {
      if (main_stack.size < 2) {
	fprintf(stderr, "Insufficient space on stack to carry out operation '-'.\n");
	exit(EXIT_FAILURE);
      }
      uint8_t rhs = pop_data_stack(&main_stack);
      uint8_t lhs = pop_data_stack(&main_stack);
      push_data_stack(&main_stack, lhs - rhs);
      break;
    }
    case PERCENT: {
      if (main_stack.size < 2) {
	fprintf(stderr, "Insufficient space on stack to carry out operation '%%'.\n");
	exit(EXIT_FAILURE);
      }
      uint8_t top_element = pop_data_stack(&main_stack);
      uint8_t next_element = pop_data_stack(&main_stack);
      push_data_stack(&main_stack, top_element);
      push_data_stack(&main_stack, next_element);
      break;
    }
    case PLUS: {
      if (main_stack.size < 2) {
	fprintf(stderr, "Insufficient space on stack to carry out operation '+'.\n");
	exit(EXIT_FAILURE);
      }
      uint8_t rhs = pop_data_stack(&main_stack);
      uint8_t lhs = pop_data_stack(&main_stack);
      push_data_stack(&main_stack, lhs + rhs);
      break;
    }
    case STAR:
      if (IS_STACK_EMPTY(main_stack)) {
	fprintf(stderr, "Tried to operate on empty stack.\n");
	exit(EXIT_FAILURE);
      }
      pop_data_stack(&main_stack);
      break;
    case WHIRLPOOL: {
      if (main_stack.size < 3) {
	fprintf(stderr, "Insufficient space on stack to carry out operation '@'.\n");
	exit(EXIT_FAILURE);
      }
      uint8_t top_element = pop_data_stack(&main_stack);
      uint8_t next_element = pop_data_stack(&main_stack);
      uint8_t hidden_element = pop_data_stack(&main_stack);
      push_data_stack(&main_stack, next_element);
      push_data_stack(&main_stack, top_element);
      push_data_stack(&main_stack, hidden_element);
      break;
    }
    default:
      fprintf(stderr, "Inexhaustive case analysis on enum token_type in 'interpreter.c' in interpret().");
      exit(EXIT_FAILURE);
    }
  }
}

void new_main_stack(void) {
  push_metastack(&metastack, main_stack);
  if (!init_data_stack(&main_stack)) {
    fprintf(stderr, "Could not create new main stack.\n");
    exit(EXIT_FAILURE);
  }
}

void return_main_stack(void) {
  destroy_data_stack(&main_stack);
  main_stack = pop_metastack(&metastack);
}

void initialize_stacks(void) {
  if (!IS_STACK_INIT(main_stack)) {
    if (!init_data_stack(&main_stack)) {
      fprintf(stderr, "Failed to initialize main_stack.\n");
      exit(EXIT_FAILURE);
    }
    atexit(kill_main_stack);
  }
  if (!IS_STACK_INIT(auxiliary_stack)) {
    if (!init_data_stack(&auxiliary_stack)) {
      fprintf(stderr, "Failed to initialize auxiliary_stack.\n");
      exit(EXIT_FAILURE);
    }
    atexit(kill_auxiliary_stack);
  }
  if (!IS_STACK_INIT(metastack)) {
    if (!init_metastack(&metastack)) {
      fprintf(stderr, "Failed to initialize metastack.\n");
      exit(EXIT_FAILURE);
    }
    atexit(kill_auxiliary_stack);
  }
}

void kill_main_stack(void) {
  if (IS_STACK_INIT(main_stack)) destroy_data_stack(&main_stack);
}

void kill_auxiliary_stack(void) {
  if (IS_STACK_INIT(auxiliary_stack)) destroy_data_stack(&auxiliary_stack);
}

void kill_metastack(void) {
  if (IS_STACK_INIT(metastack)) destroy_metastack(&metastack);
}

