#ifndef AST_H
#define AST_H


struct ast_node {
  enum {
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
  } type;
  size_t jump_index;
};

#endif
