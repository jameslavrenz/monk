#include "lexer.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef bool (*CharPredicate)(char);

struct Lexer {
	const char *input;
	size_t input_len;
	size_t position;
	size_t read_position;
	char ch;
};

static Token *token_create(TokenType type, char *literal);
static Token *read_ident_token(Lexer *lexer);
static Token *read_number_token(Lexer *lexer);
static void read_char(Lexer *lexer);
static void skip_whitespace(Lexer *lexer);
static bool is_ident_char(char ch);
static bool is_digit(char ch);
static const char *read_lexeme(Lexer *lexer, size_t *len, CharPredicate predicate);
static TokenType lookup_identifier(const char *identifier, size_t len);

Lexer *lexer_create(const char *input) {

	Lexer *lexer = calloc(1, sizeof *lexer);

	if (lexer == NULL)
		return NULL;

	lexer->input = input;
	lexer->input_len = strlen(input);

	read_char(lexer);

	return lexer;
}

void lexer_destroy(Lexer *lexer) {
	free(lexer);
}

void token_destroy(Token *token) {
	if (token == NULL)
		return;
	free(token->literal);
	free(token);
}

Token *lexer_next_token(Lexer *lexer) {

	Token *token = NULL;

	skip_whitespace(lexer);

	if (is_ident_char(lexer->ch))
		return read_ident_token(lexer);

	if (is_digit(lexer->ch))
		return read_number_token(lexer);

	switch (lexer->ch) {

		case '=':
			token = token_create(TOKEN_TYPE_ASSIGN, NULL);
			break;
		case ';':
			token = token_create(TOKEN_TYPE_SEMICOLON, NULL);
			break;
		case '(':
			token = token_create(TOKEN_TYPE_LPAREN, NULL);
			break;
		case ')':
			token = token_create(TOKEN_TYPE_RPAREN, NULL);
			break;
		case ',':
			token = token_create(TOKEN_TYPE_COMMA, NULL);
			break;
		case '+':
			token = token_create(TOKEN_TYPE_PLUS, NULL);
			break;
		case '{':
			token = token_create(TOKEN_TYPE_LBRACE, NULL);
			break;
		case '}':
			token = token_create(TOKEN_TYPE_RBRACE, NULL);
			break;
		case '\0':
			return token_create(TOKEN_TYPE_EOF, NULL);
		default:
			token = token_create(TOKEN_TYPE_ILLEGAL, NULL);
			break;

	}

	read_char(lexer);
	return token;
}

static Token *read_ident_token(Lexer *lexer) {
	size_t len = 0;
	char *literal = NULL;

	const char *identifier = read_lexeme(lexer, &len, is_ident_char);
	TokenType type = lookup_identifier(identifier, len);

	if (type == TOKEN_TYPE_IDENT)
		literal = strndup(identifier, len);

	return token_create(type, literal);
}

static Token *read_number_token(Lexer *lexer) {
	size_t len = 0;
	const char *number = read_lexeme(lexer, &len, is_digit);
	char *literal = strndup(number, len);

	return token_create(TOKEN_TYPE_INT, literal);
}

static Token *token_create(TokenType type, char *literal) {

	Token *token = malloc(sizeof *token);

	if (token == NULL) {
		return NULL;
	}

	token->type = type;
	token->literal = literal;

	return token;
}

static void read_char(Lexer *lexer) {

	if (lexer->read_position >= lexer->input_len)
		lexer->ch = '\0';
	else
		lexer->ch = lexer->input[lexer->read_position];

	lexer->position = lexer->read_position;
	lexer->read_position++;
}

static void skip_whitespace(Lexer *lexer) {
	while (lexer->ch == ' ' || lexer->ch == '\t' || lexer->ch == '\n' || lexer->ch == '\r')
		read_char(lexer);
}

static bool is_ident_char(char ch) {
	return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_';
}

static bool is_digit(char ch) {
	return ch >= '0' && ch <= '9';
}

static const char *read_lexeme(Lexer *lexer, size_t *len, CharPredicate predicate) {

	size_t start = lexer->position;

	while (predicate(lexer->ch))
		read_char(lexer);

	if (len != NULL)
		*len = lexer->position - start;

	return lexer->input + start;
}

static TokenType lookup_identifier(const char *identifier, size_t len) {

	if (len == 3 && strncmp(identifier, "let", len) == 0)
		return TOKEN_TYPE_LET;
	if (len == 2 && strncmp(identifier, "fn", len) == 0)
		return TOKEN_TYPE_FUNCTION;

	return TOKEN_TYPE_IDENT;
}
