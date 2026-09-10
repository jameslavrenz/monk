# lexer

A small C lexer for a Monkey-style language (`let`, `fn`, identifiers, integers, and a handful of punctuation). Call `lexer_next_token` until you get `TOKEN_TYPE_EOF`.

## Files

| File | Role |
|---|---|
| `lexer.h` | Public API: `Lexer`, `Token`, `TokenType` |
| `lexer.c` | Implementation |
| `main.c` | Tests |
| `Makefile` | Build |

`Lexer` is opaque. `Token` is a public struct: `type` plus an optional `literal` string.

## Build and test

```sh
make
./lexer
```

`make clean` removes the binary and object files. `make run` builds and runs the tests.

Three tests run:

1. Punctuation: `=+(){},;`
2. A short program with `let` / `fn` / identifiers / integers
3. Illegal character: `@`

Each test also checks that the next token after the list is `TOKEN_TYPE_EOF`.

## Public API

```c
Lexer *lexer_create(const char *input);
void lexer_destroy(Lexer *lexer);
Token *lexer_next_token(Lexer *lexer);
void token_destroy(Token *token);
```

- `lexer_create` copies the pointer to `input`, not the bytes. The string must stay alive until `lexer_destroy`.
- Each `Token *` from `lexer_next_token` is owned by the caller. Free it with `token_destroy` (that also frees `literal` if it is non-`NULL`).
- `token_create` is internal. Only the lexer constructs tokens.

Typical loop:

```c
Lexer *lexer = lexer_create(source);
Token *token;

while ((token = lexer_next_token(lexer))->type != TOKEN_TYPE_EOF) {
	/* use token->type and token->literal */
	token_destroy(token);
}
token_destroy(token); /* the EOF token */
lexer_destroy(lexer);
```

## Tokens

| Type | Source | `literal` |
|---|---|---|
| `TOKEN_TYPE_IDENT` | `five`, `add`, `_x` | copy of the name |
| `TOKEN_TYPE_INT` | `5`, `10` | copy of the digits |
| `TOKEN_TYPE_LET` | `let` | `NULL` |
| `TOKEN_TYPE_FUNCTION` | `fn` | `NULL` |
| `TOKEN_TYPE_ASSIGN` … `TOKEN_TYPE_RBRACE` | `=` `+` `(` `)` `{` `}` `,` `;` | `NULL` |
| `TOKEN_TYPE_ILLEGAL` | anything else, e.g. `@` | `NULL` |
| `TOKEN_TYPE_EOF` | end of input | `NULL` |

Identifiers: ASCII letters and `_`. They may not start with a digit. Keywords are matched by exact spelling and length (`let`, `fn`); everything else is `TOKEN_TYPE_IDENT`.

Integers are ASCII digits only (no sign, no decimal point).

Whitespace (` `, tab, `\n`, `\r`) is skipped. `\r` is included so Windows `\r\n` line endings are not treated as illegal tokens.

## How `lexer_next_token` works

1. Skip whitespace.
2. If the current character is an identifier character, read a span and look up keywords.
3. If it is a digit, read a span of digits.
4. Otherwise switch on a single character: punctuation, `'\0'` → EOF, anything else → ILLEGAL.

Spans share `read_lexeme`, which walks while a predicate (`is_ident_char` or `is_digit`) is true. Ident and number paths consume characters inside that loop. Punctuation and illegal tokens consume one character with `read_char` after the `switch`. EOF returns without that extra advance.

The lexer keeps both `position` (current character) and `read_position` (look-ahead index), with `ch` as the character at `position`.

## Naming

- Types: `Lexer`, `Token`, `TokenType`, `CharPredicate`
- Functions: `lexer_create`, `token_destroy`, `read_lexeme`
- Enum values: `TOKEN_TYPE_IDENT`

Public functions are `type_verb`. File-private lexer helpers drop the `lexer_` prefix (`read_char`, `skip_whitespace`).
