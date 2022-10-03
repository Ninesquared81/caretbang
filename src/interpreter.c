#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "interpreter.h"
#include "stack.h"

static struct data_stack auxiliary_stack = {0};
static struct metastack metastack = {0};
static struct symbol_stack symbol_stack = {0};

void kill_auxiliary_stack(void);
void kill_metastack(void);
void kill_symbol_stack(void);

void initialize_stacks(void);

void interpret(struct token tokens[]) {
  initialize_stacks();
  struct data_stack main_stack;
  if (!metastack.top_block) {
    main_stack = (struct data_stack) {.start = malloc(sizeof *main_stack.start)};
  }
  else {
    main_stack = pop_metastack(&metastack);
  }
  push_metastack(&metastack, main_stack);
  for (size_t i = 0; i < TOKENS_MAX; ++i) {
    switch (tokens[i].type) {
    case ARROW_LEFT: {
      uint8_t element = pop_data_stack(&main_stack);
      push_data_stack(&auxiliary_stack, element);
      break;
    }
    case ARROW_RIGHT: {
      uint8_t element = pop_data_stack(&auxiliary_stack);
      push_data_stack(&main_stack, element);
      break;
    }
    case BKT_CURLY_LEFT:
      break;
    case BKT_CURLY_RIGHT:
      break;
    case BKT_SQUARE_LEFT:
      break;
    case BKT_SQUARE_RIGHT:
      break;
    case CARET:
      break;
    case COLON:
      break;
    case COMMA:
      break;
    case DOT:
      break;
    case HASH:
      break;
    case MINUS:
      break;
    case PERCENT:
      break;
    case PLUS:
      break;
    case STAR:
      break;
    case WHIRLPOOL:
      break;
    default:
      fprintf(stderr, "Inexhaustive case analysis on enum token_type in 'interpreter.c' in interpret().");
      exit(EXIT_FAILURE);
    }
  }
}

void initialize_stacks(void) {
  if (!auxiliary_stack.start) {
    struct data_stack_block *new = malloc(sizeof *auxiliary_stack.start);
    if (!new) {
      fprintf(stderr, "Failed to initialize auxiliary_stack.\n");
      exit(EXIT_FAILURE);
    }
    auxiliary_stack.start = new;
    atexit(kill_auxiliary_stack);
  }
  if (!metastack.start) {
    struct metastack_block *new = malloc(sizeof *metastack.start);
    if (!new) {
      fprintf(stderr, "Failed to initialize metastack.\n");
      exit(EXIT_FAILURE);
    }
    metastack.start = new;
    atexit(kill_auxiliary_stack);
  }
  if (!symbol_stack.start) {
    struct symbol_stack_block *new = malloc(sizeof *symbol_stack.start);
    if (!new) {
      fprintf(stderr, "Failed to initialize symbol_stack.\n");
      exit(EXIT_FAILURE);
    }
    symbol_stack.start = new;
    atexit(kill_symbol_stack);
  }
}

void kill_auxiliary_stack(void) {
  if (auxiliary_stack.start) destroy_data_stack(&auxiliary_stack);
}

void kill_metastack(void) {
  if (metastack.start) destroy_metastack(&metastack);
}

void kill_symbol_stack(void) {
  if (symbol_stack.start) destroy_symbol_stack(&symbol_stack);
}
