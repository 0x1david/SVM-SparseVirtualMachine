#include "object.h"
#include "value.h"
#include <string.h>
#define ALLOCATE_OBJ(type, objectType)                                         \
  (type *)allocateObject(sizeof(type), objectType)

static Obj *allocateObject(size_t size, ObjType type) {
  Obj *object = (Obj *)reallocate(NULL, 0, size);
  object->type = type;
  return object;
}

static ObjString *allocateString(char *chars, int length) {
  ObjString *string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
  string->length = length;
  string->chars = chars;
  return string;
}

ObjString *copyString(const char *chars, int length) {
  char *heapChars = ALLOCATE(char, length + 1);
  memcpy(heapChars, chars, length);
  heapChars[length] = '\0';
  return allocateString(heapChars, length);
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
      ObjString *aString = AS_STRING(a);
      ObjString *bString = AS_STRING(b);
      return aString->length == bString->length &&
             memcmp(aString->chars, bString->chars, aString->length) == 0;
    }
    default: return false;
  }
}

ObjString *takeString(char *chars, int length) {
  return allocateString(chars, length);
}
