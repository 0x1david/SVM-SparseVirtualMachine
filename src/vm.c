#include "vm.h"
#include "common.h"
#include "debug.h"
#include "stack.h"
#include "value.h"
#include <stdlib.h>

void initVM(VM *vm) {
  vm->stack = malloc(sizeof(Stack));
  stackInit(vm->stack);
}
void closeVM(VM *vm) {}

InterpretResult interpret(VM *vm, Chunk *chunk) {
  vm->chunk = chunk;
  vm->ip = chunk->code;

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
    for (int i = 0; i <= vm->stack->top; i++) {
      printf("[ ");
      printValue(vm->stack->data[i]);
      printf(" ]");
    }
    printf("\n");
#endif

    uint8_t instruction;
    switch (instruction = READ_BYTE()) {
    case OP_RETURN:
      printValue(stackPop(vm->stack));
      printf("\n");
      return INTERPRET_OK;
    case OP_CONSTANT: {
      Value constant = READ_CONSTANT();
      stackPush(vm->stack, constant);
      break;
    }
    case OP_CONSTANT_LONG: {
      Value constant = READ_CONSTANT_LONG();
      stackPush(vm->stack, constant);
      break;
    }
    }
  }
#undef READ_BYTE
#undef READ_CONSTANT
#undef READ_CONSTANT_LONG
}
