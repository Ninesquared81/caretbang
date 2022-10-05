#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "sizelims.h"

size_t lex(char *source, struct token tokens[TOKENS_MAX]) {
  size_t n = 0;
  int row = 0;
  int col = 0;
  char c;
  while ((c = *source++) && n < TOKENS_MAX) {
    enum token_type type;
    switch (c) {
    case '<':
      type = ARROW_LEFT;
      break;
    case '>':
      type = ARROW_RIGHT;
      break;
    case '!':
      type = BANG;
      break;
    case '{':
      type = BKT_CURLY_LEFT;
      break;
    case '}':
      type = BKT_CURLY_RIGHT;
      break;
    case '[':
      type = BKT_SQUARE_LEFT;
      break;
    case ']':
      type = BKT_SQUARE_RIGHT;
      break;
    case '^':
      type = CARET;
      break;
    case ':':
      type = COLON;
      break;
    case ',':
      type = COMMA;
      break;
    case '.':
      type = DOT;
      break;
    case '#':
      type = HASH;
      break;
    case '-':
      type = MINUS;
      break;
    case '%':
      type = PERCENT;
      break;
    case '+':
      type = PLUS;
      break;
    case '*':
      type = STAR;
      break;
    case '@':
      type = WHIRLPOOL;
      break;
    case '\n':
      ++row;
      col = -1;
      /* fallthrough */
    default:
      /* skip adding to tokens array */
      ++col;
      continue;
    }
    tokens[n++] = (struct token) {.type = type, .pos = {row, col++}};
  }
  if (n >= TOKENS_MAX) {
    fprintf(stderr, "Token limit exceeded.\n");
    exit(EXIT_FAILURE);
  };
  return n;
}
