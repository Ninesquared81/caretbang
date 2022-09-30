#ifndef TOKEN_H
#define TOKEN_H

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

#endif
