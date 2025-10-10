#ifndef svm_chunk_h
#define svm_chunk_h

#include "common.h"
#include "memory.h"
#include "value.h"

typedef enum {
  OP_RETURN,
  OP_NEGATE,
  OP_SUBTRACT,
  OP_ADD,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_NOT,
  OP_MODULO,
  OP_CONSTANT,
  OP_CONSTANT_LONG,
  OP_NIL,
  OP_TRUE,
  OP_FALSE,
  OP_EQUAL,
  OP_GREATER,
  OP_LESS,
  OP_PRINT,
  OP_POP,
  OP_DEFINE_GLOBAL,
  OP_GET_GLOBAL,
  OP_SET_GLOBAL
} OpCode;

typedef struct {
  int length;
  int capacity;
  int *values;
} LineStartArray;

DECLARE_CONTAINER_FUNCTIONS(int, LineStartArray);

typedef struct {
  int length;
  int capacity;
  uint8_t *code;
  ValueArray constants;
  LineStartArray lines;
} Chunk;

void initChunk(Chunk *chunk);
void writeChunk(Chunk *chunk, uint8_t byte, int line, int offset);
void freeChunk(Chunk *chunk);
int addConstant(Chunk *chunk, Value value);
void writeConst(Chunk *chunk, Value value, int line);

int getLine(LineStartArray *arr, int pos);

#endif
