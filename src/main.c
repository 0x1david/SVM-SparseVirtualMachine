#include "chunk.h"
#include "common.h"
#include "debug.h"

int main(int argc, const char *argv[]) {
  Chunk ch;
  initChunk(&ch);
  writeChunk(&ch, OP_RETURN);
  disassembleChunk(&ch, "test chunk");
  freeChunk(&ch);
  return 0;
}
