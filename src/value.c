#include "value.h"
#include "memory.h"
IMPLEMENT_ARRAY_FUNCTIONS(Value, Value);

void printValue(Value value) { printf("%g", value); }
