#include "lexer.h"

#include <stdbool.h>
#include <string.h>

typedef bool (*CharPredicate)(unsigned char);

static Token token_init(TokenType type, size_t start, size_t length);
static Token read_ident_token(Lexer *lexer);
static Token read_number_token(Lexer *lexer);
static void read_char(Lexer *lexer);
static char peek_char(const Lexer *lexer);
static void skip_whitespace(Lexer *lexer);
static bool is_ident_char(unsigned char ch);
static bool is_digit(unsigned char ch);
static const char *read_lexeme(Lexer *lexer, size_t *len, CharPredicate predicate);
static TokenType lookup_identifier(const char *identifier, size_t len);

Lexer lexer_init(const char *input) {

	if (input == NULL)
		input = "";

	Lexer lexer = {
		.input = input,
		.input_len = strlen(input),
		.position = 0,
		.read_position = 0,
		.ch = '\0',
	};
	
	read_char(&lexer);

	return lexer;
}

Token lexer_next_token(Lexer *lexer) {

	Token token;

	skip_whitespace(lexer);

	if (is_ident_char((unsigned char)lexer->ch))
		return read_ident_token(lexer);

	if (is_digit((unsigned char)lexer->ch))
		return read_number_token(lexer);

	size_t start = lexer->position;

	switch (lexer->ch) {

		case '=':
			if (peek_char(lexer) == '=') {
				read_char(lexer);
				token = token_init(TOKEN_TYPE_EQ, start, 2);
			} else {
				token = token_init(TOKEN_TYPE_ASSIGN, start, 1);
			}
			break;
		case '!':
			if (peek_char(lexer) == '=') {
				read_char(lexer);
				token = token_init(TOKEN_TYPE_NOT_EQ, start, 2);
			} else {
				token = token_init(TOKEN_TYPE_BANG, start, 1);
			}
			break;
		case ';':
			token = token_init(TOKEN_TYPE_SEMICOLON, start, 1);
			break;
		case '(':
			token = token_init(TOKEN_TYPE_LPAREN, start, 1);
			break;
		case ')':
			token = token_init(TOKEN_TYPE_RPAREN, start, 1);
			break;
		case ',':
			token = token_init(TOKEN_TYPE_COMMA, start, 1);
			break;
		case '+':
			token = token_init(TOKEN_TYPE_PLUS, start, 1);
			break;
		case '-':
			token = token_init(TOKEN_TYPE_MINUS, start, 1);
			break;
		case '/':
			token = token_init(TOKEN_TYPE_SLASH, start, 1);
			break;
		case '*':
			token = token_init(TOKEN_TYPE_ASTERISK, start, 1);
			break;
		case '<':
			token = token_init(TOKEN_TYPE_LT, start, 1);
			break;
		case '>':
			token = token_init(TOKEN_TYPE_GT, start, 1);
			break;
		case '{':
			token = token_init(TOKEN_TYPE_LBRACE, start, 1);
			break;
		case '}':
			token = token_init(TOKEN_TYPE_RBRACE, start, 1);
			break;
		case '\0':
			return token_init(TOKEN_TYPE_EOF, start, 0);
		default:
			token = token_init(TOKEN_TYPE_ILLEGAL, start, 1);
			break;

	}

	read_char(lexer);
	return token;
}

static Token read_ident_token(Lexer *lexer) {
	size_t start = lexer->position;
	size_t len = 0;
	const char *identifier = read_lexeme(lexer, &len, is_ident_char);
	TokenType type = lookup_identifier(identifier, len);

	return token_init(type, start, len);
}

static Token read_number_token(Lexer *lexer) {
	size_t start = lexer->position;
	size_t len = 0;

	read_lexeme(lexer, &len, is_digit);

	return token_init(TOKEN_TYPE_INT, start, len);
}

static Token token_init(TokenType type, size_t start, size_t length) {

	Token token = {
		.type = type,
		.start = start,
		.length = length,
	};
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

static char peek_char(const Lexer *lexer) {
	if (lexer->read_position >= lexer->input_len)
		return '\0';
	return lexer->input[lexer->read_position];
}

static void skip_whitespace(Lexer *lexer) {
	while (lexer->ch == ' ' || lexer->ch == '\t' || lexer->ch == '\n' || lexer->ch == '\r')
		read_char(lexer);
}

static bool is_ident_char(unsigned char ch) {
	return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') || ch == '_';
}

static bool is_digit(unsigned char ch) {
	return ch >= '0' && ch <= '9';
}

static const char *read_lexeme(Lexer *lexer, size_t *len, CharPredicate predicate) {

	size_t start = lexer->position;

	while (predicate((unsigned char)lexer->ch))
		read_char(lexer);

	if (len != NULL)
		*len = lexer->position - start;

	return lexer->input + start;
}

static TokenType lookup_identifier(const char *identifier, size_t len) {

	if (len == 2 && strncmp(identifier, "fn", len) == 0)
		return TOKEN_TYPE_FUNCTION;
	if (len == 2 && strncmp(identifier, "if", len) == 0)
		return TOKEN_TYPE_IF;
	if (len == 3 && strncmp(identifier, "let", len) == 0)
		return TOKEN_TYPE_LET;
	if (len == 4 && strncmp(identifier, "true", len) == 0)
		return TOKEN_TYPE_TRUE;
	if (len == 4 && strncmp(identifier, "else", len) == 0)
		return TOKEN_TYPE_ELSE;
	if (len == 5 && strncmp(identifier, "false", len) == 0)
		return TOKEN_TYPE_FALSE;
	if (len == 6 && strncmp(identifier, "return", len) == 0)
		return TOKEN_TYPE_RETURN;

	return TOKEN_TYPE_IDENT;
}
