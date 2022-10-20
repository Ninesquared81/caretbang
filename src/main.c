#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sizelims.h"
#include "stack.h"
#include "parser.h"
#include "interpreter.h"


static char *input_buffer = NULL;
static struct ast_node *ast = NULL;


void kill_input_buffer(void);
void kill_ast(void);

void run(void);
void parse_cmd(int argc, char **argv, char **filename);

int main(int argc, char **argv) {
  atexit(kill_input_buffer);
  atexit(kill_ast);
  
  char *filename;
  parse_cmd(argc, argv, &filename);

  input_buffer = calloc(TOKENS_MAX, sizeof *input_buffer);
  if (!input_buffer) {
    fprintf(stderr, "Could not allocate input_buffer!\n");
    return EXIT_FAILURE;
  }

  ast = calloc(AST_MAX, sizeof *ast);
  if (!ast) {
    fprintf(stderr, "Could not allocate ast!\n");
    return EXIT_FAILURE;
  }
  
  if (filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
      fprintf(stderr, "Could not open file '%s'.\n", filename);
      return EXIT_FAILURE;
    }
    fread(input_buffer, sizeof *input_buffer, sizeof input_buffer - sizeof *input_buffer, fp);
    fclose(fp);
    run();
  }
  else {
    while (fgets(input_buffer, sizeof input_buffer, stdin)) {
      run();
    }
  }

  return EXIT_SUCCESS;
}

void run(void) {
  size_t ast_length = parse(tokens, tokens_length, ast);
  interpret(ast, ast_length);
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

void kill_input_buffer(void) {
  if (input_buffer) free(input_buffer);
  input_buffer = NULL;
}

void kill_ast(void) {
  if (ast) free(ast);
  tokens = NULL;
}
