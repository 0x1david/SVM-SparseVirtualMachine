#include "compiler.h"
#include "chunk.h"
#include "debug.h"
#include "lexer.h"
#include "memory.h"
#include "object.h"
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

static void parsePrecedence(VM *vm, Parser *parser, Lexer *lexer,
                            Precedence precedence) {
  advance(parser, lexer);
  ParseFn prefixRule = getRule(parser->previous.type)->prefix;
  if (prefixRule == NULL) {
    error(parser, "Expect expression.");
    return;
  }

  prefixRule(vm, parser, lexer);

  while (precedence <= getRule(parser->current.type)->precedence) {
    advance(parser, lexer);
    ParseFn infixRule = getRule(parser->previous.type)->infix;
    infixRule(vm, parser, lexer);
  }
}

static void expression(VM *vm, Parser *parser, Lexer *lexer) {
  parsePrecedence(vm, parser, lexer, PREC_ASSIGNMENT);
}
static void grouping(VM *vm, Parser *parser, Lexer *lexer) {
  expression(vm, parser, lexer);
  consume(parser, lexer, TOK_RIGHT_PAREN, "Expect `)` after expression.");
}
static void emitConstant(Parser *parser, Value value) {
  emitBytes(parser, OP_CONSTANT, makeConstant(parser, value));
}
static void number(VM *vm, Parser *parser, Lexer *lexer) {
  double value = strtod(parser->previous.start, NULL);
  emitConstant(parser, NUMBER_VAL(value));
}
static void unary(VM *vm, Parser *parser, Lexer *lexer) {
  TokType opType = parser->previous.type;

  parsePrecedence(vm, parser, lexer, PREC_UNARY);

  switch (opType) {
    case TOK_MINUS: emitByte(parser, OP_NEGATE); break;
    case TOK_BANG: emitByte(parser, OP_NOT); break;
    default: return;
  }
}

static void binary(VM *vm, Parser *parser, Lexer *lexer) {
  TokType opType = parser->previous.type;
  ParseRule *rule = getRule(opType);
  parsePrecedence(vm, parser, lexer, (Precedence)rule->precedence + 1);

  switch (opType) {
    case TOK_PLUS: emitByte(parser, OP_ADD); break;
    case TOK_MINUS: emitByte(parser, OP_SUBTRACT); break;
    case TOK_STAR: emitByte(parser, OP_MULTIPLY); break;
    case TOK_SLASH: emitByte(parser, OP_DIVIDE); break;
    case TOK_BANG: emitByte(parser, OP_NOT); break;
    case TOK_EQUAL_EQUAL: emitByte(parser, OP_EQUAL); break;
    case TOK_BANG_EQUAL: emitBytes(parser, OP_EQUAL, OP_NOT); break;
    case TOK_GREATER: emitByte(parser, OP_GREATER); break;
    case TOK_GREATER_EQUAL: emitBytes(parser, OP_LESS, OP_NOT); break;
    case TOK_LESS: emitByte(parser, OP_LESS); break;
    case TOK_LESS_EQUAL: emitBytes(parser, OP_GREATER, OP_NOT); break;
    default: return;
  }
}

static void literal(VM *vm, Parser *parser, Lexer *lexer) {
  switch (parser->previous.type) {
    case TOK_FALSE: emitByte(parser, OP_FALSE); break;
    case TOK_NIL: emitByte(parser, OP_NIL); break;
    case TOK_TRUE: emitByte(parser, OP_TRUE); break;
    default: return;
  }
}

static void string(VM *vm, Parser *parser, Lexer *lexer) {
  emitConstant(parser, OBJ_VAL(copyString(vm, parser->previous.start + 1,
                                          parser->previous.length - 2)));
};

bool compile(VM *vm, const char *src, Chunk *chunk) {
  Lexer lexer;
  Parser parser = {0};
  initLexer(&lexer, src);
  compilingChunk = chunk;

  advance(&parser, &lexer);
  expression(vm, &parser, &lexer);
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
    [TOK_STAR] = {NULL, binary, PREC_FACTOR},
    [TOK_SLASH] = {NULL, binary, PREC_FACTOR},
    [TOK_DOT] = {NULL, NULL, PREC_NONE},
    [TOK_MINUS] = {unary, binary, PREC_TERM},
    [TOK_PLUS] = {NULL, binary, PREC_TERM},
    [TOK_SEMICOLON] = {NULL, NULL, PREC_NONE},
    [TOK_BANG] = {unary, NULL, PREC_NONE},
    [TOK_BANG_EQUAL] = {NULL, binary, PREC_EQUALITY},
    [TOK_EQUAL] = {NULL, NULL, PREC_NONE},
    [TOK_EQUAL_EQUAL] = {NULL, binary, PREC_EQUALITY},
    [TOK_GREATER] = {NULL, binary, PREC_COMPARISON},
    [TOK_GREATER_EQUAL] = {NULL, binary, PREC_COMPARISON},
    [TOK_LESS] = {NULL, binary, PREC_COMPARISON},
    [TOK_LESS_EQUAL] = {NULL, binary, PREC_COMPARISON},
    [TOK_IDENTIFIER] = {NULL, NULL, PREC_NONE},
    [TOK_NUMBER] = {number, NULL, PREC_NONE},
    [TOK_AND] = {NULL, NULL, PREC_NONE},
    [TOK_CLASS] = {NULL, NULL, PREC_NONE},
    [TOK_ELSE] = {NULL, NULL, PREC_NONE},
    [TOK_FALSE] = {literal, NULL, PREC_NONE},
    [TOK_FOR] = {NULL, NULL, PREC_NONE},
    [TOK_FUN] = {NULL, NULL, PREC_NONE},
    [TOK_IF] = {NULL, NULL, PREC_NONE},
    [TOK_NIL] = {literal, NULL, PREC_NONE},
    [TOK_OR] = {NULL, NULL, PREC_NONE},
    [TOK_PRINT] = {NULL, NULL, PREC_NONE},
    [TOK_RETURN] = {NULL, NULL, PREC_NONE},
    [TOK_SUPER] = {NULL, NULL, PREC_NONE},
    [TOK_THIS] = {NULL, NULL, PREC_NONE},
    [TOK_TRUE] = {literal, NULL, PREC_NONE},
    [TOK_VAR] = {NULL, NULL, PREC_NONE},
    [TOK_WHILE] = {NULL, NULL, PREC_NONE},
    [TOK_ERROR] = {NULL, NULL, PREC_NONE},
    [TOK_EOF] = {NULL, NULL, PREC_NONE},
    [TOK_STRING] = {string, NULL, PREC_NONE}};
