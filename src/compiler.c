#include "lexer.h"

void compile(const char *src) {
  initLexer(src);

  for (;;) {
    Tok tok = lexTok();
    if (tok.line != line) {
      printf("%4d ", tok.line);
      line = tok.line;
    } else {
      printf("  | ");
    }
    printf("%2d '%.*s'\n", tok.type, tok.length, tok.start);
  }
}
