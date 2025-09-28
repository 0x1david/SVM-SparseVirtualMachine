#ifndef svm_lexer_h
#define svm_lexer_h

typedef struct {
  const char *start;
  const char *current;
  int line;
} Lexer;

void initLexer(Lexer *l, const char *src);

typedef enum {
  // Single-char
  TOK_LEFT_PAREN,
  TOK_RIGHT_PAREN,
  TOK_LEFT_BRACE,
  TOK_RIGHT_BRACE,
  TOK_COMMA,
  TOK_DOT,
  TOK_MINUS,
  TOK_PLUS,
  TOK_SEMICOLON,
  TOK_SLASH,
  TOK_STAR,
  // One or two chars
  TOK_BANG,
  TOK_BANG_EQUAL,
  TOK_EQUAL,
  TOK_EQUAL_EQUAL,
  TOK_GREATER,
  TOK_GREATER_EQUAL,
  TOK_LESS,
  TOK_LESS_EQUAL,
  // Literals
  TOK_IDENTIFIER,
  TOK_STRING,
  TOK_NUMBER,
  // Keywords
  TOK_AND,
  TOK_CLASS,
  TOK_ELSE,
  TOK_FALSE,
  TOK_FOR,
  TOK_FUN,
  TOK_IF,
  TOK_NIL,
  TOK_OR,
  TOK_PRINT,
  TOK_RETURN,
  TOK_SUPER,
  TOK_THIS,
  TOK_TRUE,
  TOK_VAR,
  TOK_WHILE,
  // Special
  TOK_ERROR,
  TOK_EOF
} TokType;

typedef struct {
  TokType type;
  const char *start;
  int length;
  int line;
} Tok;

Tok lexTok(Lexer *l);

#endif
