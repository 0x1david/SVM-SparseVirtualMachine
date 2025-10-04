#include "compiler.h"
#include "chunk.h"
#include "debug.h"
#include "lexer.h"
#include "memory.h"
#include <stdlib.h>

static ParseRule rules[TOK_EOF + 1];

Chunk *compilingChunk;
static Chunk *currentChunk() { return compilingChunk; }

static void errorAt(Parser *parser, Tok *tok, const char *msg) {
  if (parser->isPanicing) return;
  parser->isPanicing = true;
  fprintf(stderr, "[line %d] Error", tok->line);

  if (tok->type == TOK_EOF) {
    fprintf(stderr, " at end");
  } else if (tok->type == TOK_ERROR) {
    // TBD
  } else {
    fprintf(stderr, " at '%.*s'", tok->length, tok->start);
  }
  fprintf(stderr, ": %s\n", msg);
  parser->hadError = true;
}

static void error(Parser *parser, const char *msg) {
  errorAt(parser, &parser->previous, msg);
}

static void errorAtCurrent(Parser *parser, const char *msg) {
  errorAt(parser, &parser->current, msg);
}

static void advance(Parser *parser, Lexer *lexer) {
  parser->previous = parser->current;
  for (;;) {
    parser->current = lexTok(lexer);
    if (parser->current.type != TOK_ERROR) break;

    errorAtCurrent(parser, parser->current.start);
  }
}

static void consume(Parser *parser, Lexer *lexer, TokType type,
                    const char *msg) {
  if (parser->current.type == type) {
    advance(parser, lexer);
    return;
  }

  errorAtCurrent(parser, msg);
}

static void emitByte(Parser *parser, uint8_t byte) {
  writeChunk(currentChunk(), byte, parser->previous.line, 0); // TODO: offset
}

static void emitReturn(Parser *parser) { emitByte(parser, OP_RETURN); }
static uint8_t makeConstant(Parser *parser, Value value) {
  int constant = addConstant(currentChunk(), value);
  if (constant > UINT16_MAX) {
    error(parser, "Too many constants in one chunk.");
    return 0;
  }
  return (uint8_t)constant;
}
static void emitBytes(Parser *parser, int8_t byte1, int8_t byte2) {
  emitByte(parser, byte1);
  emitByte(parser, byte2);
}

static void endCompiler(Parser *parser) {
  emitReturn(parser);
#ifdef DEBUG_PRINT_CODE
  if (!parser->hadError) { disassembleChunk(currentChunk(), "code"); }
#endif
}

static ParseRule *getRule(TokType type) { return &rules[type]; }

static void parsePrecedence(Parser *parser, Lexer *lexer,
                            Precedence precedence) {
  advance(parser, lexer);
  ParseFn prefixRule = getRule(parser->previous.type)->prefix;
  if (prefixRule == NULL) {
    error(parser, "Expect expression.");
    return;
  }

  prefixRule();

  while (precedence <= getRule(parser->current.type)->precedence) {
    advance(parser, lexer);
    ParseFn infixRule = getRule(parser->previous.type)->infix;
    infixRule();
  }
}

static void expression(Parser *parser, Lexer *lexer) {
  parsePrecedence(parser, lexer, PREC_ASSIGNMENT);
}
static void grouping(Parser *parser, Lexer *lexer) {
  expression(parser, lexer);
  consume(parser, lexer, TOK_RIGHT_PAREN, "Expect `)` after expression.");
}
static void emitConstant(Parser *parser, Value value) {
  emitBytes(parser, OP_CONSTANT, makeConstant(parser, value));
}
static void number(Parser *parser, Lexer *lexer) {
  double value = strtod(parser->previous.start, NULL);
  emitConstant(parser, value);
}
static void unary(Parser *parser, Lexer *lexer) {
  TokType opType = parser->previous.type;

  expression(parser, lexer);
  parsePrecedence(parser, lexer, PREC_UNARY);

  switch (opType) {
    case TOK_MINUS: emitByte(parser, OP_NEGATE); break;
    default: return;
  }
}

static void binary(Parser *parser, Lexer *lexer) {
  TokType opType = parser->previous.type;
  ParseRule *rule = getRule(opType);
  parsePrecedence(parser, lexer, (Precedence)rule->precedence + 1);

  switch (opType) {
    case TOK_PLUS: emitByte(parser, OP_ADD); break;
    case TOK_MINUS: emitByte(parser, OP_SUBTRACT); break;
    case TOK_STAR: emitByte(parser, OP_MULTIPLY); break;
    case TOK_SLASH: emitByte(parser, OP_DIVIDE); break;
    default: return;
  }
}

bool compile(const char *src, Chunk *chunk) {
  Lexer lexer;
  Parser parser;
  initLexer(&lexer, src);
  compilingChunk = chunk;

  parser.hadError = false;
  parser.isPanicing = false;

  advance(&parser, &lexer);
  expression(&parser, &lexer);
  consume(&parser, &lexer, TOK_EOF, "Expect end of expression");
  endCompiler(&parser);
  return !parser.hadError;
}

static ParseRule rules[] = {
    [TOK_LEFT_PAREN] = {grouping, NULL, PREC_NONE},
    [TOK_RIGHT_PAREN] = {NULL, NULL, PREC_NONE},
    [TOK_LEFT_BRACE] = {NULL, NULL, PREC_NONE},
    [TOK_RIGHT_BRACE] = {NULL, NULL, PREC_NONE},
    [TOK_COMMA] = {NULL, NULL, PREC_NONE},
    [TOK_DOT] = {NULL, NULL, PREC_NONE},
    [TOK_MINUS] = {unary, binary, PREC_TERM},
    [TOK_PLUS] = {NULL, binary, PREC_TERM},
    [TOK_SEMICOLON] = {NULL, NULL, PREC_NONE},
    [TOK_BANG] = {NULL, NULL, PREC_NONE},
    [TOK_BANG_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOK_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOK_EQUAL_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOK_GREATER] = {NULL, NULL, PREC_NONE},
    [TOK_GREATER_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOK_LESS] = {NULL, NULL, PREC_NONE},
    [TOK_LESS_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOK_IDENTIFIER] = {NULL, NULL, PREC_NONE},
    [TOK_NUMBER] = {number, NULL, PREC_NONE},
    [TOK_AND] = {NULL, NULL, PREC_NONE},
    [TOK_CLASS] = {NULL, NULL, PREC_NONE},
    [TOK_ELSE] = {NULL, NULL, PREC_NONE},
    [TOK_FALSE] = {NULL, NULL, PREC_NONE},
    [TOK_FOR] = {NULL, NULL, PREC_NONE},
    [TOK_FUN] = {NULL, NULL, PREC_NONE},
    [TOK_IF] = {NULL, NULL, PREC_NONE},
    [TOK_NIL] = {NULL, NULL, PREC_NONE},
    [TOK_OR] = {NULL, NULL, PREC_NONE},
    [TOK_PRINT] = {NULL, NULL, PREC_NONE},
    [TOK_RETURN] = {NULL, NULL, PREC_NONE},
    [TOK_SUPER] = {NULL, NULL, PREC_NONE},
    [TOK_THIS] = {NULL, NULL, PREC_NONE},
    [TOK_TRUE] = {NULL, NULL, PREC_NONE},
    [TOK_VAR] = {NULL, NULL, PREC_NONE},
    [TOK_WHILE] = {NULL, NULL, PREC_NONE},
    [TOK_ERROR] = {NULL, NULL, PREC_NONE},
    [TOK_EOF] = {NULL, NULL, PREC_NONE},
};
