#define __USE_MINGW_ANSI_STDIO 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sizelims.h"
#include "stack.h"
#include "parser.h"
#include "interpreter.h"
#include "dynamic-array.h"
#include "debug.h"
#include "input.h"


/* Global Resources */

static struct dynamic_array input_array  = {0};
static struct dynamic_array ast = {0};


/* Function Prototypes */

static void init_input_array(void);
static void init_ast(void);

static void kill_input_array(void);
static void kill_ast(void);

static void run(void);
static void parse_cmd(int argc, char **argv, char **filename);


/* Main Function */

int main(int argc, char **argv) {
  init_input_array();
  init_ast();
  
  char *filename;
  parse_cmd(argc, argv, &filename);
  
  if (filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
      fprintf(stderr, "Could not open file '%s'.\n", filename);
      if (ferror(fp)) {
	perror("Error");
      }
      return EXIT_FAILURE;
    }
    
    for (char *buffer_pointer = input_array.elements;
	 (input_array.count = fread(buffer_pointer,
				    sizeof(char),
				    input_array.size,
				    fp)
	  ) == input_array.size;
	 buffer_pointer = (char *)input_array.count + input_array.count) {
      if (ferror(fp)) {
	fprintf(stderr, "An error occurred reading file '%s'.\n", filename);
	perror("Error");
	return EXIT_FAILURE;
      }
      enum da_result_type result_type = da_grow(&input_array);
      switch (result_type) {
      case OK: break;
      case SIZE_ERROR:
	fprintf(stderr, "Input too large (maximum input length is %zu bytes).", (size_t)DYN_ARRAY_MAX_SIZE);
	return EXIT_FAILURE;
      case ALLOCATION_ERROR:
	fprintf(stderr, "Could not reallocate input array.");
	return EXIT_FAILURE;
      default:
	exit(compiler_error("Error: unexpected error %s from da_grow_array().",
			    da_result_type_to_string(result_type)));
      }
    }
    fclose(fp);
    run();
  }
  else {
    while (fgets(input_array.elements, input_array.size, stdin)) {
      run();
    }
  }

  return EXIT_SUCCESS;
}


/* Global Resource Management */

void init_input_array(void) {
  atexit(kill_input_array);
  
  switch (da_init(&input_array)) {
  case OK: break;
  case ALLOCATION_ERROR:
    fprintf(stderr, "Could not allocate input_array!\n");
    exit(EXIT_FAILURE);
  default:
    fprintf(stderr, "Unexpected error when initializing input_buffer.\n");
    exit(EXIT_FAILURE);
  }
}

void init_ast(void) {
  atexit(kill_ast);
  
  switch (da_init(&ast)) {
  case OK: break;
  case ALLOCATION_ERROR:
    fprintf(stderr, "Could not allocate ast!\n");
    exit(EXIT_FAILURE);
  default:
    fprintf(stderr, "Unexpected error when initializing ast.\n");
    exit(EXIT_FAILURE);
  }
}

void kill_input_array(void) {
  da_destroy(&input_array);
}

void kill_ast(void) {
  da_destroy(&ast);
}


/* Helper Functions */

void run(void) {
  //size_t ast_length = parse(input_array, ast);
  //interpret(ast, ast_length);
}

void parse_cmd(int argc, char **argv, char **filename) {
  const char *usage = "Usage: %s [filename]\n";
  const char *help_msg = "\nPositional arguments:\n"
                         "filename -- (optional) name of source code file to interpret;\n"
                         "            if omitted, enter interactive mode.\n"
                         "Options:\n"
                         "-h, --help -- show this help message.\n\n";
  *filename = NULL;
  if (argc > 1) {
    for (int i = 1; i < argc; ++i) {
      char *argument = argv[i];
      if (strcmp(argument, "--") == 0) break;
      if (strcmp(argument, "-h") == 0 || strcmp(argument, "--help") == 0) {
	printf(usage, argv[0]);
	printf(help_msg);
	exit(EXIT_FAILURE);
      }
      else if (argument[0] == '-') {
	printf("Unrecognised argument %s\n", argument);
	printf(usage, argv[0]);
	printf("For help type '%s -h'.", argv[0]);
	exit(EXIT_FAILURE);
      }
    }
    *filename = argv[1];
  }  
}

