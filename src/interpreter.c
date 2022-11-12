#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "interpreter.h"
#include "stack.h"
#include "debug.h"

static struct data_stack main_stack = {0};
static struct data_stack auxiliary_stack = {0};

static void kill_main_stack(void);
static void kill_auxiliary_stack(void);

static void initialize_stacks(void);


void interpret(struct dynamic_array *ast) {
  initialize_stacks();
  for (size_t i = 0; i < ast->count; ++i) {
    uint8_t a, b, c;
    struct da_result result = ast_get_node(ast, i);
    switch (result.type) {
    case OK: break;
    case INDEX_ERROR:
      compiler_error("Error: could not access index %zu of ast", i);
      break;
    default:
      compiler_error("Error: unexpected error %s", da_result_type_to_string(result.type));
      break;
    }
    struct ast_node node = *(struct ast_node *)result.value;
    switch (node.type) {
    case ARROW_LEFT:
      if (IS_EMPTY(auxiliary_stack)) {
	fprintf(stderr, "Tried to operate on empty stack.\n");
	exit(EXIT_FAILURE);
      }
      a = pop_data_stack(&auxiliary_stack);
      push_data_stack(&main_stack, a);
      break;
    case ARROW_RIGHT:
      if (IS_EMPTY(main_stack)) {
	fprintf(stderr, "Tried to operate on empty stack.\n");
	exit(EXIT_FAILURE);
      }
      a = pop_data_stack(&main_stack);
      push_data_stack(&auxiliary_stack, a);
      break;
    case BANG:
      if (IS_EMPTY(main_stack)) {
	fprintf(stderr, "Tried to operate on empty stack.\n");
	exit(EXIT_FAILURE);
      }      
      a = pop_data_stack(&main_stack);
      push_data_stack(&main_stack, a + 1);
      break;
    case BKT_SQUARE_LEFT:
      if (IS_EMPTY(main_stack)) {
	fprintf(stderr, "Tried to operate on empty stack.\n");
	exit(EXIT_FAILURE);
      }
      a = pop_data_stack(&main_stack);
      if (!a) {
	i = node.jump_index - 1;
      }
      break;
    case BKT_SQUARE_RIGHT:
      i = node.jump_index - 1;
      break;
    case CARET:
      push_data_stack(&main_stack, 0);
      break;
    case COLON:
      if (IS_EMPTY(main_stack)) {
	fprintf(stderr, "Tried to operate on empty stack.\n");
	exit(EXIT_FAILURE);
      }      
      a = pop_data_stack(&main_stack);
      push_data_stack(&main_stack, a);
      push_data_stack(&main_stack, a);
      break;
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
      if (IS_EMPTY(main_stack)) {
	fprintf(stderr, "Tried to operate on empty stack.\n");
	exit(EXIT_FAILURE);
      }
      putchar(pop_data_stack(&main_stack));
      break;
    case MINUS:
      if (!IS_MIN_SIZE(main_stack, 2)) {
	fprintf(stderr, "Insufficient space on stack to carry out operation '-'.\n");
	exit(EXIT_FAILURE);
      }
      a = pop_data_stack(&main_stack);
      b = pop_data_stack(&main_stack);
      push_data_stack(&main_stack, b - a);
      break;
    case PERCENT:
      if (!IS_MIN_SIZE(main_stack, 2)) {
	fprintf(stderr, "Insufficient space on stack to carry out operation '%%'.\n");
	exit(EXIT_FAILURE);
      }
      a = pop_data_stack(&main_stack);
      b = pop_data_stack(&main_stack);
      push_data_stack(&main_stack, a);
      push_data_stack(&main_stack, b);
      break;
    case PLUS:
      if (!IS_MIN_SIZE(main_stack, 2)) {
	fprintf(stderr, "Insufficient space on stack to carry out operation '+'.\n");
	exit(EXIT_FAILURE);
      }
      a = pop_data_stack(&main_stack);
      b = pop_data_stack(&main_stack);
      push_data_stack(&main_stack, b + a);
      break;
    case STAR:
      if (IS_EMPTY(main_stack)) {
	fprintf(stderr, "Tried to operate on empty stack.\n");
	exit(EXIT_FAILURE);
      }
      pop_data_stack(&main_stack);
      break;
    case WHIRLPOOL:
      if (!IS_MIN_SIZE(main_stack, 3)) {
	fprintf(stderr, "Insufficient space on stack to carry out operation '@'.\n");
	exit(EXIT_FAILURE);
      }
      a = pop_data_stack(&main_stack);
      b = pop_data_stack(&main_stack);
      c = pop_data_stack(&main_stack);
      push_data_stack(&main_stack, b);
      push_data_stack(&main_stack, a);
      push_data_stack(&main_stack, c);
      break;

    default:
      fprintf(stderr, "Inexhaustive case analysis on enum token_type in 'interpreter.c' in interpret().");
      exit(EXIT_FAILURE);
    }
  }
}

void initialize_stacks(void) {
  if (!IS_INITIALIZED(main_stack)) {
    if (!init_data_stack(&main_stack)) {
      fprintf(stderr, "Failed to initialize main_stack.\n");
      exit(EXIT_FAILURE);
    }
    atexit(kill_main_stack);
  }
  if (!IS_INITIALIZED(auxiliary_stack)) {
    if (!init_data_stack(&auxiliary_stack)) {
      fprintf(stderr, "Failed to initialize auxiliary_stack.\n");
      exit(EXIT_FAILURE);
    }
    atexit(kill_auxiliary_stack);
  }
}

void kill_main_stack(void) {
  if (IS_INITIALIZED(main_stack)) destroy_data_stack(&main_stack);
}

void kill_auxiliary_stack(void) {
  if (IS_INITIALIZED(auxiliary_stack)) destroy_data_stack(&auxiliary_stack);
}


