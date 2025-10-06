#include "value.h"
#include "memory.h"
IMPLEMENT_CONTAINER_FUNCTIONS(Value, ValueArray);

void printValue(Value value) {
  switch (value.type) {
    case VAL_BOOL: printf(AS_BOOL(value) ? "true" : false); break;
    case VAL_NIL: printf("nil"); break;
    case VAL_NUMBER: printf("%g", AS_NUMBER(value)); break;
  }
}
