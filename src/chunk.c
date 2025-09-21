#include "chunk.h"
#include "memory.h"

// typedef struct {
//   int count;
//   int capacity;
//   uint8_t *code;
// } Chunk;

void initChunk(Chunk *chunk) {
  chunk->count = 0;
  chunk->capacity = 0;
  chunk->code = NULL;
}

void writeChunk(Chunk *chunk, uint8_t byte) {
  if (chunk->capacity == chunk->count) {
    int newCap = GROW_CAPACITY(chunk->capacity);
    chunk->code = GROW_ARRAY(uint8_t, chunk->code, chunk->capacity, newCap);
    chunk->capacity = newCap;
  }
  chunk->code[chunk->count] = byte;
  chunk->count++;
}

void freeChunk(Chunk *chunk) {
  FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
  initChunk(chunk);
}
