#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lexer.h"
#include "sizelims.h"


int main(int argc, char **argv) {
  (void)argc; (void)argv;
  struct token tokens[TOKENS_MAX];
  ssize_t n = lex("", tokens);
  if (n == -1) {
    fprintf(stderr, "Maximum token limit reached.\n");
    exit(EXIT_FAILURE);
  }
  return EXIT_SUCCESS;
}
