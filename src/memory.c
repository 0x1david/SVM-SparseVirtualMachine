#include "memory.h"
#include <ctype.h>
#include <stdlib.h>

void *reallocate(void *ptr, size_t oldSize, size_t newSize) {
  if (newSize == 0) {
    free(ptr);
    return NULL;
  }
  void *result = realloc(ptr, newSize);
  if (result == NULL) { exit(1); }
  return result;
}

Trie *trieNew() { return calloc(1, sizeof(Trie)); }
void trieFree(Trie *t) {
  if (t == NULL) { return; }

  for (int n = 0; n < 26; n++) {
    trieFree(t->children[n]);
  }

  free(t);
}
void trieInsert(Trie *t, const char *s, TokType tt) {
  if (*s == '\0') {
    t->is_leaf_node = true;
    t->token_type = tt;
    return;
  }

  int idx = *s - 'a';

  if (t->children[idx] == NULL) { t->children[idx] = trieNew(); }
  trieInsert(t->children[idx], s + 1, tt);
}

TokType trieFind(Trie *t, const char *s, int length) {
  if (length == 0) { return t->is_leaf_node ? t->token_type : TOK_IDENTIFIER; }
  if (!islower(*s)) { return TOK_IDENTIFIER; }
  int idx = *s - 'a';
  if (t->children[idx] == NULL) { return TOK_IDENTIFIER; }
  return trieFind(t->children[idx], s + 1, length - 1);
}
