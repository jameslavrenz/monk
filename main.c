#include "lexer.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct {
	TokenType type;
	const char *text;
} ExpectedToken;

static const char *source_text(const char *input) {
	return input != NULL ? input : "";
}

static bool check_span(const char *source, const Token *token, const char *text) {
	size_t len = strlen(text);

	return token->length == len && memcmp(source + token->start, text, len) == 0;
}

static bool run_lexer_test(const char *name, const char *input, const ExpectedToken *expected, size_t expected_count) {
	const char *source = source_text(input);
	Lexer lexer = lexer_init(input);

	for (size_t i = 0; i < expected_count; i++) {
		Token token = lexer_next_token(&lexer);

		if (token.type != expected[i].type) {
			fprintf(stderr, "FAIL %s: token %zu type %d, want %d\n",
				name, i, (int)token.type, (int)expected[i].type);
			return false;
		}

		if (!check_span(source, &token, expected[i].text)) {
			fprintf(stderr, "FAIL %s: token %zu span\n", name, i);
			return false;
		}
	}

	Token eof = lexer_next_token(&lexer);

	if (eof.type != TOKEN_TYPE_EOF || eof.length != 0 || eof.start != strlen(source)) {
		fprintf(stderr, "FAIL %s: EOF type %d start %zu length %zu, want start %zu\n",
			name, (int)eof.type, eof.start, eof.length, strlen(source));
		return false;
	}

	printf("%s passed\n", name);
	return true;
}

int main(void) {
	int failed = 0;

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

	failed += !run_lexer_test("punctuation", input, expected, sizeof(expected) / sizeof(expected[0]));

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

	failed += !run_lexer_test("program", program, expected_program, sizeof(expected_program) / sizeof(expected_program[0]));

	const ExpectedToken expected_illegal[] = {
		{ TOKEN_TYPE_ILLEGAL, "@" },
	};

	failed += !run_lexer_test("illegal", "@", expected_illegal, sizeof(expected_illegal) / sizeof(expected_illegal[0]));

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

	failed += !run_lexer_test("operators", ops, expected_ops, sizeof(expected_ops) / sizeof(expected_ops[0]));

	const char *arithmetic = "!-/*<>";

	const ExpectedToken expected_arithmetic[] = {
		{ TOKEN_TYPE_BANG, "!" },
		{ TOKEN_TYPE_MINUS, "-" },
		{ TOKEN_TYPE_SLASH, "/" },
		{ TOKEN_TYPE_ASTERISK, "*" },
		{ TOKEN_TYPE_LT, "<" },
		{ TOKEN_TYPE_GT, ">" },
	};

	failed += !run_lexer_test("arithmetic", arithmetic, expected_arithmetic, sizeof(expected_arithmetic) / sizeof(expected_arithmetic[0]));

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

	failed += !run_lexer_test("full", full, expected_full, sizeof(expected_full) / sizeof(expected_full[0]));

	failed += !run_lexer_test("empty", "", NULL, 0);
	failed += !run_lexer_test("null", NULL, NULL, 0);
	failed += !run_lexer_test("whitespace", " \t\n\r ", NULL, 0);

	const ExpectedToken expected_eq_assign[] = {
		{ TOKEN_TYPE_EQ, "==" },
		{ TOKEN_TYPE_ASSIGN, "=" },
	};

	failed += !run_lexer_test("triple-eq", "===", expected_eq_assign, sizeof(expected_eq_assign) / sizeof(expected_eq_assign[0]));

	const ExpectedToken expected_illegals[] = {
		{ TOKEN_TYPE_ILLEGAL, "@" },
		{ TOKEN_TYPE_ILLEGAL, "$" },
	};

	failed += !run_lexer_test("illegals", "@$", expected_illegals, sizeof(expected_illegals) / sizeof(expected_illegals[0]));

	if (failed != 0) {
		fprintf(stderr, "%d test(s) failed\n", failed);
		return 1;
	}

	return 0;
}
