#ifndef svm_vm_h
#define svm_vm_h

#include "chunk.h"
#include "stack.h"

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
InterpretResult interpret(VM *vm, Chunk *chunk);

#endif
