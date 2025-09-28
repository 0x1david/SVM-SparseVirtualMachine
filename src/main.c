#include "chunk.h"
#include "debug.h"
#include "vm.h"
#include <stdio.h>
#include <stdlib.h>

static void repl(VM *vm) {
  char line[1024];
  for (;;) {
    printf("> ");
    if (!fgets(line, sizeof(line), stdin)) {
      printf("\n");
      break;
    }
    interpret(vm, line);
  }
}

static char *readFile(const char *path) {
  FILE *file = fopen(path, "rb");
  if (file == NULL) {
    fprintf(stderr, "Could not open file \"%s\".\n", path);
    exit(74);
  }

  fseek(file, 0L, SEEK_END);
  size_t fileSize = ftell(file);
  rewind(file);

  char *buf = (char *)malloc(fileSize + 1);
  if (buf == NULL) {
    fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
    exit(74);
  }

  size_t bytesRead = fread(buf, sizeof(char), fileSize, file);
  if (bytesRead < fileSize) {
    fprintf(stderr, "Could not read file \"%s\".\n", path);
    exit(74);
  }
  buf[bytesRead] = '\0';

  fclose(file);
  return buf;
}

static void runFile(VM *vm, const char *path) {
  char *src = readFile(path);
  InterpretResult res = interpret(vm, src);
  free(src);

  switch (res) {
    case INTERPRET_COMPILE_ERROR: exit(65); break;
    case INTERPRET_RUNTIME_ERROR: exit(70); break;
    case INTERPRET_OK: break;
  }
}

int main(int argc, const char *argv[]) {
  VM vm;
  initVM(&vm);
  switch (argc) {
    case 1: repl(&vm); break;
    case 2: runFile(&vm, argv[1]); break;
    default: fprintf(stderr, "Usage: svm [path]\n");
  }
  closeVM(&vm);
  return 0;
}
