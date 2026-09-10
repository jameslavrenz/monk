#ifndef LEXER_H
#define LEXER_H

typedef struct Lexer Lexer;

typedef enum {
	TOKEN_TYPE_ILLEGAL,
	TOKEN_TYPE_EOF,
	TOKEN_TYPE_IDENT,
	TOKEN_TYPE_INT,
	TOKEN_TYPE_ASSIGN,
	TOKEN_TYPE_PLUS,
	TOKEN_TYPE_COMMA,
	TOKEN_TYPE_SEMICOLON,
	TOKEN_TYPE_LPAREN,
	TOKEN_TYPE_RPAREN,
	TOKEN_TYPE_LBRACE,
	TOKEN_TYPE_RBRACE,
	TOKEN_TYPE_FUNCTION,
	TOKEN_TYPE_LET,
} TokenType;

typedef struct {
	TokenType type;
	char *literal;
} Token;

Lexer *lexer_create(const char *input);
void lexer_destroy(Lexer *lexer);
Token *lexer_next_token(Lexer *lexer);
void token_destroy(Token *token);

#endif
