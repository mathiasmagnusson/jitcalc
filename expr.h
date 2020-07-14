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

Expression* parse_expr(Tokenizer* tokens, u64 precedence_level);
void print_expr(FILE* file, Expression* expr, u64 indent);
void free_expr(Expression* expr);
