#include "chunk.h"
#include "debug.h"

int main(int argc, const char *argv[]) {
  Chunk ch;
  initChunk(&ch);

  int constant = addConst(&ch, 1.2);
  writeChunk(&ch, OP_CONSTANT, 123, 1);
  writeChunk(&ch, constant, 123, 2);

  writeChunk(&ch, OP_RETURN, 123, 3);
  disassembleChunk(&ch, "test chunk");
  freeChunk(&ch);
  return 0;
}
