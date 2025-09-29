#include "lexer.h"

void compile(const char *src) {
  Lexer lexer;
  initLexer(&lexer, src);

  for (;;) {
    Tok tok = lexTok(&lexer);
    printf("%2d '%.*s'\n", tok.type, tok.length, tok.start);
  }
}
