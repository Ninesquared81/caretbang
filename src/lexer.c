#include "lexer.h"
#include "sizelims.h"

ssize_t lex(char *source, struct token tokens[TOKENS_MAX]) {
  ssize_t n = 0;
  int r = 0;
  int c = 0;
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
      ++c;
      /* fallthrough */
    default:
      /* skip adding to tokens array */
      continue;
    }
    tokens[n++] = (struct token) {.type = type, .pos = {r, c}};
    ++r;
  }
  if (n >= TOKENS_MAX) return -1;
  return n;
}
