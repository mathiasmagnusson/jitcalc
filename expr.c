#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "jitcalc.h"
#include "expr.h"

u64 get_unary_precedende(TokenKind token_kind) {
	switch (token_kind) {
		case MinusToken:    return 3;
		default:            return 0;
	}
}

u64 get_binary_precedende(TokenKind token_kind) {
	switch (token_kind) {
		case AsteriskToken: return 2;
		case SlashToken:    return 2;

		case PlusToken:     return 1;
		case MinusToken:    return 1;

		default:            return 0;
	}
}

ParseResult _parse_expr(Tokenizer* t, u64 precedence_level) {
	Expression* expr = NULL;

	Token token = next_token(t);
	if (token.kind == NumberToken) {
		expr = malloc(sizeof *expr);
		*expr = (Expression) {
			.kind = LiteralExpression,
			.token = token,
		};
	} else if (token.kind == LeftParenToken) {
		ParseResult res = parse_expr(t, 0);
		if (!res.success) return res;
		expr = res.expr;
		Token right_paren = next_token(t);
		if (right_paren.kind != RightParenToken) {
			free_expr(expr);
			return (ParseResult) {
				.success = false,
				.error = (ParseError) {
					.token = right_paren,
					.expected_specific = true,
					.expected = RightParenToken,
				},
			};
		}
	} else if (get_unary_precedende(token.kind)) {
		ParseResult res = parse_expr(t, get_unary_precedende(token.kind));
		if (!res.success) return res;
		Expression* operand = res.expr;
		expr = malloc(sizeof *expr);
		*expr = (Expression) {
			.kind = UnaryExpression,
			.token = token,
			.operand = operand,
		};
	} else {
		return (ParseResult) {
			.success = false,
			.error = (ParseError) {
				.token = token,
				.expected_specific = false,
			},
		};
	}

	while (true) {
		Token bin_op = next_token(t);
		u64 prec = get_binary_precedende(bin_op.kind);
		if (prec == 0 || prec <= precedence_level) {
			putback_token(t, bin_op);
			break;
		}

		Expression* left = expr;
		ParseResult res = parse_expr(t, prec);
		if (!res.success) {
			free_expr(left);
			return res;
		}
		Expression* right = res.expr;
		expr = malloc(sizeof *expr);
		*expr = (Expression) {
			.kind = BinaryExpression,
			.token = bin_op,
			.left = left,
			.right = right,
		};
	}

	return (ParseResult) {
		.success = true,
		.expr = expr,
	};
}

ParseResult parse_expr(Tokenizer* t, u64 precedence_level) {
	ParseResult res = _parse_expr(t, precedence_level);
	Token eof = next_token(t);
	if (eof.kind != EOFToken) {
		if (res.success) free_expr(res.expr);
		return (ParseResult) {
			.success = false,
			.error = (ParseError) {
				.token = eof,
				.expected_specific = true,
				.expected = EOFToken,
			}
		};
	}
	return res;
}

void print_expr(FILE* file, Expression* expr, u64 indent) {
	for (u64 i = 0; i < indent; i++)
		fprintf(file, "    ");
	switch (expr->kind) {
		case LiteralExpression:
			fprintf(file, "Literal ");
			print_token(file, expr->token);
			fprintf(file, "\n");
			break;
		case UnaryExpression:
			fprintf(file, "Unary ");
			print_token(file, expr->token);
			fprintf(file, "\n");
			print_expr(file, expr->operand, indent + 1);
			break;
		case BinaryExpression:
			fprintf(file, "Binary ");
			print_token(file, expr->token);
			fprintf(file, "\n");
			print_expr(file, expr->left, indent + 1);
			print_expr(file, expr->right, indent + 1);
			break;
	}
}

void free_expr(Expression* expr) {
	switch (expr->kind) {
		case LiteralExpression:
			break;
		case UnaryExpression:
			free_expr(expr->operand);
			break;
		case BinaryExpression:
			free_expr(expr->left);
			free_expr(expr->right);
			break;
	}
	free(expr);
}
