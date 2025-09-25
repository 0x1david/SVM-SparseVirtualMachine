#include "chunk.h"
#include "debug.h"

int main(int argc, const char *argv[]) {
  Chunk ch;
  initChunk(&ch);

  writeConst(&ch, 1.2, 123);

  writeChunk(&ch, OP_RETURN, 123, 3);
  disassembleChunk(&ch, "test chunk");
  freeChunk(&ch);
  return 0;
}
