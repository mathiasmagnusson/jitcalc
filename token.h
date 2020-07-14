#include "jitcalc.h"

typedef enum TokenKind {
	EOFToken = 0,
	NumberToken,
	LeftParenToken = '(',
	RightParenToken = ')',
	PlusToken = '+',
	MinusToken = '-',
	AsteriskToken = '*',
	SlashToken = '/',
} TokenKind;

typedef struct Token {
	TokenKind kind;
	u64 line;
	u64 col;
	u64 i;
	union {
		u64 integer;
	};
} Token;

typedef struct Tokenizer {
	u64 line;
	u64 col;
	u64 i;
	char* str;
	u64 len;
} Tokenizer;

void print_token(FILE*, Token token);
void putback_token(Tokenizer* t, Token token);
Tokenizer new_tokenizer(char* str);
Token next_token(Tokenizer* tokenizer);
