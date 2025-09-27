#include "chunk.h"
#include "memory.h"
#include "value.h"

void initChunk(Chunk *chunk) {
  chunk->length = 0;
  chunk->capacity = 0;
  chunk->code = NULL;
  initValueArray(&chunk->constants);
  initLineStartArray(&chunk->lines);
}

void writeChunk(Chunk *chunk, uint8_t byte, int line, int offset) {
  if (chunk->capacity == chunk->length) {
    int newCap = GROW_CAPACITY(chunk->capacity);
    chunk->code = GROW_ARRAY(uint8_t, chunk->code, chunk->capacity, newCap);
    chunk->capacity = newCap;
  }
  chunk->code[chunk->length] = byte;
  chunk->length++;

  if (line > chunk->lines.length) {
    writeLineStartArray(&chunk->lines, offset);
  }
}

void freeChunk(Chunk *chunk) {
  FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
  freeValueArray(&chunk->constants);
  freeLineStartArray(&chunk->lines);
  initChunk(chunk);
}

int addConst(Chunk *chunk, Value value) {
  writeValueArray(&chunk->constants, value);
  return chunk->constants.length - 1;
}
void writeConst(Chunk *chunk, Value value, int line) {
  int constantIndex = addConst(chunk, value);

  if (constantIndex <= 255) {
    writeChunk(chunk, OP_CONSTANT, line, 0);
    writeChunk(chunk, (uint8_t)constantIndex, line, 0);
  } else {
    writeChunk(chunk, OP_CONSTANT_LONG, line, 0);
    writeChunk(chunk, (uint8_t)(constantIndex & 0xFF), line, 0);
    writeChunk(chunk, (uint8_t)((constantIndex >> 8) & 0xFF), line, 0);
  }
}

IMPLEMENT_CONTAINER_FUNCTIONS(int, LineStartArray)

/**
 * @brief Finds the line number for a given bytecode position.
 *
 * Performs binary search on the line start array to find which
 * source line contains the bytecode at the given position.
 *
 * @return Line number (0-indexed), or -1 if not found
 *
 * @warning Returns -1 for invalid positions or empty arrays
 */
int getLine(LineStartArray *arr, int offset) {
  int start = 0;
  int end = arr->length - 1;
  if (offset < 0 || arr->length == 0) {
    return -1;
  }

  if (arr->length == 1) {
    return (offset >= arr->values[0]) ? 1 : -1;
  }

  while (start <= end) {
    int middle = (start + end) / 2;
    int middle_el = arr->values[middle];

    if (offset < middle_el) {
      if (middle > 0 && offset >= arr->values[middle - 1]) {
        return middle;
      }

      end = middle - 1;
      continue;
    }

    else {
      if (middle == arr->length - 1 || offset < arr->values[middle + 1]) {
        return middle + 1;
      }
      start = middle + 1;
    }
  }
  return -1;
}
