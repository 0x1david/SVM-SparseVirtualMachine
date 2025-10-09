#ifndef svm_vm_h
#define svm_vm_h

#include "chunk.h"
#include "map.h"
#include "stack.h"

#define BINARY_OP(vm, valueType, op)                                           \
  do {                                                                         \
    if (!IS_NUMBER(peek(vm, 0)) || !IS_NUMBER(peek(vm, 1))) {                  \
      runtimeError(vm, "Operands must be numbers.");                           \
      return INTERPRET_RUNTIME_ERROR;                                          \
    }                                                                          \
                                                                               \
    double b = AS_NUMBER(stackPop(vm->stack));                                 \
    double a = AS_NUMBER(stackPop(vm->stack));                                 \
    stackPush(vm->stack, valueType(a op b));                                   \
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
  hashMap strings;
  Obj *objects;
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
