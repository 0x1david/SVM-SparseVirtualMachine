#ifndef svm_lexer_h
#define svm_lexer_h

#include "memory.h"

typedef struct {
  const char *start;
  const char *current;
  Trie *keywords;
  int line;
} Lexer;

void initLexer(Lexer *l, const char *src);
void freeLexer(Lexer *l);

typedef struct {
  TokType type;
  const char *start;
  int length;
  int line;
} Tok;

Tok lexTok(Lexer *l);

#endif
