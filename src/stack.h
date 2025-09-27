#ifndef svm_stack_h
#define svm_stack_h

#include "memory.h"
#include "value.h"
#include <stdbool.h>

typedef struct {
  Value *data;
  int top;
  int capacity;
} Stack;

void stackInit(Stack *s);
void stackFree(Stack *s);
void stackPush(Stack *s, Value v);
Value stackPop(Stack *s);

#endif
