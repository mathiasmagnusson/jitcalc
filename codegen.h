#include "expr.h"

typedef u64 (*Function)(void);

typedef struct Assembly {
	Function f;
	s64 len;
} Assembly;

void free_assembly(Assembly assembly);

Assembly gen_code(Expression* expr);

#define PUSH_RBP         0x55
#define POP_RBP          0x5d
#define RET              0xc3
#define MOV_EAX_CONST    0xb8
#define MOV_REG_REG      0x48
#define REG_RBP          0x89
#define REG_RSP          0xe5
#define MOV_RAX_CONST_1  0x48
#define MOV_RAX_CONST_2  0xb8
#define PUSH_RAX         0x50
#define POP_RAX          0x58
#define POP_RBX          0x5b
#define NEG_RAX_1        0x48
#define NEG_RAX_2        0xf7
#define NEG_RAX_3        0xd8
#define ADD_RAX_RBX_1    0x48
#define ADD_RAX_RBX_2    0x01
#define ADD_RAX_RBX_3    0xd8
#define SUB_RAX_RBX_1    0x48
#define SUB_RAX_RBX_2    0x28
#define SUB_RAX_RBX_3    0xd8
#define IMUL_RAX_RBX_1   0x48
#define IMUL_RAX_RBX_2   0x0f
#define IMUL_RAX_RBX_3   0xaf
#define IMUL_RAX_RBX_4   0xc3
#define IDIV_RBX_1       0x48
#define IDIV_RBX_2       0xf7
#define IDIV_RBX_3       0xfb
#define XOR_EDX_EDX_1    0x31
#define XOR_EDX_EDX_2    0xd2
#define CQO_1            0x48
#define CQO_2            0x99

// add  rax, rbx  =>  48 01 d8
// sub  rax, rbx  =>  48 28 d8
// imul rax, rbx  =>  48 0f af c3
// idiv      rbx  =>  48 f7 fb
