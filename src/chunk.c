#include "chunk.h"
#include "memory.h"
#include "value.h"

void initChunk(Chunk *chunk) {
  chunk->length = 0;
  chunk->capacity = 0;
  chunk->code = NULL;
  initValueArray(&chunk->constants);
}

void writeChunk(Chunk *chunk, uint8_t byte) {
  if (chunk->capacity == chunk->length) {
    int newCap = GROW_CAPACITY(chunk->capacity);
    chunk->code = GROW_ARRAY(uint8_t, chunk->code, chunk->capacity, newCap);
    chunk->capacity = newCap;
  }
  chunk->code[chunk->length] = byte;
  chunk->length++;
}

void freeChunk(Chunk *chunk) {
  FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
  freeValueArray(&chunk->constants);
  initChunk(chunk);
}

int addConst(Chunk *chunk, Value value) {
  writeValueArray(&chunk->constants, value);
  return chunk->constants.length - 1;
}
