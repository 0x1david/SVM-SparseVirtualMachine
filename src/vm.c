#include "vm.h"
#include "chunk.h"
#include "compiler.h"
#include "stack.h"
#include <stdlib.h>

void initVM(VM *vm) {
  vm->stack = malloc(sizeof(Stack));
  stackInit(vm->stack);
}
void closeVM(VM *vm) {}

InterpretResult interpret(VM *vm, const char *src) {
  Chunk chunk;
  initChunk(&chunk);

  if (!compile(src, &chunk)) {
    freeChunk(&chunk);
    return INTERPRET_COMPILE_ERROR;
  }

  vm->chunk = &chunk;
  vm->ip = vm->chunk->code;
  InterpretResult result = run();

  freeChunk(&chunk);
  return result;
}
