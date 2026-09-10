#include "lexer.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

typedef struct {
	TokenType type;
	const char *literal;
} ExpectedToken;

static void run_lexer_test(const char *input, const ExpectedToken *expected, size_t expected_count) {

	Lexer *lexer = lexer_create(input);
	assert(lexer != NULL);

	for (size_t i = 0; i < expected_count; i++) {
		Token *token = lexer_next_token(lexer);

		assert(token != NULL);
		assert(token->type == expected[i].type);

		if (expected[i].literal == NULL)
			assert(token->literal == NULL);
		else
			assert(token->literal != NULL && strcmp(token->literal, expected[i].literal) == 0);

		token_destroy(token);
	}

	Token *eof = lexer_next_token(lexer);
	assert(eof != NULL);
	assert(eof->type == TOKEN_TYPE_EOF);
	assert(eof->literal == NULL);
	token_destroy(eof);

	lexer_destroy(lexer);
}

int main(void) {
	const char *input = "=+(){},;";

	const ExpectedToken expected[] = {
		{ TOKEN_TYPE_ASSIGN, NULL },
		{ TOKEN_TYPE_PLUS, NULL },
		{ TOKEN_TYPE_LPAREN, NULL },
		{ TOKEN_TYPE_RPAREN, NULL },
		{ TOKEN_TYPE_LBRACE, NULL },
		{ TOKEN_TYPE_RBRACE, NULL },
		{ TOKEN_TYPE_COMMA, NULL },
		{ TOKEN_TYPE_SEMICOLON, NULL },
	};

	run_lexer_test(input, expected, sizeof(expected) / sizeof(expected[0]));

	printf("first test passed!\n");

	const char *program =
		"let five = 5; "
		"let ten = 10; "
		"let add = fn(x, y){ x + y; }; "
		"let result = add(five, ten);";

	const ExpectedToken expected_program[] = {
		{ TOKEN_TYPE_LET, NULL },
		{ TOKEN_TYPE_IDENT, "five" },
		{ TOKEN_TYPE_ASSIGN, NULL },
		{ TOKEN_TYPE_INT, "5" },
		{ TOKEN_TYPE_SEMICOLON, NULL },

		{ TOKEN_TYPE_LET, NULL },
		{ TOKEN_TYPE_IDENT, "ten" },
		{ TOKEN_TYPE_ASSIGN, NULL },
		{ TOKEN_TYPE_INT, "10" },
		{ TOKEN_TYPE_SEMICOLON, NULL },

		{ TOKEN_TYPE_LET, NULL },
		{ TOKEN_TYPE_IDENT, "add" },
		{ TOKEN_TYPE_ASSIGN, NULL },
		{ TOKEN_TYPE_FUNCTION, NULL },
		{ TOKEN_TYPE_LPAREN, NULL },
		{ TOKEN_TYPE_IDENT, "x" },
		{ TOKEN_TYPE_COMMA, NULL },
		{ TOKEN_TYPE_IDENT, "y" },
		{ TOKEN_TYPE_RPAREN, NULL },
		{ TOKEN_TYPE_LBRACE, NULL },
		{ TOKEN_TYPE_IDENT, "x" },
		{ TOKEN_TYPE_PLUS, NULL },
		{ TOKEN_TYPE_IDENT, "y" },
		{ TOKEN_TYPE_SEMICOLON, NULL },
		{ TOKEN_TYPE_RBRACE, NULL },
		{ TOKEN_TYPE_SEMICOLON, NULL },

		{ TOKEN_TYPE_LET, NULL },
		{ TOKEN_TYPE_IDENT, "result" },
		{ TOKEN_TYPE_ASSIGN, NULL },
		{ TOKEN_TYPE_IDENT, "add" },
		{ TOKEN_TYPE_LPAREN, NULL },
		{ TOKEN_TYPE_IDENT, "five" },
		{ TOKEN_TYPE_COMMA, NULL },
		{ TOKEN_TYPE_IDENT, "ten" },
		{ TOKEN_TYPE_RPAREN, NULL },
		{ TOKEN_TYPE_SEMICOLON, NULL },
	};

	run_lexer_test(program, expected_program, sizeof(expected_program) / sizeof(expected_program[0]));

	printf("second test passed!\n");

	const ExpectedToken expected_illegal[] = {
		{ TOKEN_TYPE_ILLEGAL, NULL },
	};

	run_lexer_test("@", expected_illegal, sizeof(expected_illegal) / sizeof(expected_illegal[0]));

	printf("third test passed!\n");
	return 0;
}
