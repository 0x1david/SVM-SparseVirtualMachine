#ifndef svm_value_h
#define svm_value_h
#include "memory.h"

typedef double Value;

typedef struct {
  int length;
  int capacity;
  Value *values;
} ValueArray;

DECLARE_ARRAY_FUNCTIONS(Value, Value);

void printValue(Value value);

#endif
