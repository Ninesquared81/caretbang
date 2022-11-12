#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "debug.h"

int compiler_error_function(const char *restrict file, const char *restrict func,
			    const int line, const char *restrict msg, ...) {
  fprintf(stderr, "[COMPILER ERROR] ");

  va_list args;
  va_start(args, msg);
  fprintf(stderr, msg, args);
  va_end(args);
  
  fprintf(stderr,
	  "\n..in function %s()\n..in file '%s'\n..on line %d\n\n"
	  "NOTE: This is a compiler bug, not an issue with your program.\n",
	  func, file, line);
  
  return EXIT_FAILURE;
}
