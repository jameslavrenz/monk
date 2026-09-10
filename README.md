# monk

A C interpreter for a Monkey-style language. The program is called `monk`.

Right now `monk` is a lexer: it turns source text into a stream of tokens (`let`, `fn`, `if`, `return`, identifiers, integers, and operators). Call `lexer_next_token` until you get `TOKEN_TYPE_EOF`. There is no parser yet.

## Files

| File | Role |
|---|---|
| `lexer.h` | Public API: `Lexer`, `Token`, `TokenType` |
| `lexer.c` | Implementation |
| `main.c` | Tests |
| `Makefile` | Build |

`Lexer` is opaque. `Token` is a public struct: `type` plus a span (`start` and `length`) into the original input. The span is not a copy of the text.

## Build and test

The project is compiled as **C17** (`-std=c17 -Wall -Wextra -Wpedantic`).

```sh
make
./monk
```

| Target | What it does |
|---|---|
| `make` / `make all` | Build the `monk` binary |
| `make run` | Build and run the tests |
| `make clean` | Remove object files and the binary |
| `make rebuild` | `clean`, then `all` |

Six tests run:

1. Punctuation: `=+(){},;`
2. A short program with `let` / `fn` / identifiers / integers
3. Illegal character: `@`
4. Two-character operators: `==` `!=`, plus lone `!` and `=`; `true` is a keyword
5. Remaining one-character operators: `!-/*<>`
6. A fuller program mixing `let`, `if` / `else`, `return`, `true` / `false`, `==` / `!=`, and `+ - * / < >`

Each test checks token `type` and the span text, then that the next token is `TOKEN_TYPE_EOF` with `length == 0` at the end of the input.

## Public API

```c
typedef struct {
	TokenType type;
	size_t start;
	size_t length;
} Token;

Lexer *lexer_create(const char *input);
void lexer_destroy(Lexer *lexer);
Token *lexer_next_token(Lexer *lexer);
void token_destroy(Token *token);
```

- `lexer_create` stores the pointer to `input`, not a copy of the bytes. The string must stay alive as long as any `Token` span is used.
- Each `Token *` from `lexer_next_token` is owned by the caller. Free it with `token_destroy`.
- `start` / `length` are a half-open range: bytes `input[start .. start+length)`. EOF has `length == 0`.
- `token_create` is internal. Only the lexer constructs tokens.

Typical loop:

```c
Lexer *lexer = lexer_create(source);
Token *token;

while ((token = lexer_next_token(lexer))->type != TOKEN_TYPE_EOF) {
	/* use token->type and source[token->start .. token->start + token->length) */
	token_destroy(token);
}
token_destroy(token); /* the EOF token */
lexer_destroy(lexer);
```

## Tokens

| Type | Source | Span |
|---|---|---|
| `TOKEN_TYPE_IDENT` | `five`, `add`, `_x` | the name |
| `TOKEN_TYPE_INT` | `5`, `10` | the digits |
| `TOKEN_TYPE_LET` | `let` | `let` |
| `TOKEN_TYPE_FUNCTION` | `fn` | `fn` |
| `TOKEN_TYPE_TRUE` | `true` | `true` |
| `TOKEN_TYPE_FALSE` | `false` | `false` |
| `TOKEN_TYPE_IF` | `if` | `if` |
| `TOKEN_TYPE_ELSE` | `else` | `else` |
| `TOKEN_TYPE_RETURN` | `return` | `return` |
| `TOKEN_TYPE_ASSIGN` | `=` | `=` |
| `TOKEN_TYPE_EQ` | `==` | `==` |
| `TOKEN_TYPE_NOT_EQ` | `!=` | `!=` |
| `TOKEN_TYPE_BANG` | `!` | `!` |
| `TOKEN_TYPE_PLUS` | `+` | `+` |
| `TOKEN_TYPE_MINUS` | `-` | `-` |
| `TOKEN_TYPE_SLASH` | `/` | `/` |
| `TOKEN_TYPE_ASTERISK` | `*` | `*` |
| `TOKEN_TYPE_LT` | `<` | `<` |
| `TOKEN_TYPE_GT` | `>` | `>` |
| `TOKEN_TYPE_COMMA` | `,` | `,` |
| `TOKEN_TYPE_SEMICOLON` | `;` | `;` |
| `TOKEN_TYPE_LPAREN` | `(` | `(` |
| `TOKEN_TYPE_RPAREN` | `)` | `)` |
| `TOKEN_TYPE_LBRACE` | `{` | `{` |
| `TOKEN_TYPE_RBRACE` | `}` | `}` |
| `TOKEN_TYPE_ILLEGAL` | anything else, e.g. `@` | that character |
| `TOKEN_TYPE_EOF` | end of input | empty (`length == 0`) |

Identifiers: ASCII letters and `_`. They may not start with a digit. Keywords are matched by exact spelling and length (`fn`, `if`, `let`, `true`, `else`, `false`, `return`); everything else is `TOKEN_TYPE_IDENT`.

Integers are ASCII digits only (no sign, no decimal point).

Whitespace (` `, tab, `\n`, `\r`) is skipped. `\r` is included so Windows `\r\n` line endings are not treated as illegal tokens.

`=` vs `==` and `!` vs `!=` use one character of look-ahead (`peek_char`). `===` is `EQ` then `ASSIGN`.

## How `lexer_next_token` works

1. Skip whitespace.
2. If the current character is an identifier character, read a span and look up keywords (`lookup_identifier`).
3. If it is a digit, read a span of digits.
4. Otherwise switch on the current character: operators and punctuation, `==` / `!=` via `peek_char`, `'\0'` → EOF, anything else → ILLEGAL.

Spans share `read_lexeme`, which walks while a predicate (`is_ident_char` or `is_digit`) is true. Ident and number paths consume characters inside that loop. One-character punctuation and illegal tokens consume one character with `read_char` after the `switch`. Two-character operators consume the second character inside the case, then that same trailing `read_char`. EOF returns without that extra advance.

The lexer keeps both `position` (current character) and `read_position` (look-ahead index), with `ch` as the character at `position`. `peek_char` reads `input[read_position]` without advancing.

## Naming

- Types: `Lexer`, `Token`, `TokenType`, `CharPredicate`
- Functions: `lexer_create`, `lexer_destroy`, `token_destroy`, `read_char`, `peek_char`, `read_lexeme`
- Enum values: `TOKEN_TYPE_IDENT`, `TOKEN_TYPE_EQ`, `TOKEN_TYPE_NOT_EQ`

Public functions are `type_verb`. File-private lexer helpers drop the `lexer_` prefix (`read_char`, `skip_whitespace`, `peek_char`).
