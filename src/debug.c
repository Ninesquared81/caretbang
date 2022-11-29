#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "debug.h"

int compiler_error_function(const char *restrict file, const char *restrict func,
			    const int line, const char *restrict msg, ...) {
    va_list args;
    va_start(args, msg);
    int ret = v_compiler_error_function(file, func, line, msg, args);
    va_end(args);
    return ret;
}

int v_compiler_error_function(const char *restrict file, const char *restrict func,
			      const int line, const char *restrict msg, va_list args) {
    fprintf(stderr, "[COMPILER ERROR] ");
    vfprintf(stderr, msg, args);
    fprintf(stderr,
	    "\n..in function %s()\n..in file '%s'\n..on line %d\n\n"
	    "NOTE: This is a compiler bug, not an issue with your program.\n",
	    func, file, line);
  
    return EXIT_FAILURE;
}

int memory_error_function(const char *restrict file, const char *restrict func,
			  const int line, const char *restrict msg, ...) {
    va_list args;
    va_start(args, msg);
    int ret = v_memory_error_function(file, func, line, msg, args);
    va_end(args);
    return ret;
}

int compiler_limit_function(const char *restrict file, const char *restrict func,
			    const int line, const char *restrict msg, ...) {
    va_list args;
    va_start(args, msg);
    int ret = v_compiler_limit_function(file, func, line, msg, args);
    return ret;
}

int v_memory_error_function(const char *restrict file, const char *restrict func,
			    const int line, const char *restrict msg, va_list args) {
    fprintf(stderr, "[MEMORY ERROR] ");
    vfprintf(stderr, msg, args);
    fprintf(stderr,
	    "\n..in function %s()\n..in file '%s'\n..on line %d\n\n"
	    "NOTE: This may be a bug in the compiler\n.",
	    func, file, line);
    
    return EXIT_FAILURE;
}

int v_compiler_limit_function(const char *restrict file, const char *restrict func,
			      const int line, const char *restrict msg, va_list args) {
    fprintf(stderr, "[COMPILER LIMIT] ");
    vfprintf(stderr, msg, args);
    fprintf(stderr,
	    "\n..in function %s()\n..in file '%s'\n..on line %d\n\n"
	    "NOTE: This is a limit set by the compiler. If it is unreasonably low, it may be a bug",
	    func, file, line);

    return EXIT_FAILURE;
}
