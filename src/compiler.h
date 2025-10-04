#ifndef svm_compiler_h
#define svm_compiler_h

#include "chunk.h"
#include "lexer.h"
bool compile(const char *src, Chunk *chunk);

typedef struct {
  Tok current;
  Tok previous;
  bool hadError;
  bool isPanicing;
} Parser;

typedef enum {
  PREC_NONE,
  PREC_ASSIGNMENT,
  PREC_OR,
  PREC_AND,
  PREC_EQUALITY,
  PREC_COMPARISON,
  PREC_TERM,
  PREC_FACTOR,
  PREC_UNARY,
  PREC_CALL,
  PREC_PRIMARY,
} Precedence;

typedef void (*ParseFn)();

typedef struct {
  ParseFn prefix;
  ParseFn infix;
  Precedence precedence;
} ParseRule;

#endif
