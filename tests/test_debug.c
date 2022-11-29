#include <stdio.h>
#include <stdlib.h>

#include "../src/debug.h"

int main(void) {
  int the_answer = 42;
  int code = compiler_error("The error occurred (%d).", the_answer);
  printf("The exit code was %d", code);
  return EXIT_SUCCESS;
}
