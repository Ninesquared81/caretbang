#include "command_line.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *usage = "Usage: %s [options] <filename>\n";
static const char *help_msg =
    "Positional arguments:\n"
    "filename -- name of source code file to interpret.\n"
    "Options:\n"
    "-h, --help -- show this help message.\n"
;

void parse_cmd(int argc, char **argv, char **filename) {
    if (argc <= 1) {
	fprintf(stderr,
		"Error: missing positional argument 'filename'.\n"
	        "Interpretation failed.\n");
	print_usage_error(argv[0]);
	exit(EXIT_FAILURE);
    }
    int i;
    for (i = 1; i < argc; ++i) {
	char *argument = argv[i];
	if (strcmp(argument, "--") == 0) break;
	if (strcmp(argument, "-h") == 0 || strcmp(argument, "--help") == 0) {
	    printf(usage, argv[0]);
	    printf("%s\n", help_msg);
	    exit(EXIT_SUCCESS);
	}
	else if (argument[0] == '-') {
	    fprintf(stderr, "Unrecognised argument '%s'\n", argument);
	    print_usage_error(argv[0]);
	    exit(EXIT_FAILURE);
	}
    }
    *filename = argv[i-1];
}

void print_usage_error(char *program_name) {
    fprintf(stderr, usage, program_name);
    fprintf(stderr, "For help type '%s -h'.", program_name);
}
