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
  OP_MODULO,
  OP_CONSTANT,
  OP_CONSTANT_LONG
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
