#ifndef svm_memory_h
#define svm_memory_h
#include "common.h"

#define GROW_CAPACITY(c) ((c) == 0 ? 8 : ((c) * 2))
#define GROW_ARRAY(type, ptr, oldSize, newSize)                                \
  (type *)reallocate(ptr, sizeof(type) * oldSize, sizeof(type) * newSize)

#define FREE_ARRAY(type, ptr, oldSize)                                         \
  reallocate(ptr, sizeof(type) * oldSize, 0)

void *reallocate(void *ptr, size_t oldSize, size_t newSize);

#endif
