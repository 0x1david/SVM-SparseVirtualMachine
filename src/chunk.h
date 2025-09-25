#ifndef svm_chunk_h
#define svm_chunk_h

#include "common.h"
#include "memory.h"
#include "value.h"

typedef enum { OP_RETURN, OP_CONSTANT } OpCode;

typedef struct {
  int length;
  int capacity;
  int *values;
} LineStartArray;

DECLARE_ARRAY_FUNCTIONS(int, LineStart);

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
int addConst(Chunk *chunk, Value value);

int getLine(LineStartArray *arr, int pos);

#endif
