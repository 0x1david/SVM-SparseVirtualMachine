#include "object.h"
#include "map.h"
#include "value.h"
#include "vm.h"
#include <string.h>
#define ALLOCATE_OBJ(vm, type, objectType)                                     \
  (type *)allocateObject(vm, sizeof(type), objectType)

static Obj *allocateObject(VM *vm, size_t size, ObjType type) {
  Obj *object = (Obj *)reallocate(NULL, 0, size);
  object->type = type;

  object->next = vm->objects;
  return object;
}

static ObjString *allocateString(VM *vm, char *chars, int length) {
  ObjString *string = ALLOCATE_OBJ(vm, ObjString, OBJ_STRING);
  string->length = length;
  string->chars = chars;
  string->hash = hashString(chars, length);
  mapInsert(&vm->strings, string, NIL_VAL());
  return string;
}

ObjString *copyString(VM *vm, const char *chars, int length) {
  uint32_t hash = hashString(chars, length);
  ObjString *interned = mapFindString(&vm->strings, chars, length, hash);
  if (interned != NULL) return interned;

  char *heapChars = ALLOCATE(char, length + 1);
  memcpy(heapChars, chars, length);
  heapChars[length] = '\0';
  return allocateString(vm, heapChars, length);
}

void printObject(Value value) {
  switch (OBJ_TYPE(value)) {
    case OBJ_STRING: printf("%s", AS_CSTRING(value)); break;
  }
}

bool valuesEqual(Value a, Value b) {
  if (a.type != b.type) { return false; }
  switch (a.type) {
    case VAL_BOOL: return AS_BOOL(a) == AS_BOOL(b);
    case VAL_NUMBER: return AS_NUMBER(a) == AS_NUMBER(b);
    case VAL_NIL: return true;
    case VAL_OBJ: {
      return AS_OBJ(a) == AS_OBJ(b);
    }
    default: return false;
  }
}

ObjString *takeString(VM *vm, char *chars, int length) {
  uint32_t hash = hashString(chars, length);
  ObjString *interned = mapFindString(&vm->strings, chars, length, hash);

  if (interned != NULL) {
    FREE_ARRAY(char, chars, length + 1);
    return interned;
  }

  return allocateString(vm, chars, length);
}
