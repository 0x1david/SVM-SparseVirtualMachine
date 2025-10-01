#include "lexer.h"
#include "memory.h"

void compile(const char *src) {
  int line = -1;
  Lexer lexer;
  initLexer(&lexer, src);

  for (;;) {
    Tok tok = lexTok(&lexer);
    if (tok.line != line) {
      printf("%4d ", tok.line);
      line = tok.line;
    } else {
      printf("  | ");
    }
    printf("%2d '%.*s'\n", tok.type, tok.length, tok.start);

    if (tok.type == TOK_EOF) break;
  }
}
