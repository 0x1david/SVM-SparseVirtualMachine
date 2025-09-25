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

IMPLEMENT_ARRAY_FUNCTIONS(int, LineStart)

int getLine(LineStartArray *arr, int pos) {
  int start = 0;
  int end = arr->length - 1;
  if (pos < 0 || arr->length == 0) {
    return -1;
  }

  if (arr->length == 1) {
    return (pos >= arr->values[0]) ? 0 : -1;
  }

  while (start <= end) {
    int middle = (start + end) / 2;
    int middle_el = arr->values[middle];

    if (pos < middle_el) {
      if (middle > 0 && pos >= arr->values[middle - 1]) {
        return middle - 1;
      }

      end = middle - 1;
      continue;
    }

    else {
      if (middle == arr->length - 1 || pos < arr->values[middle + 1]) {
        return middle;
      }
      start = middle + 1;
    }
  }
  return -1;
}
