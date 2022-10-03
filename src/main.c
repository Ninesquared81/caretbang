#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "lexer.h"
#include "sizelims.h"
#include "stack.h"


int main(int argc, char **argv) {
  (void)argc; (void)argv;
  struct token *tokens = malloc(TOKENS_MAX * sizeof *tokens);
  ssize_t n = lex("^!", tokens);
  if (n == -1) {
    fprintf(stderr, "Maximum token limit reached.\n");
    return EXIT_FAILURE;
  }
  for (int i = 0; i < n; ++i) {
    print_token(tokens[i]);
    printf(" ");
  }
  return EXIT_SUCCESS;
}
