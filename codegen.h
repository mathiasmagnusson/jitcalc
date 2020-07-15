#include "expr.h"

typedef struct AssemblyInt {
	s64 (*func)(void);
	u64 len;
} AssemblyInt;

typedef struct AssemblyFloat {
	double (*func)(void);
	u64 len;
} AssemblyFloat;

void free_assembly_int(AssemblyInt assembly);
void free_assembly_float(AssemblyFloat assembly);

AssemblyInt gen_code_int(Expression* expr);
AssemblyFloat gen_code_float(Expression* expr);

#define PUSH_RBP          0x55

#define POP_RBP           0x5d

#define RET               0xc3

#define MOV_EAX_CONST     0xb8

#define MOV_RBP_RSP_1     0x48
#define MOV_RBP_RSP_2     0x89
#define MOV_RBP_RSP_3     0xe5

#define MOV_RAX_CONST_1   0x48
#define MOV_RAX_CONST_2   0xb8

#define PUSH_RAX          0x50

#define POP_RAX           0x58

#define POP_RBX           0x5b

#define NEG_RAX_1         0x48
#define NEG_RAX_2         0xf7
#define NEG_RAX_3         0xd8

#define ADD_RAX_RBX_1     0x48
#define ADD_RAX_RBX_2     0x01
#define ADD_RAX_RBX_3     0xd8

#define SUB_RAX_RBX_1     0x48
#define SUB_RAX_RBX_2     0x29
#define SUB_RAX_RBX_3     0xd8

#define IMUL_RAX_RBX_1    0x48
#define IMUL_RAX_RBX_2    0x0f
#define IMUL_RAX_RBX_3    0xaf
#define IMUL_RAX_RBX_4    0xc3

#define IDIV_RBX_1        0x48
#define IDIV_RBX_2        0xf7
#define IDIV_RBX_3        0xfb

#define XOR_EDX_EDX_1     0x31
#define XOR_EDX_EDX_2     0xd2

#define CQO_1             0x48
#define CQO_2             0x99

#define MOV_RBX_RAX_1     0x48
#define MOV_RBX_RAX_2     0x89
#define MOV_RBX_RAX_3     0xc3

#define MOVQ_XMM0_RAX_1   0x66
#define MOVQ_XMM0_RAX_2   0x48
#define MOVQ_XMM0_RAX_3   0x0f
#define MOVQ_XMM0_RAX_4   0x6e
#define MOVQ_XMM0_RAX_5   0xc0

#define MOVQ_RAX_XMM0_1   0x66
#define MOVQ_RAX_XMM0_2   0x48
#define MOVQ_RAX_XMM0_3   0x0f
#define MOVQ_RAX_XMM0_4   0x7e
#define MOVQ_RAX_XMM0_5   0xc0

#define MOVQ_XMM1_XMM0_1  0xf3
#define MOVQ_XMM1_XMM0_2  0x0f
#define MOVQ_XMM1_XMM0_3  0x7e
#define MOVQ_XMM1_XMM0_4  0xc8

#define ADDSD_XMM0_XMM1_1 0xf2
#define ADDSD_XMM0_XMM1_2 0x0f
#define ADDSD_XMM0_XMM1_3 0x58
#define ADDSD_XMM0_XMM1_4 0xc1

#define SUBSD_XMM0_XMM1_1 0xf2
#define SUBSD_XMM0_XMM1_2 0x0f
#define SUBSD_XMM0_XMM1_3 0x5c
#define SUBSD_XMM0_XMM1_4 0xc1

#define MULSD_XMM0_XMM1_1 0xf2
#define MULSD_XMM0_XMM1_2 0x0f
#define MULSD_XMM0_XMM1_3 0x59
#define MULSD_XMM0_XMM1_4 0xc1

#define DIVSD_XMM0_XMM1_1 0xf2
#define DIVSD_XMM0_XMM1_2 0x0f
#define DIVSD_XMM0_XMM1_3 0x5e
#define DIVSD_XMM0_XMM1_4 0xc1

#define XORPS_XMM0_XMM0_1 0x0f
#define XORPS_XMM0_XMM0_2 0x57
#define XORPS_XMM0_XMM0_3 0xc0

#define SUBSD_XMM0_XMM2_1 0xf2
#define SUBSD_XMM0_XMM2_2 0x0f
#define SUBSD_XMM0_XMM2_3 0x5c
#define SUBSD_XMM0_XMM2_4 0xc2

#define MOVQ_XMM2_XMM0_1  0xf3
#define MOVQ_XMM2_XMM0_2  0x0f
#define MOVQ_XMM2_XMM0_3  0x7e
#define MOVQ_XMM2_XMM0_4  0xd0
