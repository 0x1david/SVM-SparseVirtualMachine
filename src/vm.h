#ifndef svm_vm_h
#define svm_vm_h

#include "chunk.h"
#include "stack.h"

#define BINARY_OP(vm, op)                                                      \
  do {                                                                         \
    Value b = stackPop(vm->stack);                                             \
    Value a = stackPop(vm->stack);                                             \
    stackPush(vm->stack, a op b);                                              \
  } while (false)

#define BINARY_FUNC(vm, func)                                                  \
  do {                                                                         \
    Value b = stackPop(vm->stack);                                             \
    Value a = stackPop(vm->stack);                                             \
    stackPush(vm->stack, func(a, b));                                          \
  } while (false)

typedef struct VM {
  Chunk *chunk;
  Stack *stack;
  uint8_t *ip;
} VM;

typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpretResult;

void initVM(VM *vm);
void closeVM(VM *vm);
InterpretResult interpret(VM *vm, const char *src);

#endif
