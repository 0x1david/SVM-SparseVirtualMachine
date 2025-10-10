#include "map.h"
#include "memory.h"
#include "object.h"
#include "value.h"
#include <stdlib.h>
#include <string.h>

#define IS_TOMBSTONE(slot)                                                     \
  ((slot).key == NULL && IS_BOOL((slot).value) && AS_BOOL((slot).value))
#define EXPAND_FACTOR 0.7

uint32_t hashString(const char *s, int length) {
  uint32_t hash = 2166136261u;
  for (int i = 0; i < length; i++) {
    hash ^= (uint8_t)s[i];
    hash *= 16777619;
  }
  return hash;
}

ObjString *mapFindString(hashMap *map, const char *chars, int length,
                         uint32_t hash) {
  if (map->length == 0) return NULL;

  uint32_t index = hash % map->capacity;
  for (;;) {
    mapObject *item = &map->contents[index];
    if (item->key == NULL) {
      if (IS_NIL(item->value)) return NULL;
    } else if (item->key->length == length && item->key->hash == hash &&
               memcmp(item->key->chars, chars, length) == 0) {
      return item->key;
    }
    index = ((index + 1) % map->capacity);
  }
}

void mapInit(hashMap *m) {
  m->length = 0;
  m->capacity = 0;
  m->contents = NULL;
}
void mapReset(hashMap *m) {
  free(m->contents);
  mapInit(m);
}

static void mapReallocate(hashMap *m) {
  hashMap new_map;
  mapInit(&new_map);

  int old_cap = m->capacity;

  new_map.capacity = GROW_CAPACITY(m->capacity);
  new_map.contents = ALLOCATE(mapObject, new_map.capacity);

  for (int i = 0; i < new_map.capacity; i++) {
    new_map.contents[i].key = NULL;
    new_map.contents[i].value = NIL_VAL();
  }

  for (int n = 0; n < old_cap; n++) {
    mapObject item = m->contents[n];
    if (IS_TOMBSTONE(item) || IS_NIL(item.value)) { continue; }

    mapInsert(&new_map, item.key, item.value);
  }
  mapReset(m);
  *m = new_map;
}

static mapObject *findEntry(hashMap *m, ObjString *key) {
  int idx = key->hash % m->capacity;
  mapObject *tombstone = NULL;

  for (;;) {
    mapObject *item = &m->contents[idx];

    if (item->key == NULL) {
      if (IS_NIL(item->value)) {
        return tombstone != NULL ? tombstone : item;
      } else {
        if (tombstone == NULL) { tombstone = item; }
      }
    } else if (item->key == key) {
      return item;
    }

    idx = (idx + 1) % m->capacity;
  }
};

bool mapInsert(hashMap *m, ObjString *key, Value value) {
  if (m->length + 1 > (EXPAND_FACTOR * m->capacity)) { mapReallocate(m); }

  mapObject *item = findEntry(m, key);
  bool is_new_key = (item->key == NULL && !IS_TOMBSTONE(*item));

  item->key = key;
  item->value = value;

  if (is_new_key) { m->length += 1; }
  return is_new_key;
}

bool mapGet(hashMap *m, ObjString *key, Value *value) {
  if (m->length == 0) return false;

  mapObject *entry = findEntry(m, key);
  if (entry->key == NULL) { return false; }

  *value = entry->value;
  return true;
}

void mapDelete(hashMap *m, ObjString *key) {
  if (m->capacity == 0) return;

  mapObject *item = findEntry(m, key);
  if (item->key == NULL) return;

  item->key = NULL;
  item->value = BOOL_VAL(true);
}
