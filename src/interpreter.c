#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "interpreter.h"
#include "stack.h"

static struct data_stack auxiliary_stack = {0};
static struct metastack metastack = {0};

void kill_auxiliary_stack(void);
void kill_metastack(void);

void initialize_stacks(void);
struct data_stack new_main_stack(void);

void interpret(struct ast_node ast[], size_t length) {
  initialize_stacks();
  struct data_stack main_stack;
  if (!metastack.top_block) {
    main_stack = new_main_stack();
  }
  else {
    main_stack = pop_metastack(&metastack);
  }
  for (size_t i = 0; i < length; ++i) {
    struct ast_node node = ast[i];
    switch (node.token.type) {
    case ARROW_LEFT: {
      if (IS_STACK_EMPTY(auxiliary_stack)) {
	fprintf(stderr, "Tried to operate on empty stack at (%d, %d).\n",
		node.token.pos.row, node.token.pos.col);
	exit(EXIT_FAILURE);
      }
      uint8_t element = pop_data_stack(&auxiliary_stack);
      push_data_stack(&main_stack, element);
      break;
    }
    case ARROW_RIGHT: {
      if (IS_STACK_EMPTY(main_stack)) {
	fprintf(stderr, "Tried to operate on empty stack at (%d, %d).\n",
		node.token.pos.row, node.token.pos.col);
	exit(EXIT_FAILURE);
      }
      uint8_t element = pop_data_stack(&main_stack);
      push_data_stack(&auxiliary_stack, element);
      break;
    }
    case BANG: {
      if (IS_STACK_EMPTY(main_stack)) {
	fprintf(stderr, "Tried to operate on empty stack at (%d, %d).\n",
		node.token.pos.row, node.token.pos.col);
	exit(EXIT_FAILURE);
      }      
      uint8_t element = pop_data_stack(&main_stack);
      push_data_stack(&main_stack, element + 1);
      break;
    }
    case BKT_CURLY_LEFT:
      push_metastack(&metastack, main_stack);
      main_stack = new_main_stack();
      break;
    case BKT_CURLY_RIGHT: {
      destroy_data_stack(&main_stack);
      main_stack = pop_metastack(&metastack);
      size_t temp = node.jump_index;
      node.jump_index = i + 1;
      i = temp - 1;
      break;
    }
    case BKT_SQUARE_LEFT: {
      if (IS_STACK_EMPTY(main_stack)) {
	fprintf(stderr, "Tried to operate on empty stack at (%d, %d).\n",
		node.token.pos.row, node.token.pos.col);
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
	fprintf(stderr, "Tried to operate on empty stack at (%d, %d).\n",
		node.token.pos.row, node.token.pos.col);
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
	fprintf(stderr, "Tried to operate on empty stack at (%d, %d).\n",
		node.token.pos.row, node.token.pos.col);
	exit(EXIT_FAILURE);
      }
      putchar(pop_data_stack(&main_stack));
      break;
    case HASH:
      push_metastack(&metastack, main_stack);
      main_stack = new_main_stack();
      ast[ast[node.jump_index].jump_index].jump_index = i;
      i = node.jump_index - 1;
      break;
    case MINUS: {
      if (main_stack.size < 2) {
	fprintf(stderr, "Insufficient space on stack to carry out operation '-' at (%d, %d).\n",
		node.token.pos.row, node.token.pos.col);
	exit(EXIT_FAILURE);
      }
      uint8_t rhs = pop_data_stack(&main_stack);
      uint8_t lhs = pop_data_stack(&main_stack);
      push_data_stack(&main_stack, lhs - rhs);
      break;
    }
    case PERCENT: {
      if (main_stack.size < 2) {
	fprintf(stderr, "Insufficient space on stack to carry out operation '%%' at (%d, %d).\n",
		node.token.pos.row, node.token.pos.col);
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
	fprintf(stderr, "Insufficient space on stack to carry out operation '+' at (%d, %d).\n",
		node.token.pos.row, node.token.pos.col);
	exit(EXIT_FAILURE);
      }
      uint8_t rhs = pop_data_stack(&main_stack);
      uint8_t lhs = pop_data_stack(&main_stack);
      push_data_stack(&main_stack, lhs + rhs);
      break;
    }
    case STAR:
      if (IS_STACK_EMPTY(main_stack)) {
	fprintf(stderr, "Tried to operate on empty stack at (%d, %d).\n",
		node.token.pos.row, node.token.pos.col);
	exit(EXIT_FAILURE);
      }
      pop_data_stack(&main_stack);
      break;
    case WHIRLPOOL: {
      if (main_stack.size < 3) {
	fprintf(stderr, "Insufficient space on stack to carry out operation '@' at (%d, %d).\n",
		node.token.pos.row, node.token.pos.col);
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

struct data_stack new_main_stack(void) {
  struct data_stack new_main = {.meminfo.first_block = malloc(sizeof *new_main.meminfo.first_block)};
  if (!new_main.meminfo.first_block) {
    fprintf(stderr, "Could not create new main stack.\n");
    exit(EXIT_FAILURE);
  }
  return new_main;
}

void initialize_stacks(void) {
  if (!auxiliary_stack.meminfo.first_block) {
    struct data_stack_block *new = malloc(sizeof *auxiliary_stack.meminfo.first_block);
    if (!new) {
      fprintf(stderr, "Failed to initialize auxiliary_stack.\n");
      exit(EXIT_FAILURE);
    }
    auxiliary_stack.meminfo.first_block = new;
    atexit(kill_auxiliary_stack);
  }
  if (!metastack.meminfo.first_block) {
    struct metastack_block *new = malloc(sizeof *metastack.meminfo.first_block);
    if (!new) {
      fprintf(stderr, "Failed to initialize metastack.\n");
      exit(EXIT_FAILURE);
    }
    metastack.meminfo.first_block = new;
    atexit(kill_auxiliary_stack);
  }
}

void kill_auxiliary_stack(void) {
  if (auxiliary_stack.meminfo.first_block) destroy_data_stack(&auxiliary_stack);
}

void kill_metastack(void) {
  if (metastack.meminfo.first_block) destroy_metastack(&metastack);
}

