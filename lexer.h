#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>

typedef struct Lexer {
	const char *input;
	size_t input_len;
	size_t position;
	size_t read_position;
	char ch;
} Lexer;

typedef enum {
	TOKEN_TYPE_ILLEGAL,
	TOKEN_TYPE_EOF,
	TOKEN_TYPE_IDENT,
	TOKEN_TYPE_INT,
	TOKEN_TYPE_ASSIGN,
	TOKEN_TYPE_EQ,
	TOKEN_TYPE_NOT_EQ,
	TOKEN_TYPE_BANG,
	TOKEN_TYPE_PLUS,
	TOKEN_TYPE_MINUS,
	TOKEN_TYPE_SLASH,
	TOKEN_TYPE_ASTERISK,
	TOKEN_TYPE_LT,
	TOKEN_TYPE_GT,
	TOKEN_TYPE_COMMA,
	TOKEN_TYPE_SEMICOLON,
	TOKEN_TYPE_LPAREN,
	TOKEN_TYPE_RPAREN,
	TOKEN_TYPE_LBRACE,
	TOKEN_TYPE_RBRACE,
	TOKEN_TYPE_FUNCTION,
	TOKEN_TYPE_LET,
	TOKEN_TYPE_TRUE,
	TOKEN_TYPE_FALSE,
	TOKEN_TYPE_IF,
	TOKEN_TYPE_ELSE,
	TOKEN_TYPE_RETURN,
} TokenType;

typedef struct {
	TokenType type;
	size_t start;
	size_t length;
} Token;

Lexer lexer_init(const char *input);
Token lexer_next_token(Lexer *lexer);

#endif
