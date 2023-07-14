#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

#include <stdbool.h>

void parse_cmd(int argc, char **argv, char **filename, bool *should_print_stacks);
void print_usage_error(char *program_name);

#endif
