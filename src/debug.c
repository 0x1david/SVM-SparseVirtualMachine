#include "debug.h"
#include "chunk.h"
#include <stdio.h>

static int simpleInstruction(const char *name, int offset) {
  printf("%s\n", name);
  return offset + 1;
}

static int constantInstruction(const char *name, Chunk *chunk, int offset) {
  uint8_t constant = chunk->code[offset + 1];
  printf("%-16s %4d '", name, constant);
  printValue(chunk->constants.values[constant]);
  printf("'\n");
  return offset + 2;
}

static int constantLongInstruction(const char *name, Chunk *chunk, int offset) {
  uint16_t constant = 0;
  constant |= chunk->code[offset + 1];
  constant |= chunk->code[offset + 2] << 8;
  printf("%-16s %4d '", name, constant);
  printValue(chunk->constants.values[constant]);
  printf("'\n");
  return offset + 3;
}

void disassembleChunk(Chunk *chunk, const char *name) {
  printf("== %s ==\n", name);
  for (int offset = 0; offset < chunk->length;) {
    offset = disassembleInstruction(chunk, offset);
  }
}

int disassembleInstruction(Chunk *chunk, int offset) {

  if (offset > 0 &&
      getLine(&chunk->lines, offset) == getLine(&chunk->lines, offset - 1)) {
    printf("   | ");
  } else {
    printf("%04d ", getLine(&chunk->lines, offset));
  }

  uint8_t instruction = chunk->code[offset];
  switch (instruction) {
    case OP_RETURN: return simpleInstruction("OP_RETURN", offset);
    case OP_CONSTANT: return constantInstruction("OP_CONSTANT", chunk, offset);
    case OP_CONSTANT_LONG:
      return constantLongInstruction("OP_CONSTANT_LONG", chunk, offset);
    case OP_NEGATE: return simpleInstruction("OP_NEGATE", offset);
    case OP_ADD: return simpleInstruction("OP_PLUS", offset);
    case OP_SUBTRACT: return simpleInstruction("OP_MINUS", offset);
    case OP_MULTIPLY: return simpleInstruction("OP_MULTIPLY", offset);
    case OP_DIVIDE: return simpleInstruction("OP_DIVIDE", offset);
    case OP_MODULO: return simpleInstruction("OP_MODULO", offset);
    default: printf("Unknown opcode %d\n", instruction); return offset + 1;
  }
}
