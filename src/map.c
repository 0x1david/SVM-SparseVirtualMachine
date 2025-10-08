#include "map.h"
#include <stdlib.h>

void mapInit(hashMap *m) {
  m->length = 0;
  m->capacity = 0;
  m->contents = NULL;
}
void mapReset(hashMap *m) {
  free(m->contents);
  mapInit(m);
}
static int mapContains(hashMap *m, uint32_t *key) {};
void mapInsert(hashMap *m, ObjString *key, Value value) {}
static void mapReallocate(hashMap *m) {}
Value mapGet(hashMap *m, ObjString *key);
void mapDelete(hashMap *m, ObjString *key);
