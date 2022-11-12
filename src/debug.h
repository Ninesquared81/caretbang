#ifndef DEBUG_H
#define DEBUG_H

#define compiler_error(MSG, ...) compiler_error_function(__FILE__, __func__, __LINE__, MSG, __VA_ARGS__)
#define memory_error(MSG, ...)     memory_error_function(__FILE__, __func__, __LINE__, MSG, __VA_ARGS__)
#define compiler_limit(MSG, ...) compiler_limit_function(__FILE__, __func__, __LINE__, MSG, __VA_ARGS__)


int compiler_error_function(const char *restrict file, const char *restrict func,
			    const int line, const char *restrict msg, ...);

int memory_error_function(const char *restrict file, const char *restrict func,
			    const int line, const char *restrict msg, ...);

int compiler_limit_function(const char *restrict file, const char *restrict func,
			    const int line, const char *restrict msg, ...);

#endif
