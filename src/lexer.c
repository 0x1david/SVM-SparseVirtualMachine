#include "lexer.h"
#include <stdbool.h>
#include <string.h>

void initLexer(Lexer *l, const char *src) {
  l->start = src;
  l->current = src;
}

static bool isAtEnd(Lexer *l) { return *l->current == '\0'; };

Tok makeTok(Lexer *l, TokType tt) {
  Tok t;
  t.start = l->start;
  t.type = tt;
  t.length = l->current - l->start;
  t.line = l->line;
  return t;
}

Tok errorTok(Lexer *l, const char *msg) {
  Tok t;
  t.start = msg;
  t.type = TOK_ERROR;
  t.length = (int)strlen(msg);
  t.line = l->line;
  return t;
}

static bool matches(Lexer *l, char c) {
  if (isAtEnd(l)) return false;
  if (*l->current != c) return false;

  l->current++;
  return true;
};

Tok lexTok(Lexer *l) {
  l->start = l->current;

  if (isAtEnd(l)) return makeTok(l, TOK_EOF);

  switch (*l->current) {
    // Single-char
    case '(': return makeTok(l, TOK_LEFT_PAREN);
    case ')': return makeTok(l, TOK_RIGHT_PAREN);
    case '{': return makeTok(l, TOK_LEFT_BRACE);
    case '}': return makeTok(l, TOK_RIGHT_BRACE);
    case ',': return makeTok(l, TOK_COMMA);
    case '.': return makeTok(l, TOK_DOT);
    case '-': return makeTok(l, TOK_MINUS);
    case '+': return makeTok(l, TOK_PLUS);
    case ';': return makeTok(l, TOK_SEMICOLON);
    case '/': return makeTok(l, TOK_SLASH);
    case '*': return makeTok(l, TOK_STAR);
    case '!': return makeTok(l, matches(l, '=') ? TOK_BANG_EQUAL : TOK_BANG);
    case '=': return makeTok(l, matches(l, '=') ? TOK_EQUAL_EQUAL : TOK_EQUAL);
    case '>':
      return makeTok(l, matches(l, '=') ? TOK_GREATER_EQUAL : TOK_GREATER);
    case '<': return makeTok(l, matches(l, '=') ? TOK_LESS_EQUAL : TOK_LESS);
  }
  //   // Literals
  //   TOK_IDENTIFIER,
  //   TOK_STRING,
  //   TOK_NUMBER,
  //   // Keywords
  //   TOK_AND,
  //   TOK_CLASS,
  //   TOK_ELSE,
  //   TOK_FALSE,
  //   TOK_FOR,
  //   TOK_FUN,
  //   TOK_IF,
  //   TOK_NIL,
  //   TOK_OR,
  //   TOK_PRINT,
  //   TOK_RETURN,
  //   TOK_SUPER,
  //   TOK_THIS,
  //   TOK_TRUE,
  //   TOK_VAR,
  //   TOK_WHILE,
  //   // Special
  //   TOK_ERROR,
  //   TOK_EOF
  // } TokType;

  return errorTok(l, "Unexpected character.");
}
