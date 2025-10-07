#ifndef svm_memory_h
#define svm_memory_h
#include "common.h"
#include "token.h"
#include <stdbool.h>

#define ALLOCATE(type, count)                                                  \
  (type *)reallocate(NULL, 0, sizeof(type) * (count))

#define GROW_CAPACITY(c) ((c) == 0 ? 8 : ((c) * 2))
#define GROW_ARRAY(type, ptr, oldSize, newSize)                                \
  ptr = (type *)reallocate(ptr, sizeof(type) * oldSize, sizeof(type) * newSize)

#define FREE_ARRAY(type, ptr, oldSize)                                         \
  reallocate(ptr, sizeof(type) * oldSize, 0)

#define FREE(type, ptr) reallocate(ptr, sizeof(type), 0)

#define DECLARE_CONTAINER_FUNCTIONS(type, container_name)                      \
  void init##container_name(container_name *container);                        \
  void write##container_name(container_name *container, type value);           \
  void free##container_name(container_name *container);

// Macro to implement the container functions for an existing struct
#define IMPLEMENT_CONTAINER_FUNCTIONS(type, container_name)                    \
  void init##container_name(container_name *container) {                       \
    container->capacity = 0;                                                   \
    container->length = 0;                                                     \
    container->values = NULL;                                                  \
  }                                                                            \
                                                                               \
  void write##container_name(container_name *container, type value) {          \
    if (container->capacity == container->length) {                            \
      int new_capacity = GROW_CAPACITY(container->capacity);                   \
      GROW_ARRAY(type, container->values, container->capacity, new_capacity);  \
      container->capacity = new_capacity;                                      \
    }                                                                          \
    container->values[container->length] = value;                              \
    container->length++;                                                       \
  }                                                                            \
                                                                               \
  void free##container_name(container_name *container) {                       \
    FREE_ARRAY(type, container->values, container->capacity);                  \
    init##container_name(container);                                           \
  }

void *realloc(void *ptr, size_t size);
void *reallocate(void *ptr, size_t oldSize, size_t newSize);

typedef struct Trie {
  struct Trie *children[26];
  bool is_leaf_node;
  TokType token_type;
} Trie;

Trie *trieNew();
void trieFree(Trie *t);
void trieInsert(Trie *t, const char *s, TokType tt);
TokType trieFind(Trie *t, const char *s, int length);
void freeObjects();

#endif
