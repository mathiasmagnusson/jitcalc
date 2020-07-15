#include <stdio.h>

#include "token.h"

typedef enum ExpressionKind {
	LiteralExpression,
	BinaryExpression,
	UnaryExpression,
} ExpressionKind;

typedef struct Expression Expression;
struct Expression {
	ExpressionKind kind;
	Token token;
	union {
		Expression* operand;
		struct {
			Expression* left;
			Expression* right;
		};
	};
};

typedef struct ParseError {
	Token token;
	bool expected_specific;
	TokenKind expected;
} ParseError;

typedef struct ParseResult {
	bool success;
	union {
		Expression* expr;
		ParseError error;
	};
} ParseResult;

ParseResult parse_expr(Tokenizer* tokens, u64 precedence_level);
void print_expr(FILE* file, Expression* expr, u64 indent);
void free_expr(Expression* expr);
