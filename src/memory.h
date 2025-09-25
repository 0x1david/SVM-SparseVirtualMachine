#ifndef svm_memory_h
#define svm_memory_h
#include "common.h"

#define GROW_CAPACITY(c) ((c) == 0 ? 8 : ((c) * 2))
#define GROW_ARRAY(type, ptr, oldSize, newSize)                                \
  ptr = (type *)reallocate(ptr, sizeof(type) * oldSize, sizeof(type) * newSize)

#define FREE_ARRAY(type, ptr, oldSize)                                         \
  reallocate(ptr, sizeof(type) * oldSize, 0)

// Macro to declare the array function prototypes for an existing struct
#define DECLARE_ARRAY_FUNCTIONS(type, name)                                    \
  void init##name##Array(name##Array *array);                                  \
  void write##name##Array(name##Array *array, type value);                     \
  void free##name##Array(name##Array *array);

// Macro to implement the array functions for an existing struct
#define IMPLEMENT_ARRAY_FUNCTIONS(type, name)                                  \
  void init##name##Array(name##Array *array) {                                 \
    array->capacity = 0;                                                       \
    array->length = 0;                                                         \
    array->values = NULL;                                                      \
  }                                                                            \
                                                                               \
  void write##name##Array(name##Array *array, type value) {                    \
    if (array->capacity == array->length) {                                    \
      int new_capacity = GROW_CAPACITY(array->capacity);                       \
      GROW_ARRAY(type, array->values, array->capacity, new_capacity);          \
      array->capacity = new_capacity;                                          \
    }                                                                          \
    array->values[array->length] = value;                                      \
    array->length++;                                                           \
  }                                                                            \
                                                                               \
  void free##name##Array(name##Array *array) {                                 \
    FREE_ARRAY(type, array->values, array->capacity);                          \
    init##name##Array(array);                                                  \
  }

void *realloc(void *ptr, size_t size);
void *reallocate(void *ptr, size_t oldSize, size_t newSize);

#endif
