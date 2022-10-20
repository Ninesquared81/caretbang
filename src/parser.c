#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "sizelims.h"
#include "stack.h"
#define printf __mingw_printf
#define fprintf __mingw_fprintf
size_t parse(char *code, size_t length, struct ast_node ast[]) {
  size_t n = 0;
  char c;
  struct delim_stack delim_stack;
  if (!init_delim_stack(&delim_stack)) {
    fprintf(stderr, "Failed to allocate delim_stack in 'parser.c' in  parse().");
    exit(EXIT_FAILURE);
  }
  while ((c = *code++)) {
    ast[n].jump_index = n;
    switch(c) {
    case '<':
      ast[n].type = ARROW_LEFT;
      break;
    case '>':
      ast[n].type = ARROW_RIGHT;
      break;
    case '!':
      ast[n].type = BANG;
      break;
    case '^':
      ast[n].type = CARET;
      break;
    case ':':
      ast[n].type = COLON;
      break;
    case ',':
      ast[n].type = COMMA;
      break;
    case '.':
      ast[n].type = DOT;
      break;
    case '-':
      ast[n].type = MINUS;
      break;
    case '%':
      ast[n].type = PERCENT;
      break;
    case '+':
      ast[n].type = PLUS;
      break;
    case '*':
      ast[n].type = STAR;
      break;
    case '@':
      ast[n].type = WHIRLPOOL;
      break;
    case '#':
      ast[n].type = HASH;
      
      break;
      
    case '{': {
      ast[n].type = BKT_CURLY_LEFT;
      push_delim_stack(&delim_stack, (struct delim) {.type = STACK_START, .index = n});
      break;
    }
    case '}': {
      ast[n].type = BKT_CURLY_RIGHT;
      struct delim delim;
      if (IS_STACK_EMPTY(delim_stack) || (delim = pop_delim_stack(&delim_stack)).type != STACK_START) {
	destroy_delim_stack(&delim_stack);
	fprintf(stderr, "Unmatched '}' at %d.\n", n);
	exit(EXIT_FAILURE);	
      }
      ast[n] = (struct ast_node) {.token = token, .jump_index = n + 1};
      ast[delim.index].jump_index = n;
      break;
    }
    
    case '[': {
      ast[n].type = BKT_SQUARE_LEFT;
      void *ret = push_delim_stack(&delim_stack, (struct delim) {.type = LOOP_START, .index = n});
      fprintf(stderr,"ret = %p, top_block = %p, first_block = %p, type = %d;",
	      ret, (void *)delim_stack.top_block, delim_stack.meminfo.first_block,
	      delim_stack.top_block->delims[0].type);
      if (!ret) {
	fprintf(stderr, "Failed to push element to stack.\n");
	exit(EXIT_FAILURE);
      }
      break;
    }
    case ']': {
      ast[n].type = BKT_SQUARE_RIGHT;
      fprintf(stderr,"first_block: %p, top_block: %p, size: %zu, type: %d;",
	      delim_stack.meminfo.first_block, (void *)delim_stack.top_block, delim_stack.size,0
	      //delim_stack.top_block->delims[0].type
	      );
      struct delim delim;
      if (IS_STACK_EMPTY(delim_stack) || (delim = pop_delim_stack(&delim_stack)).type != LOOP_START) {
	fprintf(stderr, "type=%d;", delim.type);
	destroy_delim_stack(&delim_stack);
	fprintf(stderr, "Unmatched ']' at (%d, %d).\n", token.pos.row, token.pos.col);
	exit(EXIT_FAILURE);
      }
      ast[n].jump_index = delim.index;
      ast[delim.index].jump_index = n + 1;
      break;
    }
    default:
      /* don't increment n */
      continue;
    }
    ++n;
  }
  
  destroy_delim_stack(&delim_stack);
  
  if (n >= AST_MAX) {
    fprintf(stderr, "AST_MAX exceeded.\n");
    exit(EXIT_FAILURE);
  }
  
  return n;
}
