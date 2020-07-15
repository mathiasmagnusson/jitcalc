#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "jitcalc.h"
#include "token.h"

void print_token_kind(FILE* file, TokenKind kind) {
	switch (kind) {
		case EOFToken:        fprintf(file, "<eof>");    break;
		case NumberToken:     fprintf(file, "<number>"); break;
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
		case NumberToken:
			fprintf(file, "%ld", token.integer);
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
start:
	if (t->i >= t->len) {
		return (Token) {
			.kind = EOFToken,
			.line = t->line,
			.col = t->col,
			.i = t->i,
		};
	}

	switch (t->str[t->i]) {
		case '(': case ')': case '+': case '-': case '*': case '/': {
			Token token = (Token) {
				.kind = t->str[t->i],
				.line = t->line,
				.col = t->col,
				.i = t->i,
			};
			t->i++;
			t->col++;
			return token;
		}
	}

	if (isdigit(t->str[t->i])) {
		u64 value = 0;

		u64 start_col = t->col;
		u64 start_i = t->i;

		while (t->i < t->len && isdigit(t->str[t->i])) {
			value *= 10;
			value += t->str[t->i] - '0';
			t->i++;
			t->col++;
		}

		Token token = (Token) {
			.kind = NumberToken,
			.line = t->line,
			.col = start_col,
			.i = start_i,

			.integer = value,
		};

		return token;
	} else if (isspace(t->str[t->i])) {
		if (t->str[t->i] == '\n') {
			t->line++;
			t->col = 1;
		}
		t->i++;
		t->col++;
		goto start;
	} else {
		fprintf(stderr, "ERROR: Invalid character %c at %ld:%ld\n",
			t->str[t->i], t->line, t->col);
		t->i++;
		t->col++;
		goto start;
	}
}
