#include "parser.h"

#include <stdio.h>
#include <stdlib.h>

#include "sizelims.h"
#include "stack.h"
#include "debug.h"
#include "dstring.h"
#include "reporting.h"


void parse(const struct dstring *restrict source, struct ast_list *restrict ast) {
    const char *source_start = get_dstring_chars(source);
    const char *source_end = source_start + source->length;
    parse_recursive(source_start, source_end, ast);
}


const char *parse_recursive(const char *restrict source_start, const char *restrict source_end,
			    struct ast_list *restrict ast) {
    /* struct delim_stack delim_stack;
    if (!init_delim_stack(&delim_stack)) {
	exit(memory_error("Failed to allocate delim_stack."));
	} (void)source_end; (void)ast;//*/
    const char *string = source_start;//*
    for (unsigned row = 0, col = 0; string < source_end; ++string) {
	if (ast->length >= get_ast_size_logical(ast)) {
	    switch (grow_ast_list(ast)) {
	    case GROW_SUCCESS: break;
	    case GROW_ALLOC_ERROR:
		exit(memory_error("Error: failed to reallocate ast."));
	    case GROW_SIZE_ERROR:
		exit(compiler_limit("Error: requested size of ast too large (max size %zu bytes).",
				    AST_LIST_MAX_BYTES));
	    default:
		exit(compiler_error("Inexhausive handling of grow_ast_list(ast).\n"));
	    }
	}
	struct ast_node *nodes = get_ast_nodes(ast);
	struct ast_node *node_p = nodes + ast->length;
	node_p->row = row;
	node_p->col = col++;
	node_p->tag = SIMPLE_NODE;
	switch(*string) {
	case '<':
	    node_p->sn.type = AUX_TO_MAIN;
	    break;
	case '>':
	    node_p->sn.type = MAIN_TO_AUX;
	    break;
	case '!':
	    node_p->sn.type = INCREMENT;
	    break;
	case '^':
	    node_p->sn.type = PUSH_ZERO;
	    break;
	case ':':
	    node_p->sn.type = DUPE;
	    break;
	case ',':
	    node_p->sn.type = INPUT;
	    break;
	case '.':
	    node_p->sn.type = PRINT;
	    break;
	case '-':
	    node_p->sn.type = MINUS;
	    break;
	case '%':
	    node_p->sn.type = SWAP;
	    break;
	case '+':
	    node_p->sn.type = PLUS;
	    break;
	case '*':
	    node_p->sn.type = POP;
	    break;
	case '@':
	    node_p->sn.type = DISCOVER;
	    break;
    
	case '[':
	    node_p->tag = LOOP_NODE;
	    string = parse_recursive(string, source_end, &node_p->ln.body);
	    if (string == source_end) {
		// The loop was never closed.
		exit(parse_error(node_p, "Unmatched '['. Unexpected EOF.\n"));
	    }
	    if (*string != ']') {
		// Some other character closed the loop.
		exit(parse_error(node_p, "Unmatched '['. Expected ']' but got '%c'.\n", *string));
	    }
	    break;
	case ']':
	    return string;
    
	case '\n':
	    ++row;
	    col = 0;
	    // fallthorugh
	default:
	    // don't increment n
	    continue;
	}
	++ast->length;
    }//*/
  
    // destroy_delim_stack(&delim_stack);
    return string;
}
