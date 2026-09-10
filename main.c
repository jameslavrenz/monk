#include "lexer.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct {
	TokenType type;
	const char *text;
} ExpectedToken;

static void assert_span(const char *input, const Token *token, const char *text) {
	size_t len = strlen(text);

	assert(token->length == len);
	assert(memcmp(input + token->start, text, len) == 0);
}

static void run_lexer_test(const char *input, const ExpectedToken *expected, size_t expected_count) {

	Lexer *lexer = lexer_create(input);
	assert(lexer != NULL);

	for (size_t i = 0; i < expected_count; i++) {
		Token *token = lexer_next_token(lexer);

		assert(token != NULL);
		assert(token->type == expected[i].type);
		assert_span(input, token, expected[i].text);

		token_destroy(token);
	}

	Token *eof = lexer_next_token(lexer);
	assert(eof != NULL);
	assert(eof->type == TOKEN_TYPE_EOF);
	assert(eof->length == 0);
	assert(eof->start == strlen(input));
	token_destroy(eof);

	lexer_destroy(lexer);
}

int main(void) {
	const char *input = "=+(){},;";

	const ExpectedToken expected[] = {
		{ TOKEN_TYPE_ASSIGN, "=" },
		{ TOKEN_TYPE_PLUS, "+" },
		{ TOKEN_TYPE_LPAREN, "(" },
		{ TOKEN_TYPE_RPAREN, ")" },
		{ TOKEN_TYPE_LBRACE, "{" },
		{ TOKEN_TYPE_RBRACE, "}" },
		{ TOKEN_TYPE_COMMA, "," },
		{ TOKEN_TYPE_SEMICOLON, ";" },
	};

	run_lexer_test(input, expected, sizeof(expected) / sizeof(expected[0]));

	printf("first test passed!\n");

	const char *program =
		"let five = 5; "
		"let ten = 10; "
		"let add = fn(x, y){ x + y; }; "
		"let result = add(five, ten);";

	const ExpectedToken expected_program[] = {
		{ TOKEN_TYPE_LET, "let" },
		{ TOKEN_TYPE_IDENT, "five" },
		{ TOKEN_TYPE_ASSIGN, "=" },
		{ TOKEN_TYPE_INT, "5" },
		{ TOKEN_TYPE_SEMICOLON, ";" },

		{ TOKEN_TYPE_LET, "let" },
		{ TOKEN_TYPE_IDENT, "ten" },
		{ TOKEN_TYPE_ASSIGN, "=" },
		{ TOKEN_TYPE_INT, "10" },
		{ TOKEN_TYPE_SEMICOLON, ";" },

		{ TOKEN_TYPE_LET, "let" },
		{ TOKEN_TYPE_IDENT, "add" },
		{ TOKEN_TYPE_ASSIGN, "=" },
		{ TOKEN_TYPE_FUNCTION, "fn" },
		{ TOKEN_TYPE_LPAREN, "(" },
		{ TOKEN_TYPE_IDENT, "x" },
		{ TOKEN_TYPE_COMMA, "," },
		{ TOKEN_TYPE_IDENT, "y" },
		{ TOKEN_TYPE_RPAREN, ")" },
		{ TOKEN_TYPE_LBRACE, "{" },
		{ TOKEN_TYPE_IDENT, "x" },
		{ TOKEN_TYPE_PLUS, "+" },
		{ TOKEN_TYPE_IDENT, "y" },
		{ TOKEN_TYPE_SEMICOLON, ";" },
		{ TOKEN_TYPE_RBRACE, "}" },
		{ TOKEN_TYPE_SEMICOLON, ";" },

		{ TOKEN_TYPE_LET, "let" },
		{ TOKEN_TYPE_IDENT, "result" },
		{ TOKEN_TYPE_ASSIGN, "=" },
		{ TOKEN_TYPE_IDENT, "add" },
		{ TOKEN_TYPE_LPAREN, "(" },
		{ TOKEN_TYPE_IDENT, "five" },
		{ TOKEN_TYPE_COMMA, "," },
		{ TOKEN_TYPE_IDENT, "ten" },
		{ TOKEN_TYPE_RPAREN, ")" },
		{ TOKEN_TYPE_SEMICOLON, ";" },
	};

	run_lexer_test(program, expected_program, sizeof(expected_program) / sizeof(expected_program[0]));

	printf("second test passed!\n");

	const ExpectedToken expected_illegal[] = {
		{ TOKEN_TYPE_ILLEGAL, "@" },
	};

	run_lexer_test("@", expected_illegal, sizeof(expected_illegal) / sizeof(expected_illegal[0]));

	printf("third test passed!\n");

	const char *ops = "10 == 10 != 9 !true =";

	const ExpectedToken expected_ops[] = {
		{ TOKEN_TYPE_INT, "10" },
		{ TOKEN_TYPE_EQ, "==" },
		{ TOKEN_TYPE_INT, "10" },
		{ TOKEN_TYPE_NOT_EQ, "!=" },
		{ TOKEN_TYPE_INT, "9" },
		{ TOKEN_TYPE_BANG, "!" },
		{ TOKEN_TYPE_TRUE, "true" },
		{ TOKEN_TYPE_ASSIGN, "=" },
	};

	run_lexer_test(ops, expected_ops, sizeof(expected_ops) / sizeof(expected_ops[0]));

	printf("fourth test passed!\n");

	const char *arithmetic = "!-/*<>";

	const ExpectedToken expected_arithmetic[] = {
		{ TOKEN_TYPE_BANG, "!" },
		{ TOKEN_TYPE_MINUS, "-" },
		{ TOKEN_TYPE_SLASH, "/" },
		{ TOKEN_TYPE_ASTERISK, "*" },
		{ TOKEN_TYPE_LT, "<" },
		{ TOKEN_TYPE_GT, ">" },
	};

	run_lexer_test(arithmetic, expected_arithmetic, sizeof(expected_arithmetic) / sizeof(expected_arithmetic[0]));

	printf("fifth test passed!\n");

	const char *full =
		"let x = 10 * 2 / 1 - 5; "
		"if (x < 20) { "
		"return x != 10; "
		"} else { "
		"return false; "
		"} "
		"x == 15; "
		"x > 0;";

	const ExpectedToken expected_full[] = {
		{ TOKEN_TYPE_LET, "let" },
		{ TOKEN_TYPE_IDENT, "x" },
		{ TOKEN_TYPE_ASSIGN, "=" },
		{ TOKEN_TYPE_INT, "10" },
		{ TOKEN_TYPE_ASTERISK, "*" },
		{ TOKEN_TYPE_INT, "2" },
		{ TOKEN_TYPE_SLASH, "/" },
		{ TOKEN_TYPE_INT, "1" },
		{ TOKEN_TYPE_MINUS, "-" },
		{ TOKEN_TYPE_INT, "5" },
		{ TOKEN_TYPE_SEMICOLON, ";" },

		{ TOKEN_TYPE_IF, "if" },
		{ TOKEN_TYPE_LPAREN, "(" },
		{ TOKEN_TYPE_IDENT, "x" },
		{ TOKEN_TYPE_LT, "<" },
		{ TOKEN_TYPE_INT, "20" },
		{ TOKEN_TYPE_RPAREN, ")" },
		{ TOKEN_TYPE_LBRACE, "{" },
		{ TOKEN_TYPE_RETURN, "return" },
		{ TOKEN_TYPE_IDENT, "x" },
		{ TOKEN_TYPE_NOT_EQ, "!=" },
		{ TOKEN_TYPE_INT, "10" },
		{ TOKEN_TYPE_SEMICOLON, ";" },
		{ TOKEN_TYPE_RBRACE, "}" },
		{ TOKEN_TYPE_ELSE, "else" },
		{ TOKEN_TYPE_LBRACE, "{" },
		{ TOKEN_TYPE_RETURN, "return" },
		{ TOKEN_TYPE_FALSE, "false" },
		{ TOKEN_TYPE_SEMICOLON, ";" },
		{ TOKEN_TYPE_RBRACE, "}" },

		{ TOKEN_TYPE_IDENT, "x" },
		{ TOKEN_TYPE_EQ, "==" },
		{ TOKEN_TYPE_INT, "15" },
		{ TOKEN_TYPE_SEMICOLON, ";" },

		{ TOKEN_TYPE_IDENT, "x" },
		{ TOKEN_TYPE_GT, ">" },
		{ TOKEN_TYPE_INT, "0" },
		{ TOKEN_TYPE_SEMICOLON, ";" },
	};

	run_lexer_test(full, expected_full, sizeof(expected_full) / sizeof(expected_full[0]));

	printf("sixth test passed!\n");
	return 0;
}
