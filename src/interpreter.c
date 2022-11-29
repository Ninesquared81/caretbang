#include "interpreter.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "stack.h"
#include "debug.h"
#include "ast.h"
#include "reporting.h"


static struct data_stack main_stack = {0};
static struct data_stack auxiliary_stack = {0};


static void kill_main_stack(void);
static void kill_auxiliary_stack(void);

static void initialize_stacks(void);

static void interpret_recursive(struct ast_list *ast);
static void interpret_simple_node(struct ast_node *node);
static void interpret_loop_node(struct ast_node *node);


void interpret(struct ast_list *ast) {
    initialize_stacks();
    interpret_recursive(ast);
}


void interpret_recursive(struct ast_list *ast) {
    struct ast_node *start_node = ast->darray.elements;
    struct ast_node *end_node = start_node + ast->length;
    for (struct ast_node *node = start_node; node < end_node; ++node) {
	switch (node->tag) {
	case SIMPLE_NODE:
	    interpret_simple_node(node);
	    break;
	case LOOP_NODE:
	    interpret_loop_node(node);
	    break;
	case ERROR_NODE:
	    exit(compiler_error(
		     "Unexpected ERROR_NODE (%d:%d:'%c') in interpretation step.\n",
		     node->pos.row, node->pos.col, node->en.symbol));
	default:
	    exit(compiler_error("Inexhaustive case analysis of node tags."));
	}
    }
}

void interpret_simple_node(struct ast_node *node) {
    uint8_t a, b, c;
    switch (node->sn.type) {
    case AUX_TO_MAIN:
	if (IS_EMPTY(auxiliary_stack)) {
	    exit(empty_stack_error(node));
	}
	a = pop_data_stack(&auxiliary_stack);
	push_data_stack(&main_stack, a);
	break;
    case MAIN_TO_AUX:
	if (IS_EMPTY(main_stack)) {
	    exit(empty_stack_error(node));
	}
	a = pop_data_stack(&main_stack);
	push_data_stack(&auxiliary_stack, a);
	break;
    case INCREMENT:
	if (IS_EMPTY(main_stack)) {
	    exit(empty_stack_error(node));
	}      
	a = pop_data_stack(&main_stack);
	push_data_stack(&main_stack, a + 1);
	break;
    case PUSH_ZERO:
	push_data_stack(&main_stack, 0);
	break;
    case DUPE:
	if (IS_EMPTY(main_stack)) {
	    exit(empty_stack_error(node));
	}      
	a = pop_data_stack(&main_stack);
	push_data_stack(&main_stack, a);
	push_data_stack(&main_stack, a);
	break;
    case INPUT: {
	int ch = getchar();
	if (feof(stdin)) {
	    ch = 0;
	    clearerr(stdin);
	}
	push_data_stack(&main_stack, ch);
	break;
    }
    case PRINT:
	if (IS_EMPTY(main_stack)) {
	    exit(empty_stack_error(node));
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
    case PLUS:
	if (!IS_MIN_SIZE(main_stack, 2)) {
	    fprintf(stderr, "Insufficient space on stack to carry out operation '+'.\n");
	    exit(EXIT_FAILURE);
	}
	a = pop_data_stack(&main_stack);
	b = pop_data_stack(&main_stack);
	push_data_stack(&main_stack, b + a);
	break;
    case POP:
	if (IS_EMPTY(main_stack)) {
	    exit(empty_stack_error(node));
	}
	pop_data_stack(&main_stack);
	break;
    case SWAP:
	if (!IS_MIN_SIZE(main_stack, 2)) {
	    fprintf(stderr, "Insufficient space on stack to carry out operation '%%'.\n");
	    exit(EXIT_FAILURE);
	}
	a = pop_data_stack(&main_stack);
	b = pop_data_stack(&main_stack);
	push_data_stack(&main_stack, a);
	push_data_stack(&main_stack, b);
	break;
    case DISCOVER:
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
    case MAIN_NON_EMPTY:
	push_data_stack(&main_stack, !IS_EMPTY(main_stack));
	break;
    case AUX_NON_EMPTY:
	push_data_stack(&main_stack, !IS_EMPTY(auxiliary_stack));
	break;
    case EXIT_PROGRAM:
	if (IS_EMPTY(main_stack)) {
	    exit(empty_stack_error(node));
	}
	// exit with a user-specified exit status
	a = pop_data_stack(&main_stack);
	exit(a);
    default:
	exit(compiler_error("Inexhaustive case analysis of node->sn.type."));
    }
}
  
void interpret_loop_node(struct ast_node *node) {
    // Check before first loop iteration.
    if (IS_EMPTY(main_stack)) {
	exit(empty_stack_error(node));
    }
    while (pop_data_stack(&main_stack)) {
	interpret_recursive(&node->ln.body);

	// This check cannot be easily factored into the loop condition.
	if (IS_EMPTY(main_stack)) {
	    exit(empty_stack_error(node));
	}
    }
}

void initialize_stacks(void) {
    if (!IS_INITIALIZED(main_stack)) {
	if (!init_data_stack(&main_stack)) {
	    fprintf(stderr, "Failed to initialize main_stack.\n");
	    exit(EXIT_FAILURE);
	}
	if (atexit(kill_main_stack) != 0) {
	    exit(compiler_error("atexit() failed to register kill_main_stack()."));
	}
    }
    if (!IS_INITIALIZED(auxiliary_stack)) {
	if (!init_data_stack(&auxiliary_stack)) {
	    fprintf(stderr, "Failed to initialize auxiliary_stack.\n");
	    exit(EXIT_FAILURE);
	}
	if (atexit(kill_auxiliary_stack) != 0) {
	    exit(compiler_error("atexit() failed to register kill_auxiliary_stack()."));
	}
    }
}

void kill_main_stack(void) {
    if (IS_INITIALIZED(main_stack)) destroy_data_stack(&main_stack);
}

void kill_auxiliary_stack(void) {
    if (IS_INITIALIZED(auxiliary_stack)) destroy_data_stack(&auxiliary_stack);
}
