#define __USE_MINGW_ANSI_STDIO 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sizelims.h"
#include "stack.h"
#include "parser.h"
#include "interpreter.h"
#include "debug.h"
#include "dstring.h"
#include "command_line.h"


/* Global Resources */

static struct dstring input = {0};
static struct ast_list ast = {0};


/* Function Prototypes */

static void init_input(void);
static void init_ast(void);

static void kill_input(void);
static void kill_ast(void);

static void run(void);


/* Main Function */

int main(int argc, char **argv) {
    init_input();
    init_ast();
  
    char *filename = NULL;  // should be initialized in next function
    parse_cmd(argc, argv, &filename);
    if (!filename) {
	exit(compiler_error("parse_cmd() failed to initialize 'filename'.\n"));
    }
    
    FILE *fp = fopen(filename, "r");
    if (!fp) {
	fprintf(stderr, "Could not open file '%s'.\n", filename);
	if (ferror(fp)) {
	    perror("Error");
	}
	return EXIT_FAILURE;
    }
    
    for (
	char *buffer_pointer = input.darray.elements;
	(input.length = fread(buffer_pointer, sizeof(char),
			      input.darray.size, fp)) == input.darray.size;
	buffer_pointer += input.length) {
	if (ferror(fp)) {
	    fprintf(stderr, "An error occurred reading file '%s'.\n", filename);
	    perror("Error");
	    return EXIT_FAILURE;
	}
	switch (grow_dstring(&input)) {
	case GROW_SUCCESS: break;
	case GROW_SIZE_ERROR:
	    exit(compiler_limit("Input too large (maximum input length is %zu bytes).", DSTRING_MAX_BYTES));
	case GROW_ALLOC_ERROR:
	    exit(memory_error("Could not reallocate input array."));
	default:
	    exit(compiler_error("Inexhaustive case analysis for `enum da_grow_rc`.\n"));
	}
    }
    fclose(fp);
    run();
    
    return EXIT_SUCCESS;
}


/* Global Resource Management */

void init_input(void) {
    if (atexit(kill_input) != 0) {
	exit(compiler_error("Failed to register kill_input() with atexit().\n"));
    }
    if (!init_dstring(&input)) {
	exit(memory_error("Failed to initialize input.\n"));
    }
}

void init_ast(void) {
    if (atexit(kill_ast) != 0) {
	exit(compiler_error("Failed to register kill_ast() with atexit().\n"));
    }
    if (!init_ast_list(&ast)) {
	exit(memory_error("Failed to initialize ast.\n"));
    }
}

void kill_input(void) {
    destroy_dstring(&input);
}

void kill_ast(void) {
    destroy_ast_list(&ast);
}


/* Helper Functions */

void run(void) {
    parse(&input, &ast);
    interpret(&ast);
}

