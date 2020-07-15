#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "jitcalc.h"
#include "token.h"

void print_token_kind(FILE* file, TokenKind kind) {
	switch (kind) {
		case EOFToken:        fprintf(file, "<eof>");    break;
		case LiteralToken:    fprintf(file, "<number>"); break;
		case LeftParenToken:  fprintf(file, "(");        break;
		case RightParenToken: fprintf(file, ")");        break;
		case PlusToken:       fprintf(file, "+");        break;
		case MinusToken:      fprintf(file, "-");        break;
		case AsteriskToken:   fprintf(file, "*");        break;
		case SlashToken:      fprintf(file, "/");        break;
	}
}

void print_token(FILE* file, Token token) {
	switch (token.kind) {
		case LiteralToken:
			switch (token.value_kind) {
				case IntegerValue:  fprintf(file, "%ld", token.integer);  break;
				case FloatingValue: fprintf(file, "%lg", token.floating); break;
			}
			break;
		default:
			print_token_kind(file, token.kind);
	}
}

void add_token(Token** tokens, u64* len, u64* cap, Token token) {
	if (*len + 1 < *cap) {
		*cap *= 2;
		*tokens = realloc(*tokens, *cap * sizeof token);
	}
	(*tokens)[*len] = token;
	*len += 1;
}

Tokenizer new_tokenizer(char* str) {
	return (Tokenizer) {
		.line = 1,
		.col = 1,
		.i = 0,
		.str = str,
		.len = strlen(str),
	};
}

void putback_token(Tokenizer* t, Token token) {
	t->line = token.line;
	t->col = token.col;
	t->i = token.i;
}

Token next_token(Tokenizer* t) {
	#define ch t->str[t->i]
start:
	if (t->i >= t->len) {
		return (Token) {
			.kind = EOFToken,
			.line = t->line,
			.col = t->col,
			.i = t->i,
		};
	}

	switch (ch) {
		case '(': case ')': case '+': case '-': case '*': case '/': {
			Token token = (Token) {
				.kind = ch,
				.line = t->line,
				.col = t->col,
				.i = t->i,
			};
			t->i++;
			t->col++;
			return token;
		}
	}

	if (isdigit(ch) || ch == '.') {
		u64 value = 0;

		u64 start_col = t->col;
		u64 start_i = t->i;

		while (t->i < t->len && (isdigit(ch) || ch == '.')) {
			if (ch == '.') {
				char* end;
				double value = strtod(&t->str[start_i], &end);
				u64 chars_read = end - &t->str[start_i];
				if (chars_read == 0) {
					fprintf(stderr,
						"ERROR: Invalid floating point number at %ld:%ld\n",
						t->line, start_col);
					t->i++;
					t->col++;
					goto start;
				}
				t->i = start_i + chars_read;
				t->col = start_col + chars_read;
				return (Token) {
					.kind = LiteralToken,
					.line = t->line,
					.col = start_col,
					.i = start_i,

					.value_kind = FloatingValue,
					.floating = value,
				};
			}

			value *= 10;
			value += ch - '0';
			t->i++;
			t->col++;
		}

		Token token = (Token) {
			.kind = LiteralToken,
			.line = t->line,
			.col = start_col,
			.i = start_i,

			.value_kind = IntegerValue,
			.integer = value,
		};

		return token;
	} else if (isspace(ch)) {
		if (ch == '\n') {
			t->line++;
			t->col = 1;
		}
		t->i++;
		t->col++;
		goto start;
	} else {
		fprintf(stderr, "ERROR: Invalid character %c at %ld:%ld\n",
			ch, t->line, t->col);
		t->i++;
		t->col++;
		goto start;
	}
}
