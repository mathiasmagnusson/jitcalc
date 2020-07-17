#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/mman.h>

#include "codegen.h"

#define byte unsigned char

u64 len;
u64 cap;
unsigned char* code;

#define add(x) do { \
	if (len + 1 > cap) { \
		cap *= 2; \
		code = realloc(code, cap * sizeof code[0]); \
	} \
	code[len] = x; \
	len++; \
} while(0)

bool add_expr_int(Expression* expr) {
	switch (expr->kind) {
		case LiteralExpression:
			switch (expr->token.kind) {
				case LiteralToken:
					add(MOV_RAX_CONST_1);
					add(MOV_RAX_CONST_2);
					byte* value = (byte*)&expr->token.integer;
					add(value[0]);
					add(value[1]);
					add(value[2]);
					add(value[3]);
					add(value[4]);
					add(value[5]);
					add(value[6]);
					add(value[7]);
					break;
				default:
					fprintf(stderr, "ERROR: Invalid literal expression type ");
					print_token(stderr, expr->token);
					fprintf(stderr, "\n");
					return false;
			}
			break;
		case UnaryExpression:
			add_expr_int(expr->operand);
			switch (expr->token.kind) {
				case MinusToken:
					add(NEG_RAX_1);
					add(NEG_RAX_2);
					add(NEG_RAX_3);
					break;
				default:
					fprintf(stderr, "ERROR: Invalid unary expression type ");
					print_token(stderr, expr->token);
					fprintf(stderr, "\n");
					return false;
			}
			break;
		case BinaryExpression:
			if (!add_expr_int(expr->left)) return false;
			add(PUSH_RAX);
			if (!add_expr_int(expr->right)) return false;
			add(MOV_RCX_RAX_1);
			add(MOV_RCX_RAX_2);
			add(MOV_RCX_RAX_3);
			add(POP_RAX);
			switch (expr->token.kind) {
				case PlusToken:
					add(ADD_RAX_RCX_1);
					add(ADD_RAX_RCX_2);
					add(ADD_RAX_RCX_3);
					break;
				case MinusToken:
					add(SUB_RAX_RCX_1);
					add(SUB_RAX_RCX_2);
					add(SUB_RAX_RCX_3);
					break;
				case AsteriskToken:
					add(IMUL_RAX_RCX_1);
					add(IMUL_RAX_RCX_2);
					add(IMUL_RAX_RCX_3);
					add(IMUL_RAX_RCX_4);
					break;
				case SlashToken:
					add(CQO_1);
					add(CQO_2);
					add(IDIV_RCX_1);
					add(IDIV_RCX_2);
					add(IDIV_RCX_3);
					break;
				default:
					fprintf(stderr, "ERROR: Invalid binary expression type ");
					print_token(stderr, expr->token);
					fprintf(stderr, "\n");
					return false;
			}
			break;
		default:
			fprintf(stderr, "ERROR: Invalid expression\n");
			return false;
			break;
	}

	return true;
}

bool add_expr_float(Expression* expr) {
	switch (expr->kind) {
		case LiteralExpression:
			switch (expr->token.kind) {
				case LiteralToken:
					add(MOV_RAX_CONST_1);
					add(MOV_RAX_CONST_2);
					byte* value = (byte*)&expr->token.floating;
					add(value[0]);
					add(value[1]);
					add(value[2]);
					add(value[3]);
					add(value[4]);
					add(value[5]);
					add(value[6]);
					add(value[7]);
					add(MOVQ_XMM0_RAX_1);
					add(MOVQ_XMM0_RAX_2);
					add(MOVQ_XMM0_RAX_3);
					add(MOVQ_XMM0_RAX_4);
					add(MOVQ_XMM0_RAX_5);
					break;
				default:
					fprintf(stderr, "ERROR: Invalid literal expression type ");
					print_token(stderr, expr->token);
					fprintf(stderr, "\n");
					return false;
			}
			break;
		case UnaryExpression:
			add_expr_float(expr->operand);
			switch (expr->token.kind) {
				case MinusToken:
					add(MOVQ_XMM2_XMM0_1);
					add(MOVQ_XMM2_XMM0_2);
					add(MOVQ_XMM2_XMM0_3);
					add(MOVQ_XMM2_XMM0_4);
					add(XORPS_XMM0_XMM0_1);
					add(XORPS_XMM0_XMM0_2);
					add(XORPS_XMM0_XMM0_3);
					add(SUBSD_XMM0_XMM2_1);
					add(SUBSD_XMM0_XMM2_2);
					add(SUBSD_XMM0_XMM2_3);
					add(SUBSD_XMM0_XMM2_4);
					break;
				default:
					fprintf(stderr, "ERROR: Invalid unary expression type ");
					print_token(stderr, expr->token);
					fprintf(stderr, "\n");
					return false;
			}
			break;
		case BinaryExpression:
			if (!add_expr_float(expr->left)) return false;
			add(MOVQ_RAX_XMM0_1);
			add(MOVQ_RAX_XMM0_2);
			add(MOVQ_RAX_XMM0_3);
			add(MOVQ_RAX_XMM0_4);
			add(MOVQ_RAX_XMM0_5);
			add(PUSH_RAX);
			if (!add_expr_float(expr->right)) return false;
			add(MOVQ_XMM1_XMM0_1);
			add(MOVQ_XMM1_XMM0_2);
			add(MOVQ_XMM1_XMM0_3);
			add(MOVQ_XMM1_XMM0_4);

			add(POP_RAX);
			add(MOVQ_XMM0_RAX_1);
			add(MOVQ_XMM0_RAX_2);
			add(MOVQ_XMM0_RAX_3);
			add(MOVQ_XMM0_RAX_4);
			add(MOVQ_XMM0_RAX_5);

			switch (expr->token.kind) {
				case PlusToken:
					add(ADDSD_XMM0_XMM1_1);
					add(ADDSD_XMM0_XMM1_2);
					add(ADDSD_XMM0_XMM1_3);
					add(ADDSD_XMM0_XMM1_4);
					break;
				case MinusToken:
					add(SUBSD_XMM0_XMM1_1);
					add(SUBSD_XMM0_XMM1_2);
					add(SUBSD_XMM0_XMM1_3);
					add(SUBSD_XMM0_XMM1_4);
					break;
				case AsteriskToken:
					add(MULSD_XMM0_XMM1_1);
					add(MULSD_XMM0_XMM1_2);
					add(MULSD_XMM0_XMM1_3);
					add(MULSD_XMM0_XMM1_4);
					break;
				case SlashToken:
					add(DIVSD_XMM0_XMM1_1);
					add(DIVSD_XMM0_XMM1_2);
					add(DIVSD_XMM0_XMM1_3);
					add(DIVSD_XMM0_XMM1_4);
					break;
				default:
					fprintf(stderr, "ERROR: Invalid binary expression type ");
					print_token(stderr, expr->token);
					fprintf(stderr, "\n");
					return false;
			}
			break;
		default:
			fprintf(stderr, "ERROR: Invalid expression\n");
			return false;
			break;
	}

	return true;
}

CodeInt gen_code_int(Expression* expr) {
	len = 0;
	cap = 8;
	code = malloc(cap * sizeof *code);

	add(PUSH_RBP);
	add(MOV_RBP_RSP_1);
	add(MOV_RBP_RSP_2);
	add(MOV_RBP_RSP_3);

	if (!add_expr_int(expr)) return (CodeInt) { .len = 0 };

	add(POP_RBP);
	add(RET);

	s64(*executable)(void) = mmap(
		NULL, len * sizeof *code,
		PROT_EXEC | PROT_WRITE | PROT_READ,
		MAP_PRIVATE | MAP_ANONYMOUS,
		-1, 0
	);

	for (u64 i = 0; i < len; i++) {
		((unsigned char*)executable)[i] = code[i];
	}

	return (CodeInt) {
		.func = executable,
		.len = len,
	};
}

CodeFloat gen_code_float(Expression* expr) {
	len = 0;
	cap = 8;
	code = malloc(cap * sizeof *code);

	add(PUSH_RBP);
	add(MOV_RBP_RSP_1);
	add(MOV_RBP_RSP_2);
	add(MOV_RBP_RSP_3);

	if (!add_expr_float(expr)) return (CodeFloat) { .len = 0 };

	add(POP_RBP);
	add(RET);

	double(*executable)(void) = mmap(
		NULL, len * sizeof *code,
		PROT_EXEC | PROT_WRITE | PROT_READ,
		MAP_PRIVATE | MAP_ANONYMOUS,
		-1, 0
	);

	for (u64 i = 0; i < len; i++) {
		((unsigned char*)executable)[i] = code[i];
	}

	return (CodeFloat) {
		.func = executable,
		.len = len,
	};
}

void free_code_int(CodeInt code) {
	munmap(code.func, code.len);
}

void free_code_float(CodeFloat code) {
	munmap(code.func, code.len);
}
