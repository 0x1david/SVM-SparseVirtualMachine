#include "stack.h"
#include "memory.h"
#include <stdlib.h>

#define INITIAL_STACK_SIZE 256

void stackInit(Stack *s) {
  s->data = NULL;
  s->top = -1;
  s->capacity = 0;
}

void stackFree(Stack *s) { free(s->data); }

void stackPush(Stack *s, Value v) {
  if (s->top + 1 >= s->capacity) {
    int new_capacity = s->capacity == 0 ? INITIAL_STACK_SIZE : s->capacity * 2;
    GROW_ARRAY(Value, s->data, s->capacity, new_capacity);
    s->capacity = new_capacity;
  }

  s->data[++s->top] = v;
}

Value stackPop(Stack *s) { return s->data[s->top--]; }
