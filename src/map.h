#ifndef svm_map_h
#define svm_map_h
#include "value.h"

typedef struct {
  ObjString *key;
  Value value;
} mapObject;

typedef struct {
  int length;
  int capacity;
  mapObject *contents;
} hashMap;

uint32_t hashString(char *s, int length);
void mapInit(hashMap *m);
void mapFree(hashMap *m);
void mapInsert(hashMap *m, ObjString *key, Value value);
Value mapGet(hashMap *m, ObjString *key);
void mapDelete(hashMap *m, ObjString *key);

#endif
