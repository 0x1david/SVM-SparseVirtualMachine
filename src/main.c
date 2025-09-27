#include "chunk.h"
#include "debug.h"
#include "vm.h"

int main(int argc, const char *argv[]) {
  VM vm;
  initVM(&vm);

  Chunk ch;
  initChunk(&ch);

  writeConst(&ch, 1.2, 123);

  writeChunk(&ch, OP_RETURN, 123, 3);
  disassembleChunk(&ch, "test chunk");
  interpret(&vm, &ch);

  closeVM(&vm);
  freeChunk(&ch);
  return 0;
}
