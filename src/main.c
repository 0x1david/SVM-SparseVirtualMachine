#include "chunk.h"
#include "debug.h"

int main(int argc, const char *argv[]) {
  Chunk ch;
  initChunk(&ch);

  int constant = addConst(&ch, 1.2);
  writeChunk(&ch, OP_CONSTANT);
  writeChunk(&ch, constant);

  writeChunk(&ch, OP_RETURN);
  disassembleChunk(&ch, "test chunk");
  freeChunk(&ch);
  return 0;
}
