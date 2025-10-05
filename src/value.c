#include "value.h"
#include "memory.h"
IMPLEMENT_CONTAINER_FUNCTIONS(Value, ValueArray);

void printValue(Value value) { printf("%g", AS_NUMBER(value)); }
