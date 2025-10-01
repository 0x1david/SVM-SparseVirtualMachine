#include "../src/lexer.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

// Helper function to print token type name
const char *tokTypeName(TokType type) {
  switch (type) {
    case TOK_LEFT_PAREN: return "TOK_LEFT_PAREN";
    case TOK_RIGHT_PAREN: return "TOK_RIGHT_PAREN";
    case TOK_LEFT_BRACE: return "TOK_LEFT_BRACE";
    case TOK_RIGHT_BRACE: return "TOK_RIGHT_BRACE";
    case TOK_COMMA: return "TOK_COMMA";
    case TOK_DOT: return "TOK_DOT";
    case TOK_MINUS: return "TOK_MINUS";
    case TOK_PLUS: return "TOK_PLUS";
    case TOK_SEMICOLON: return "TOK_SEMICOLON";
    case TOK_SLASH: return "TOK_SLASH";
    case TOK_STAR: return "TOK_STAR";
    case TOK_BANG: return "TOK_BANG";
    case TOK_BANG_EQUAL: return "TOK_BANG_EQUAL";
    case TOK_EQUAL: return "TOK_EQUAL";
    case TOK_EQUAL_EQUAL: return "TOK_EQUAL_EQUAL";
    case TOK_GREATER: return "TOK_GREATER";
    case TOK_GREATER_EQUAL: return "TOK_GREATER_EQUAL";
    case TOK_LESS: return "TOK_LESS";
    case TOK_LESS_EQUAL: return "TOK_LESS_EQUAL";
    case TOK_IDENTIFIER: return "TOK_IDENTIFIER";
    case TOK_STRING: return "TOK_STRING";
    case TOK_NUMBER: return "TOK_NUMBER";
    case TOK_AND: return "TOK_AND";
    case TOK_CLASS: return "TOK_CLASS";
    case TOK_ELSE: return "TOK_ELSE";
    case TOK_FALSE: return "TOK_FALSE";
    case TOK_FOR: return "TOK_FOR";
    case TOK_FUN: return "TOK_FUN";
    case TOK_IF: return "TOK_IF";
    case TOK_NIL: return "TOK_NIL";
    case TOK_OR: return "TOK_OR";
    case TOK_PRINT: return "TOK_PRINT";
    case TOK_RETURN: return "TOK_RETURN";
    case TOK_SUPER: return "TOK_SUPER";
    case TOK_THIS: return "TOK_THIS";
    case TOK_TRUE: return "TOK_TRUE";
    case TOK_VAR: return "TOK_VAR";
    case TOK_WHILE: return "TOK_WHILE";
    case TOK_ERROR: return "TOK_ERROR";
    case TOK_EOF: return "TOK_EOF";
    default: return "UNKNOWN";
  }
}

// Helper function to compare tokens with detailed error messages
void assertToken(Tok tok, TokType expectedType, const char *expectedLexeme,
                 int expectedLength) {
  if (tok.type != expectedType) {
    fprintf(stderr, "  ✗ Token type mismatch!\n");
    fprintf(stderr, "    Expected: %s (%d)\n", tokTypeName(expectedType),
            expectedType);
    fprintf(stderr, "    Got:      %s (%d)\n", tokTypeName(tok.type), tok.type);
    fprintf(stderr, "    Lexeme:   '%.*s'\n", tok.length, tok.start);
    assert(0);
  }

  if (tok.length != expectedLength) {
    fprintf(stderr, "  ✗ Token length mismatch!\n");
    fprintf(stderr, "    Expected length: %d\n", expectedLength);
    fprintf(stderr, "    Got length:      %d\n", tok.length);
    fprintf(stderr, "    Expected lexeme: '%s'\n", expectedLexeme);
    fprintf(stderr, "    Got lexeme:      '%.*s'\n", tok.length, tok.start);
    assert(0);
  }

  if (strncmp(tok.start, expectedLexeme, expectedLength) != 0) {
    fprintf(stderr, "  ✗ Token lexeme mismatch!\n");
    fprintf(stderr, "    Expected: '%s'\n", expectedLexeme);
    fprintf(stderr, "    Got:      '%.*s'\n", tok.length, tok.start);
    assert(0);
  }
}

// Test 1: Keywords are recognized correctly
void test_keywords() {
  printf("Testing keywords...\n");

  const char *keywords[] = {"and",  "class", "else", "false", "for",    "fun",
                            "if",   "nil",   "or",   "print", "return", "super",
                            "this", "true",  "var",  "while"};

  TokType expectedTypes[] = {TOK_AND,  TOK_CLASS, TOK_ELSE,   TOK_FALSE,
                             TOK_FOR,  TOK_FUN,   TOK_IF,     TOK_NIL,
                             TOK_OR,   TOK_PRINT, TOK_RETURN, TOK_SUPER,
                             TOK_THIS, TOK_TRUE,  TOK_VAR,    TOK_WHILE};

  for (int i = 0; i < 16; i++) {
    Lexer lexer;
    initLexer(&lexer, keywords[i]);

    Tok tok = lexTok(&lexer);
    assertToken(tok, expectedTypes[i], keywords[i], strlen(keywords[i]));

    // Next token should be EOF
    Tok eof = lexTok(&lexer);
    if (eof.type != TOK_EOF) {
      fprintf(stderr, "  ✗ Expected EOF after '%s', got %s\n", keywords[i],
              tokTypeName(eof.type));
      assert(0);
    }

    freeLexer(&lexer);
    printf("  ✓ '%s' -> %s\n", keywords[i], tokTypeName(expectedTypes[i]));
  }
}

// Test 2: Unknown keywords become identifiers
void test_identifiers() {
  printf("\nTesting identifiers...\n");

  const char *identifiers[] = {"foo",      "bar", "myVariable", "test123",
                               "_private", "AND", "Class",      "whilee"};

  for (int i = 0; i < 8; i++) {
    Lexer lexer;
    initLexer(&lexer, identifiers[i]);

    Tok tok = lexTok(&lexer);
    assertToken(tok, TOK_IDENTIFIER, identifiers[i], strlen(identifiers[i]));

    freeLexer(&lexer);
    printf("  ✓ '%s' -> TOK_IDENTIFIER\n", identifiers[i]);
  }
}

// Test 3: Single character tokens
void test_single_char_tokens() {
  printf("\nTesting single character tokens...\n");

  struct {
    const char *input;
    TokType type;
  } tests[] = {
      {"/", TOK_SLASH},      {"(", TOK_LEFT_PAREN},  {")", TOK_RIGHT_PAREN},
      {"{", TOK_LEFT_BRACE}, {"}", TOK_RIGHT_BRACE}, {",", TOK_COMMA},
      {".", TOK_DOT},        {"-", TOK_MINUS},       {"+", TOK_PLUS},
      {";", TOK_SEMICOLON},  {"*", TOK_STAR},
  };

  for (int i = 0; i < 11; i++) {
    Lexer lexer;
    initLexer(&lexer, tests[i].input);

    Tok tok = lexTok(&lexer);
    assertToken(tok, tests[i].type, tests[i].input, 1);

    freeLexer(&lexer);
    printf("  ✓ '%s' -> %s\n", tests[i].input, tokTypeName(tests[i].type));
  }
}

// Test 4: Two character tokens
void test_two_char_tokens() {
  printf("\nTesting two character tokens...\n");

  struct {
    const char *input;
    TokType type;
  } tests[] = {
      {"!", TOK_BANG},    {"!=", TOK_BANG_EQUAL},
      {"=", TOK_EQUAL},   {"==", TOK_EQUAL_EQUAL},
      {"<", TOK_LESS},    {"<=", TOK_LESS_EQUAL},
      {">", TOK_GREATER}, {">=", TOK_GREATER_EQUAL},
  };

  for (int i = 0; i < 8; i++) {
    Lexer lexer;
    initLexer(&lexer, tests[i].input);

    Tok tok = lexTok(&lexer);
    assertToken(tok, tests[i].type, tests[i].input, strlen(tests[i].input));

    freeLexer(&lexer);
    printf("  ✓ '%s' -> %s\n", tests[i].input, tokTypeName(tests[i].type));
  }
}

// Test 5: String literals
void test_strings() {
  printf("\nTesting string literals...\n");

  const char *tests[] = {
      "\"hello\"",
      "\"\"",
      "\"hello world\"",
      "\"123\"",
  };

  for (int i = 0; i < 4; i++) {
    Lexer lexer;
    initLexer(&lexer, tests[i]);

    Tok tok = lexTok(&lexer);
    assertToken(tok, TOK_STRING, tests[i], strlen(tests[i]));

    freeLexer(&lexer);
    printf("  ✓ %s -> TOK_STRING\n", tests[i]);
  }

  // Test unterminated string
  Lexer lexer;
  initLexer(&lexer, "\"unterminated");
  Tok tok = lexTok(&lexer);
  if (tok.type != TOK_ERROR) {
    fprintf(stderr, "  ✗ Expected TOK_ERROR for unterminated string, got %s\n",
            tokTypeName(tok.type));
    assert(0);
  }
  freeLexer(&lexer);
  printf("  ✓ Unterminated string -> TOK_ERROR\n");
}

// Test 6: Number literals
void test_numbers() {
  printf("\nTesting number literals...\n");

  struct {
    const char *input;
    bool shouldSucceed;
  } tests[] = {
      {"123", true},       {"0", true},     {"3.14", true}, {"0.5", true},
      {"123.456", true},   {"123.", false}, // Invalid: trailing dot
      {"123..456", false},                  // Invalid: double dot
  };

  for (int i = 0; i < 7; i++) {
    Lexer lexer;
    initLexer(&lexer, tests[i].input);

    Tok tok = lexTok(&lexer);
    if (tests[i].shouldSucceed) {
      assertToken(tok, TOK_NUMBER, tests[i].input, strlen(tests[i].input));
      printf("  ✓ '%s' -> TOK_NUMBER\n", tests[i].input);
    } else {
      if (tok.type != TOK_ERROR) {
        fprintf(stderr, "  ✗ Expected TOK_ERROR for '%s', got %s\n",
                tests[i].input, tokTypeName(tok.type));
        assert(0);
      }
      printf("  ✓ '%s' -> TOK_ERROR (expected)\n", tests[i].input);
    }

    freeLexer(&lexer);
  }
}

// Test 7: Whitespace and comments
void test_whitespace_and_comments() {
  printf("\nTesting whitespace and comments...\n");

  Lexer lexer;
  initLexer(&lexer, "  \t\r  foo  \n  bar  // comment\nbaz");

  Tok tok1 = lexTok(&lexer);
  assertToken(tok1, TOK_IDENTIFIER, "foo", 3);

  Tok tok2 = lexTok(&lexer);
  assertToken(tok2, TOK_IDENTIFIER, "bar", 3);

  Tok tok3 = lexTok(&lexer);
  assertToken(tok3, TOK_IDENTIFIER, "baz", 3);

  Tok eof = lexTok(&lexer);
  if (eof.type != TOK_EOF) {
    fprintf(stderr, "  ✗ Expected TOK_EOF, got %s\n", tokTypeName(eof.type));
    assert(0);
  }

  freeLexer(&lexer);
  printf("  ✓ Whitespace and comments handled correctly\n");
}

// Test 8: Composite statement
void test_composite_statement() {
  printf("\nTesting composite statement...\n");

  const char *source =
      "var x = 10 + 20;\nif (x >= 30) {\n  print \"success\";\n}";

  struct {
    TokType type;
    const char *lexeme;
  } expected[] = {
      {TOK_VAR, "var"},
      {TOK_IDENTIFIER, "x"},
      {TOK_EQUAL, "="},
      {TOK_NUMBER, "10"},
      {TOK_PLUS, "+"},
      {TOK_NUMBER, "20"},
      {TOK_SEMICOLON, ";"},
      {TOK_IF, "if"},
      {TOK_LEFT_PAREN, "("},
      {TOK_IDENTIFIER, "x"},
      {TOK_GREATER_EQUAL, ">="},
      {TOK_NUMBER, "30"},
      {TOK_RIGHT_PAREN, ")"},
      {TOK_LEFT_BRACE, "{"},
      {TOK_PRINT, "print"},
      {TOK_STRING, "\"success\""},
      {TOK_SEMICOLON, ";"},
      {TOK_RIGHT_BRACE, "}"},
      {TOK_EOF, ""},
  };

  Lexer lexer;
  initLexer(&lexer, source);

  for (int i = 0; i < 19; i++) {
    Tok tok = lexTok(&lexer);

    if (tok.type != expected[i].type) {
      fprintf(stderr, "  ✗ Token %d type mismatch!\n", i);
      fprintf(stderr, "    Expected: %s\n", tokTypeName(expected[i].type));
      fprintf(stderr, "    Got:      %s\n", tokTypeName(tok.type));
      fprintf(stderr, "    Lexeme:   '%.*s'\n", tok.length, tok.start);
      assert(0);
    }

    if (expected[i].type != TOK_EOF) {
      if (strncmp(tok.start, expected[i].lexeme, strlen(expected[i].lexeme)) !=
          0) {
        fprintf(stderr, "  ✗ Token %d lexeme mismatch!\n", i);
        fprintf(stderr, "    Expected: '%s'\n", expected[i].lexeme);
        fprintf(stderr, "    Got:      '%.*s'\n", tok.length, tok.start);
        assert(0);
      }
      printf("  ✓ Token %d: '%.*s' -> %s\n", i, tok.length, tok.start,
             tokTypeName(tok.type));
    } else {
      printf("  ✓ Token %d: EOF\n", i);
    }
  }

  freeLexer(&lexer);
}

int main(void) {
  printf("Running lexer tests...\n\n");

  test_keywords();
  test_identifiers();
  test_single_char_tokens();
  test_two_char_tokens();
  test_strings();
  test_numbers();
  test_whitespace_and_comments();
  test_composite_statement();

  printf("\n✅ All tests passed!\n");
  return 0;
}
