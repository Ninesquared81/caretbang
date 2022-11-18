#include "ast.h"
#include "dynamic_array.h"
#include "sizelims.h"


void *init_ast_list(ast_list *ast) {
  return init_dynamic_array(&ast->_darray, AST_LIST_INITIAL_SIZE);
}

void destroy_ast_list(ast_list *ast) {
  destroy_dynamic_array(&ast->_darray);
}

struct ast_node *get_nodes(ast_list *ast) {
  return ast->_darray.elements;
}

size_t get_ast_size(ast_list *ast) {
  return ast->_darray.size;
}

enum da_grow_rc grow_ast_list(ast_list *ast) {
  return grow_dynamic_array(&ast->_darray, AST_LIST_MAX);
}
