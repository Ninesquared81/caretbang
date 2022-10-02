#include <stdlib.h>
#include <stdio.h>

#include "token.h"

const char *token_type_to_string(enum token_type type) {
  switch (type) {
  case ARROW_LEFT:
    return "ARROW_LEFT";
  case ARROW_RIGHT:
    return "ARROW_RIGHT";
  case BANG:
    return "BANG";
  case BKT_CURLY_LEFT:
    return "BKT_CURLY_LEFT";
  case BKT_CURLY_RIGHT:
    return "BKT_CURLY_RIGHT";
  case BKT_SQUARE_LEFT:
    return "BKT_SQUARE_LEFT";
  case BKT_SQUARE_RIGHT:
    return "BKT_SQUARE_RIGHT";
  case CARET:
    return "CARET";
  case COLON:
    return "COLON";
  case COMMA:
    return "COMMA";
  case DOT:
    return "DOT";
  case HASH:
    return "HASH";
  case MINUS:
    return "MINUS";
  case PERCENT:
    return "PERCENT";
  case PLUS:
    return "PLUS";
  case STAR:
    return "STAR";
  case WHIRLPOOL:
    return "WHIRLPOOL";
  default:
    return NULL;
  }
}

const char *token_type_to_symbol(enum token_type type) {
  switch (type) {
  case ARROW_LEFT:
    return "<";
  case ARROW_RIGHT:
    return ">";
  case BANG:
    return "!";
  case BKT_CURLY_LEFT:
    return "{";
  case BKT_CURLY_RIGHT:
    return "}";
  case BKT_SQUARE_LEFT:
    return "[";
  case BKT_SQUARE_RIGHT:
    return "]";
  case CARET:
    return "^";
  case COLON:
    return ":";
  case COMMA:
    return ",";
  case DOT:
    return ".";
  case HASH:
    return "#";
  case MINUS:
    return "-";
  case PERCENT:
    return "%";
  case PLUS:
    return "+";
  case STAR:
    return "*";
  case WHIRLPOOL:
    return "@";
  default:
    return NULL;
  }
}

const char *token_to_string(struct token token) {
  return token_type_to_string(token.type);
}

const char *token_to_symbol(struct token token) {
  return token_type_to_symbol(token.type);
}

void print_token(struct token token) {
  printf(token_to_string(token));
}

void print_token_symbol(struct token token) {
  printf(token_to_symbol(token));
}
