#include "vm.h"
#include "common.h"
#include "debug.h"
#include "value.h"

void initVM(VM *vm) {}
void freeVM(VM *vm) {}
InterpretResult interpret(VM *vm) {
#define READ_BYTE() (*vm->ip++)
#define READ_CONSTANT() (vm->chunk->constants.values[READ_BYTE()])
#define READ_CONSTANT_LONG()                                                   \
  ({                                                                           \
    uint8_t low = READ_BYTE();                                                 \
    uint8_t high = READ_BYTE();                                                \
    vm->chunk->constants.values[low | (high << 8)];                            \
  })

  for (;;) {
#ifdef DEBUG_VM
    disassembleInstruction(vm->chunk, (int)(vm->ip - vm->chunk->code));
#endif
    uint8_t instruction;
    switch (instruction = READ_BYTE()) {
    case OP_RETURN:
      return INTERPRET_OK;
    case OP_CONSTANT: {
      Value constant = READ_CONSTANT();
      printValue(constant);
      printf("\n");
      break;
    }
    case OP_CONSTANT_LONG: {
      Value constant = READ_CONSTANT_LONG();
      printValue(constant);
      printf("\n");
      break;
    }
    }
  }
#undef READ_BYTE
#undef READ_CONSTANT
#undef READ_CONSTANT_LONG
}
