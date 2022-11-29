#ifndef DEBUG_H
#define DEBUG_H

#include <stdarg.h>

#define compiler_error(/*MSG,*/ ...) \
    compiler_error_function(__FILE__, __func__, __LINE__, /*MSG,*/ __VA_ARGS__)

#define memory_error(/*MSG,*/ ...) \
    memory_error_function(__FILE__, __func__, __LINE__, /*MSG,*/ __VA_ARGS__)

#define compiler_limit(/*MSG,*/ ...) \
    compiler_limit_function(__FILE__, __func__, __LINE__, /*MSG,*/ __VA_ARGS__)


/* full function versions */
int compiler_error_function(const char *restrict file, const char *restrict func,
			    const int line, const char *restrict msg, ...);

int memory_error_function(const char *restrict file, const char *restrict func,
			    const int line, const char *restrict msg, ...);

int compiler_limit_function(const char *restrict file, const char *restrict func,
			    const int line, const char *restrict msg, ...);


/* va_list versions */
int v_compiler_error_function(const char *restrict file, const char *restrict func,
			      const int line, const char *restrict msg, va_list args);

int v_memory_error_function(const char *restrict file, const char *restrict func,
			    const int line, const char *restrict msg, va_list args);

int v_compiler_limit_function(const char *restrict file, const char *restrict func,
			      const int line, const char *restrict msg, va_list args);

#endif
