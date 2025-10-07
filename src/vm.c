#include "vm.h"
#include "chunk.h"
#include "common.h"
#include "compiler.h"
#include "debug.h"
#include "object.h"
#include "stack.h"
#include "value.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

void initVM(VM *vm) {
  vm->stack = malloc(sizeof(Stack));
  stackInit(vm->stack);
}
void closeVM(VM *vm) {}

static Value peek(VM *vm, int distance) {
  return vm->stack->data[vm->stack->top - distance];
}

static bool isFalsey(Value value) {
  return IS_NIL(value) || (IS_BOOL(value) && !AS_BOOL(value));
}

static void concatenate(VM *vm) {
  ObjString *b = AS_STRING(stackPop(vm->stack));
  ObjString *a = AS_STRING(stackPop(vm->stack));
  int length = a->length + b->length;
  char *chars = ALLOCATE(char, length + 1);
  memcpy(chars, a->chars, a->length);
  memcpy(chars + a->length, b->chars, b->length);
  chars[length] = '\0';
  ObjString *result = takeString(chars, length);
  stackPush(vm->stack, OBJ_VAL(result));
}

static void runtimeError(VM *vm, const char *format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  fputs("\n", stderr);

  size_t instruction = vm->ip - vm->chunk->code - 1;
  int line = vm->chunk->lines.values[instruction];
  fprintf(stderr, "[line %d] in script\n", line);
  stackFree(vm->stack);
}

InterpretResult run(VM *vm) {
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
    disassembleInstruction(vm->chunk, (int)(vm->ip - vm->chunk->code));
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
      case OP_NEGATE:
        if (!IS_NUMBER(peek(vm, 0))) {
          runtimeError(vm, "Operand must be a number.");
          return INTERPRET_RUNTIME_ERROR;
        }
        stackPush(vm->stack, NUMBER_VAL(-AS_NUMBER(stackPop(vm->stack))));
        break;
      case OP_SUBTRACT: BINARY_OP(vm, NUMBER_VAL, -); break;
      case OP_MULTIPLY: BINARY_OP(vm, NUMBER_VAL, *); break;
      case OP_DIVIDE: BINARY_OP(vm, NUMBER_VAL, /); break;
      case OP_EQUAL: {
        Value b = stackPop(vm->stack);
        Value a = stackPop(vm->stack);
        stackPush(vm->stack, BOOL_VAL(valuesEqual(a, b)));
      }; break;
      case OP_GREATER: BINARY_OP(vm, NUMBER_VAL, >); break;
      case OP_LESS: BINARY_OP(vm, NUMBER_VAL, <); break;
      case OP_NOT:
        stackPush(vm->stack, BOOL_VAL(isFalsey(stackPop(vm->stack))));
        break;
      case OP_NIL: stackPush(vm->stack, NIL_VAL()); break;
      case OP_ADD: {
        if (IS_STRING(peek(vm, 0)) && IS_STRING(peek(vm, 1))) {
          concatenate(vm);
        } else if (IS_NUMBER(peek(vm, 0)) && IS_NUMBER(peek(vm, 1))) {
          double b = AS_NUMBER(stackPop(vm->stack));
          double a = AS_NUMBER(stackPop(vm->stack));
          stackPush(vm->stack, NUMBER_VAL(a + b));
        } else {
          runtimeError(vm, "Operands must be two numbers or two strings.");
          return INTERPRET_RUNTIME_ERROR;
        }
      }
    }
  }
}
#undef READ_BYTE
#undef READ_CONSTANT
#undef READ_CONSTANT_LONG

InterpretResult interpret(VM *vm, const char *src) {
  Chunk chunk;
  initChunk(&chunk);

  if (!compile(src, &chunk)) {
    freeChunk(&chunk);
    return INTERPRET_COMPILE_ERROR;
  }

  vm->chunk = &chunk;
  vm->ip = vm->chunk->code;
  InterpretResult result = run(vm);

  freeChunk(&chunk);
  return result;
}
