#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "sizelims.h"
#include "stack.h"

size_t parse(struct token tokens[], size_t length, struct ast_node ast[]) {
  size_t n = 0;
  struct delim_stack delim_stack = {.start =  malloc(sizeof *delim_stack.start)};
  if (!delim_stack.start) {
    fprintf(stderr, "Failed to allocate delim_stack in 'parser.c' in  parse().");
    exit(EXIT_FAILURE);
  }
  for (size_t i = 0; i < length; ++i, ++n) {
    struct token token = tokens[i];
    switch(token.type) {
    /* simple tokens: */
    case ARROW_LEFT:
    case ARROW_RIGHT:
    case BANG:
    case CARET:
    case COLON:
    case COMMA:
    case DOT:
    case MINUS:
    case PERCENT:
    case PLUS:
    case STAR:
    case WHIRLPOOL:
      ast[n] = (struct ast_node) {.token = token, .jump_index = n + 1};
      break;

    /* special case */
    case HASH: {
      struct delim delim = {.type = LOOP_START};
      struct ast_node node = {.token = token};
      if (find_delim_stack(&delim_stack, &delim)) {
	node.jump_index = delim.index;
      }
      else {
	node.jump_index = 0;
      }
      ast[n] = node;
      break;
    }
      
    /* body tokens */
    case BKT_CURLY_LEFT:
      ast[n] = (struct ast_node) {.token = token, .jump_index = n + 1};
      push_delim_stack(&delim_stack, (struct delim) {.type = STACK_START, .index = n});
      break;
    case BKT_CURLY_RIGHT: {
      struct delim delim;
      if (IS_STACK_EMPTY(delim_stack) || (delim = pop_delim_stack(&delim_stack)).type != STACK_START) {
	destroy_delim_stack(&delim_stack);
	fprintf(stderr, "Unmatched '}' at (%d, %d).\n", token.pos.row, token.pos.col);
	exit(EXIT_FAILURE);	
      }
      ast[n] = (struct ast_node) {.token = token, .jump_index = n + 1};
      ast[delim.index].jump_index = n;
      break;
    }
    
    case BKT_SQUARE_LEFT:
      ast[n] = (struct ast_node) {.token = token, .jump_index = n + 1};
      push_delim_stack(&delim_stack, (struct delim) {.type = LOOP_START, .index = n});
      break;
    case BKT_SQUARE_RIGHT: {
      struct delim delim;
      if (IS_STACK_EMPTY(delim_stack) || (delim = pop_delim_stack(&delim_stack)).type != LOOP_START) {
	destroy_delim_stack(&delim_stack);
	fprintf(stderr, "Unmatched ']' at (%d, %d).\n", token.pos.row, token.pos.col);
	exit(EXIT_FAILURE);
      }
      ast[n] = (struct ast_node) {.token = token, .jump_index = delim.index};
      ast[delim.index].jump_index = n + 1;
      break;
    }
    default:
      destroy_delim_stack(&delim_stack);
      fprintf(stderr, "Inexhaustive case analysis in 'parser.c' in parse().");
      exit(EXIT_FAILURE);
    }
  }
  destroy_delim_stack(&delim_stack);
  if (n >= AST_MAX) {
    fprintf(stderr, "AST_MAX exceeded.\n");
    exit(EXIT_FAILURE);
  }
  return n;
}
