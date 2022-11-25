#include "reporting.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>


int report_location(const struct ast_node *node) {
    fprintf(stderr, "%zu:%zu: ", node->row, node->col);
    return EXIT_FAILURE;
}

int parse_error(const struct ast_node *node, const char *restrict message, ...) {
    fprintf(stderr, "[Parse Error] ");
    report_location(node);
    char node_name[NODE_NAME_MAX];
    ast_human_readable_name(node, node_name);
    fprintf(stderr, "(at '%s'): ", node_name);
    
    va_list args;
    va_start(args, message);
    vfprintf(stderr, message, args);
    va_end(args);
    
    return EXIT_FAILURE;
}


int empty_stack_error(const struct ast_node *node) {
    fprintf(stderr, "[Stack Error] ");
    report_location(node);
    fprintf(stderr, "Tried to operate on empty stack.\n");
    return EXIT_FAILURE;
}

int stack_size_error(const struct ast_node *node, size_t expected_size, size_t actual_size) {
    fprintf(stderr, "[Stack Error] ");
    report_location(node);
    char node_name[NODE_NAME_MAX];
    ast_human_readable_name(node, node_name);
    fprintf(stderr,
	    "Operation '%s' requires at least %zu elements on the stack, but only %zu were found.\n",
	    node_name, expected_size, actual_size);
    return EXIT_FAILURE;
}

