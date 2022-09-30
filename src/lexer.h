#ifndef LEXER_H
#define LEXER_H

#include <sys/types.h>
#include "token.h"

ssize_t lex(char *source, struct token tokens[]);

#endif
