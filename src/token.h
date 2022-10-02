#ifndef TOKEN_H
#define TOKEN_H

#include <stdio.h>

enum token_type {
  ARROW_LEFT,
  ARROW_RIGHT,
  BANG,
  BKT_CURLY_LEFT,
  BKT_CURLY_RIGHT,
  BKT_SQUARE_LEFT,
  BKT_SQUARE_RIGHT,
  CARET,
  COLON,
  COMMA,
  DOT,
  HASH,
  MINUS,
  PERCENT,
  PLUS,
  STAR,
  WHIRLPOOL,
};

struct token {
  enum token_type type;
  struct {
    int row, col;
  } pos;
};

const char *token_type_to_string(enum token_type type);
const char *token_to_string(struct token token);

const char *token_type_to_symbol(enum token_type type);
const char *token_to_symbol(struct token token);

void print_token(struct token token);
void print_token_symbol(struct token token);

#endif
