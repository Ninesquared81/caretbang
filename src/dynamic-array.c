#include <stdlib.h>

#include "dynamic-array.h"
#include "sizelims.h"


void *init_ast_list(struct ast_list *ast) {
  struct ast_list *nodes = malloc(AST_LIST_INITIAL_SIZE);
  ast->nodes = nodes;
  ast->size = (nodes) ? AST_LIST_INITIAL_SIZE : 0;
  ast->length = 0;
  return nodes;
}

void *init_input_array(struct input_array *input) {
  struct input_array *string = calloc(INPUT_ARRAY_INITIAL_SIZE, sizeof input->string);
  input->string = string;
  input->size = (nodes) ? INPUT_ARRAY_INITIAL_SIZE : 0;
  input->length = 0;
  return string;
}

void destroy_ast_list(struct ast_list *ast) {
  free(ast->nodes);
  ast->nodes = NULL;
  ast->size = 0;
  ast->length = 0;
}

void destroy_input_array(struct input_array *input) {
  free(input->string);
  input->string = NULL;
  input->size = 0;
  input->length = 0;
}

enum da_grow_error grow_ast_list(struct ast_list *ast) {
  size_t old_size = ast->size;
  if (!CAN_GROW_SIZE(old_size, AST_LIST_MAX)) return NULL;
  
  size_t new_size = GROW_ARRAY_SIZE(old_size);
  void *new_nodes = realloc(ast->nodes, new_size);
  if (!new_nodes) return NULL;

  ast->nodes = new_nodes;
  ast->size = new_size;

  return new_nodes;
}

enum da_grow_error grow_input_array(struct input_array *input) {
  size_t old_size = input->size;
  if (!CAN_GROW_SIZE(old_size, INPUT_ARRAY_MAX)) return GROW_SIZE_ERROR;
  
  size_t new_size = GROW_ARRAY_SIZE(old_size);
  void *new_string = realloc(input->string, new_size);
  if (!new_string) return GROW_ALLOC_ERROR;
  
  memset(new_string + old_size, 0, new_size - old_size);
  input->string = new_string;
  input->size = new_size;
  
  return GROW_SUCCESS;
}
