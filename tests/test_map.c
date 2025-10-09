#include "../src/map.h"
#include "../src/object.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *duplicateString(const char *str) {
  size_t len = strlen(str) + 1;
  char *dup = malloc(len);
  if (dup) { memcpy(dup, str, len); }
  return dup;
}

// Test utilities
static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) static void name()
#define RUN_TEST(test)                                                         \
  do {                                                                         \
    printf("Running %s...", #test);                                            \
    test();                                                                    \
    tests_run++;                                                               \
    tests_passed++;                                                            \
    printf(" PASSED\n");                                                       \
  } while (0)

// Helper to create test strings (assumes you have newString or similar)
// Adjust based on your actual ObjString creation API
static ObjString *makeTestString(const char *str) {
  // Replace with your actual string creation function
  ObjString *obj = malloc(sizeof(ObjString));
  obj->length = strlen(str);
  obj->chars = duplicateString(str);
  return obj;
}

static void freeTestString(ObjString *str) {
  free(str->chars);
  free(str);
}

// ============================================================================
// Basic Functionality Tests
// ============================================================================

TEST(test_map_init) {
  hashMap map;
  mapInit(&map);

  assert(map.length == 0);
  assert(map.capacity == 0);
  assert(map.contents == NULL);

  mapReset(&map);
}

TEST(test_insert_single_element) {
  hashMap map;
  mapInit(&map);

  ObjString *key = makeTestString("test_key");
  Value val = {.type = VAL_NUMBER, .as.number = 42.0};

  mapInsert(&map, key, val);
  assert(map.length == 1);

  mapReset(&map);
  freeTestString(key);
}

TEST(test_get_existing_key) {
  hashMap map;
  mapInit(&map);

  ObjString *key = makeTestString("test_key");
  Value insertVal = {.type = VAL_NUMBER, .as.number = 42.0};
  Value retrievedVal;

  mapInsert(&map, key, insertVal);
  bool found = mapGet(&map, key, &retrievedVal);

  assert(found == true);
  assert(retrievedVal.type == VAL_NUMBER);
  assert(retrievedVal.as.number == 42.0);

  mapReset(&map);
  freeTestString(key);
}

TEST(test_get_nonexistent_key) {
  hashMap map;
  mapInit(&map);

  ObjString *key = makeTestString("nonexistent");
  Value retrievedVal;

  bool found = mapGet(&map, key, &retrievedVal);
  assert(found == false);

  mapReset(&map);
  freeTestString(key);
}

TEST(test_update_existing_key) {
  hashMap map;
  mapInit(&map);

  ObjString *key = makeTestString("test_key");
  Value val1 = {.type = VAL_NUMBER, .as.number = 42.0};
  Value val2 = {.type = VAL_NUMBER, .as.number = 99.0};
  Value retrieved;

  mapInsert(&map, key, val1);
  mapInsert(&map, key, val2); // Update

  assert(map.length == 1); // Should still be 1
  mapGet(&map, key, &retrieved);
  assert(retrieved.as.number == 99.0);

  mapReset(&map);
  freeTestString(key);
}

TEST(test_delete_existing_key) {
  hashMap map;
  mapInit(&map);

  ObjString *key = makeTestString("test_key");
  Value val = {.type = VAL_NUMBER, .as.number = 42.0};
  Value retrieved;

  mapInsert(&map, key, val);
  mapDelete(&map, key);

  assert(mapGet(&map, key, &retrieved) == false);

  mapReset(&map);
  freeTestString(key);
}

TEST(test_delete_nonexistent_key) {
  hashMap map;
  mapInit(&map);

  ObjString *key = makeTestString("nonexistent");
  int lengthBefore = map.length;

  mapDelete(&map, key); // Should not crash
  assert(map.length == lengthBefore);

  mapReset(&map);
  freeTestString(key);
}

// ============================================================================
// Multiple Elements Tests
// ============================================================================

TEST(test_multiple_insertions) {
  hashMap map;
  mapInit(&map);

  ObjString *keys[5];
  Value vals[5];

  for (int i = 0; i < 5; i++) {
    char keyStr[20];
    sprintf(keyStr, "key_%d", i);
    keys[i] = makeTestString(keyStr);
    vals[i] = (Value){.type = VAL_NUMBER, .as.number = (double)i};
    mapInsert(&map, keys[i], vals[i]);
  }

  assert(map.length == 5);

  // Verify all can be retrieved
  for (int i = 0; i < 5; i++) {
    Value retrieved;
    assert(mapGet(&map, keys[i], &retrieved) == true);
    assert(retrieved.as.number == (double)i);
  }

  mapReset(&map);
  for (int i = 0; i < 5; i++) {
    freeTestString(keys[i]);
  }
}

TEST(test_interleaved_operations) {
  hashMap map;
  mapInit(&map);

  ObjString *k1 = makeTestString("key1");
  ObjString *k2 = makeTestString("key2");
  ObjString *k3 = makeTestString("key3");
  Value v1 = {.type = VAL_NUMBER, .as.number = 1.0};
  Value v2 = {.type = VAL_NUMBER, .as.number = 2.0};
  Value v3 = {.type = VAL_NUMBER, .as.number = 3.0};
  Value retrieved;

  mapInsert(&map, k1, v1);
  mapInsert(&map, k2, v2);
  mapDelete(&map, k1);
  mapInsert(&map, k3, v3);

  assert(map.length == 2);
  assert(mapGet(&map, k1, &retrieved) == false);
  assert(mapGet(&map, k2, &retrieved) == true);
  assert(mapGet(&map, k3, &retrieved) == true);

  mapReset(&map);
  freeTestString(k1);
  freeTestString(k2);
  freeTestString(k3);
}

// ============================================================================
// Collision & Resize Tests
// ============================================================================

TEST(test_many_insertions_trigger_resize) {
  hashMap map;
  mapInit(&map);

  int initialCapacity = 256;
  int numElements = initialCapacity * 2; // Force resize

  ObjString **keys = malloc(sizeof(ObjString *) * numElements);

  for (int i = 0; i < numElements; i++) {
    char keyStr[32];
    sprintf(keyStr, "key_%d", i);
    keys[i] = makeTestString(keyStr);
    Value val = {.type = VAL_NUMBER, .as.number = (double)i};
    mapInsert(&map, keys[i], val);
  }

  assert(map.length == numElements);
  assert(map.capacity > initialCapacity);

  // Verify all elements still retrievable after resize
  for (int i = 0; i < numElements; i++) {
    Value retrieved;
    assert(mapGet(&map, keys[i], &retrieved) == true);
    assert(retrieved.as.number == (double)i);
  }

  mapReset(&map);
  for (int i = 0; i < numElements; i++) {
    freeTestString(keys[i]);
  }
  free(keys);
}

// ============================================================================
// Edge Cases
// ============================================================================

TEST(test_empty_string_key) {
  hashMap map;
  mapInit(&map);

  ObjString *key = makeTestString("");
  Value val = {.type = VAL_NUMBER, .as.number = 42.0};
  Value retrieved;

  mapInsert(&map, key, val);
  assert(mapGet(&map, key, &retrieved) == true);
  assert(retrieved.as.number == 42.0);

  mapReset(&map);
  freeTestString(key);
}

TEST(test_long_string_key) {
  hashMap map;
  mapInit(&map);

  char longKey[1000];
  memset(longKey, 'a', 999);
  longKey[999] = '\0';

  ObjString *key = makeTestString(longKey);
  Value val = {.type = VAL_NUMBER, .as.number = 42.0};
  Value retrieved;

  mapInsert(&map, key, val);
  assert(mapGet(&map, key, &retrieved) == true);

  mapReset(&map);
  freeTestString(key);
}

TEST(test_similar_keys) {
  hashMap map;
  mapInit(&map);

  ObjString *k1 = makeTestString("test");
  ObjString *k2 = makeTestString("test1");
  ObjString *k3 = makeTestString("1test");
  Value v1 = {.type = VAL_NUMBER, .as.number = 1.0};
  Value v2 = {.type = VAL_NUMBER, .as.number = 2.0};
  Value v3 = {.type = VAL_NUMBER, .as.number = 3.0};
  Value retrieved;

  mapInsert(&map, k1, v1);
  mapInsert(&map, k2, v2);
  mapInsert(&map, k3, v3);

  assert(mapGet(&map, k1, &retrieved) == true && retrieved.as.number == 1.0);
  assert(mapGet(&map, k2, &retrieved) == true && retrieved.as.number == 2.0);
  assert(mapGet(&map, k3, &retrieved) == true && retrieved.as.number == 3.0);

  mapReset(&map);
  freeTestString(k1);
  freeTestString(k2);
  freeTestString(k3);
}

// ============================================================================
// Hash Function Tests
// ============================================================================

TEST(test_hash_string_consistency) {
  char *str = "test_string";
  int len = strlen(str);

  uint32_t hash1 = hashString(str, len);
  uint32_t hash2 = hashString(str, len);

  assert(hash1 == hash2); // Same input = same hash
}

TEST(test_hash_string_different_strings) {
  char *str1 = "test1";
  char *str2 = "test2";

  uint32_t hash1 = hashString(str1, strlen(str1));
  uint32_t hash2 = hashString(str2, strlen(str2));

  // Different strings should (usually) have different hashes
  // Note: This could theoretically fail due to collision, but unlikely
  assert(hash1 != hash2);
}

// ============================================================================
// Test Runner
// ============================================================================

int main(void) {
  printf("Running HashMap Tests\n");
  printf("=====================\n\n");

  RUN_TEST(test_map_init);
  RUN_TEST(test_insert_single_element);
  RUN_TEST(test_get_existing_key);
  RUN_TEST(test_get_nonexistent_key);
  RUN_TEST(test_update_existing_key);
  RUN_TEST(test_delete_existing_key);
  RUN_TEST(test_delete_nonexistent_key);
  RUN_TEST(test_multiple_insertions);
  RUN_TEST(test_interleaved_operations);
  RUN_TEST(test_many_insertions_trigger_resize);
  RUN_TEST(test_empty_string_key);
  RUN_TEST(test_long_string_key);
  RUN_TEST(test_similar_keys);
  RUN_TEST(test_hash_string_consistency);
  RUN_TEST(test_hash_string_different_strings);

  printf("\n=====================\n");
  printf("Tests: %d/%d passed\n", tests_passed, tests_run);

  return tests_passed == tests_run ? 0 : 1;
}
