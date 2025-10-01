#include "lexer.h"
#include "memory.h"
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

void addKeywords(Trie *t) {
  trieInsert(t, "and", TOK_AND);
  trieInsert(t, "class", TOK_CLASS);
  trieInsert(t, "else", TOK_ELSE);
  trieInsert(t, "false", TOK_FALSE);
  trieInsert(t, "for", TOK_FOR);
  trieInsert(t, "fun", TOK_FUN);
  trieInsert(t, "if", TOK_IF);
  trieInsert(t, "nil", TOK_NIL);
  trieInsert(t, "or", TOK_OR);
  trieInsert(t, "print", TOK_PRINT);
  trieInsert(t, "return", TOK_RETURN);
  trieInsert(t, "super", TOK_SUPER);
  trieInsert(t, "this", TOK_THIS);
  trieInsert(t, "true", TOK_TRUE);
  trieInsert(t, "var", TOK_VAR);
  trieInsert(t, "while", TOK_WHILE);
}

void initLexer(Lexer *l, const char *src) {
  l->start = src;
  l->current = src;
  l->keywords = trieNew();
  l->line = 1;
  addKeywords(l->keywords);
}

void freeLexer(Lexer *l) { trieFree(l->keywords); }

static char advance(Lexer *l) {
  l->current++;
  return l->current[-1];
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

static char peek(Lexer *l) { return *l->current; }

static void skipWhitespace(Lexer *l) {
  for (;;) {
    char c = peek(l);
    switch (c) {
      case ' ':
      case '\r':
      case '\n':
      case '\t': advance(l); break;
      case '/':
        if (l->current[1] == '/') {
          while (peek(l) != '\n' && !isAtEnd(l)) {
            advance(l);
          }
        } else {
          return;
        }
        break;
      default: return;
    }
  }
}

Tok lexTok(Lexer *l) {
  skipWhitespace(l);
  l->start = l->current;

  if (isAtEnd(l)) return makeTok(l, TOK_EOF);

  char c = advance(l);

  switch (c) {
    case '/': return makeTok(l, TOK_SLASH);
    case '(': return makeTok(l, TOK_LEFT_PAREN);
    case ')': return makeTok(l, TOK_RIGHT_PAREN);
    case '{': return makeTok(l, TOK_LEFT_BRACE);
    case '}': return makeTok(l, TOK_RIGHT_BRACE);
    case ',': return makeTok(l, TOK_COMMA);
    case '.': return makeTok(l, TOK_DOT);
    case '-': return makeTok(l, TOK_MINUS);
    case '+': return makeTok(l, TOK_PLUS);
    case ';': return makeTok(l, TOK_SEMICOLON);
    case '*': return makeTok(l, TOK_STAR);
    case '!': return makeTok(l, matches(l, '=') ? TOK_BANG_EQUAL : TOK_BANG);
    case '=': return makeTok(l, matches(l, '=') ? TOK_EQUAL_EQUAL : TOK_EQUAL);
    case '<': return makeTok(l, matches(l, '=') ? TOK_LESS_EQUAL : TOK_LESS);
    case '>':
      return makeTok(l, matches(l, '=') ? TOK_GREATER_EQUAL : TOK_GREATER);

    case '"':
      while (*l->current != '"') {
        if (isAtEnd(l)) { return errorTok(l, "Unexpected stream end."); }
        l->current++;
      }
      advance(l);
      return makeTok(l, TOK_STRING);

    case 'a' ... 'z':
    case 'A' ... 'Z':
    case '_':
      while (isalnum(*l->current) || *l->current == '_') {
        l->current++;
      }
      TokType tt = trieFind(l->keywords, l->start, l->current - l->start);
      return makeTok(l, tt);

    case '0' ... '9':
      bool seen_dot = false;
      while (*l->current == '.' || isdigit(*l->current)) {
        if (seen_dot && *l->current == '.') {
          return errorTok(l, "Invalid number literal");
        }
        seen_dot = *l->current == '.' || seen_dot;
        l->current++;
      }
      return l->current[-1] != '.' ? makeTok(l, TOK_NUMBER)
                                   : errorTok(l, "Invalid number literal");
  }

  return errorTok(l, "Unexpected character");
}
