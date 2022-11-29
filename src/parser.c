#include "parser.h"

#include <stdio.h>
#include <stdlib.h>

#include "sizelims.h"
#include "stack.h"
#include "debug.h"
#include "dstring.h"
#include "reporting.h"
#include "pos.h"


void parse(const struct dstring *source, struct ast_list *ast) {
    const char *source_start = get_dstring_chars(source);
    const char *source_end = source_start + source->length;
    struct pos pos = {0, 0};
    parse_recursive(source_start, source_end, ast, &pos);
}


const char *parse_recursive(const char *source_start, const char *source_end,
			    struct ast_list *ast, struct pos *pos) {
    const char *string = source_start;//*
    for (; string < source_end; ++string) {
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
	node_p->pos = *pos;
	++pos->col;
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
	case '?':
	    node_p->sn.type = MAIN_NON_EMPTY;
	    break;
	case ';':
	    node_p->sn.type = AUX_NON_EMPTY;
	    break;

	case '[':
	    node_p->tag = LOOP_NODE;
	    if (!init_ast_list(&node_p->ln.body)) {
		exit(memory_error("Failed to initialize loop body.\n"));
	    }
	    ++pos->col;
	    string = parse_recursive(string + 1, source_end, &node_p->ln.body, pos);
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

	case '(':
	    ++pos->col;
	    string = parse_comment(string + 1, source_end, pos);
	    // Set the node up as an error in case the comment isn't closed properly.
	    // It will be reset to the proper type on the next iteration.
	    node_p->tag = ERROR_NODE;
	    node_p->en.symbol = '(';
	    if (string == source_end) {
		// The comment was never closed.
		exit(parse_error(node_p, "Unmatched '('. Unexpected EOF.\n"));
	    }
	    if (*string != ')') {
		// Some other character closed the comment.
		exit(parse_error(node_p, "Unmatched '('. Expected ')' but got '%c'.\n", *string));
	    }
	    // don't increment n
	    continue;
	case ')':
	    // This is only seen if there is no matching '('.
	    node_p->tag = ERROR_NODE;
	    node_p->en.symbol = ')';
	    exit(parse_error(node_p, "Unmatched ')'.\n"));
	case '\n':
	    ++pos->row;
	    pos->col = 0;
	    // fallthorugh
	default:
	    // don't increment n
	    continue;
	}
	++ast->length;
    }

    return string;
}

const char *parse_comment(const char *source_start, const char *source_end, struct pos *pos) {
    const char *string = source_start;
    for (; string < source_end && *string != ')'; ++string) {
	++pos->col;
	if (*string == '(') {
	    ++pos->col;
	    string = parse_comment(string + 1, source_end, pos);
	}
	else if (*string == '\n') {
	    ++pos->row;
	    pos->col = 0;
	}
    }
    return string;
}
