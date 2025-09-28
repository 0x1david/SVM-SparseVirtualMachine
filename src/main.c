#include "chunk.h"
#include "debug.h"
#include "vm.h"

int main(int argc, const char *argv[]) {
  VM vm;
  initVM(&vm);
  Chunk ch;
  initChunk(&ch);

  // Test: 1.2 + 3.4 - 2.0 (should equal 2.6)
  writeConst(&ch, 1.2, 123);
  writeConst(&ch, 3.4, 123);
  writeChunk(&ch, OP_ADD, 123, 3);

  writeConst(&ch, 2.0, 123);
  writeChunk(&ch, OP_SUBTRACT, 123, 3);

  // Test: negate the result (should equal -2.6)
  writeChunk(&ch, OP_NEGATE, 123, 3);

  // Test: multiply by 2.0 (should equal -5.2)
  writeConst(&ch, 2.0, 123);
  writeChunk(&ch, OP_MULTIPLY, 123, 3);

  // Test: divide by 4.0 (should equal -1.3)
  writeConst(&ch, 4.0, 123);
  writeChunk(&ch, OP_DIVIDE, 123, 3);

  writeChunk(&ch, OP_RETURN, 123, 3);

  disassembleChunk(&ch, "test chunk");
  interpret(&vm, &ch);
  closeVM(&vm);
  freeChunk(&ch);
  return 0;
}
