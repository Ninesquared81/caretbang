#include <stdio.h>
#include <stdlib.h>

#include "parser.h"
#include "sizelims.h"
#include "stack.h"
#include "dynamic-array.h"
#include "debug.h"


void parse(struct input_array *source, struct dynamic_array *ast) {
  struct delim_stack delim_stack;
  if (!init_delim_stack(&delim_stack)) {
    exit(memory_error("Failed to allocate delim_stack."));
  }
  for (size_t i = 0; i < source->length; ++i) {
    size_t n = ast->length;
    if (n >= ast->size) {
      switch (grow_ast_list(ast)) {
      case GROW_SUCCESS: break;
      case GROW_ALLOC_ERROR:
	exit(memory_error("Error: failed to reallocate ast."));
      case GROW_SIZE_ERROR:
	exit(compiler_limit("Error: requested size of ast too large (max size %zu bytes).",
			    (size_t)AST_LIST__MAX));
      }
    }
    struct ast_node *nodes = ast->nodes;
    nodes[n].tag = SIMPLE_NODE;
    switch(source->string[i]) {
    case '<':
      nodes[n].sn.type = AUX_TO_MAIN;
      break;
    case '>':
      nodes[n].sn.type = MAIN_TO_AUX;
      break;
    case '!':
      nodes[n].sn.type = INCREMENT;
      break;
    case '^':
      nodes[n].sn.type = PUSH_ZERO;
      break;
    case ':':
      nodes[n].sn.type = DUPE;
      break;
    case ',':
      nodes[n].sn.type = INPUT;
      break;
    case '.':
      nodes[n].sn.type = PRINT;
      break;
    case '-':
      nodes[n].sn.type = MINUS;
      break;
    case '%':
      nodes[n].sn.type = SWAP;
      break;
    case '+':
      nodes[n].sn.type = PLUS;
      break;
    case '*':
      nodes[n].sn.type = POP;
      break;
    case '@':
      nodes[n].sn.type = WHIRLPOOL;
      break;
    
    case '[': {
      nodes[n].tag = LOOP_NODE;
      void *ret = push_delim_stack(&delim_stack, (struct delim) {.type = LOOP_START, .index = n});/*
      fprintf(stderr,"ret = %p, top_block = %p, first_block = %p, type = %d;",
	      ret, (void *)delim_stack.top_block, delim_stack.meminfo.first_block,
	      delim_stack.top_block->delims[0].type);*/
      if (!ret) {
	fprintf(stderr, "Failed to push element to stack.\n");
	exit(EXIT_FAILURE);
      }
      if (!init_ast_list(&nodes[n].ln.body)) {
	report_location(source, i);
	exit(memory_error("Error: failed to allocate loop body.\n"));
      }
      break;
    }
    case ']': {
      nodes[n].type = BKT_SQUARE_RIGHT;/*
      fprintf(stderr,"first_block: %p, top_block: %p, size: %zu, type: %d;",
	      delim_stack.meminfo.first_block, (void *)delim_stack.top_block, delim_stack.size,0
	      //delim_stack.top_block->delims[0].type
	      );*/
      struct delim delim;
      if (IS_EMPTY(delim_stack) || (delim = pop_delim_stack(&delim_stack)).type != LOOP_START) {
	//fprintf(stderr, "type=%d;", delim.type);
	destroy_delim_stack(&delim_stack);
	parse_error("Unmatched ']'.\n");
     	exit(EXIT_FAILURE);
      }
      nodes[n].jump_index = delim.index;
      nodes[delim.index].jump_index = n + 1;
      break;
    }
    
    default:
      /* don't increment n */
      continue;
    }
    ++ast->length;
  }
  
  destroy_delim_stack(&delim_stack);
}
