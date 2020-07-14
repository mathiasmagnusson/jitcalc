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

bool add_expr(Expression* expr) {
	switch (expr->kind) {
		case LiteralExpression:
			switch (expr->token.kind) {
				case NumberToken:
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
			add_expr(expr->operand);
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
			if (!add_expr(expr->left)) return false;
			add(PUSH_RAX);
			if (!add_expr(expr->right)) return false;
			add(MOV_RBX_RAX_1);
			add(MOV_RBX_RAX_2);
			add(MOV_RBX_RAX_3);
			add(POP_RAX);
			switch (expr->token.kind) {
				case PlusToken:
					add(ADD_RAX_RBX_1);
					add(ADD_RAX_RBX_2);
					add(ADD_RAX_RBX_3);
					break;
				case MinusToken:
					add(SUB_RAX_RBX_1);
					add(SUB_RAX_RBX_2);
					add(SUB_RAX_RBX_3);
					break;
				case AsteriskToken:
					add(IMUL_RAX_RBX_1);
					add(IMUL_RAX_RBX_2);
					add(IMUL_RAX_RBX_3);
					add(IMUL_RAX_RBX_4);
					break;
				case SlashToken:
					add(CQO_1);
					add(CQO_2);
					add(IDIV_RBX_1);
					add(IDIV_RBX_2);
					add(IDIV_RBX_3);
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

Assembly gen_code(Expression* expr) {
	len = 0;
	cap = 8;
	code = malloc(cap * sizeof *code);

	add(PUSH_RBP);
	add(MOV_RBP_RSP_1);
	add(MOV_RBP_RSP_2);
	add(MOV_RBP_RSP_3);

	if (!add_expr(expr)) return (Assembly) {};

	add(POP_RBP);
	add(RET);

	Function executable = mmap(
		NULL, len * sizeof *code,
		PROT_EXEC | PROT_WRITE | PROT_READ,
		MAP_PRIVATE | MAP_ANONYMOUS,
		-1, 0
	);

	for (u64 i = 0; i < len; i++) {
		((unsigned char*)executable)[i] = code[i];
	}

	return (Assembly) {
		.f = executable,
		.len = len,
	};
}

void free_assembly(Assembly assembly) {
	munmap(assembly.f, assembly.len);
}
