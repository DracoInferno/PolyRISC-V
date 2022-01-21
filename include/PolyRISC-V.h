#ifndef POLYRISC_V_H
#define POLYRISC_V_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>

// RV32I
#define BITS 32
//#define MEM_SIZE 0xFFFF // no need for more
//#define STACK_SIZE 0x7FFF
typedef int32_t reg_kt;
typedef uint32_t pc_kt;

// Register Aliases
//		ALIAS		REG			SAVER		DESCRIPTION
#define ZERO		0	//	-----		Hard-wired zero
#define RA			1	//	Caller		Return address
#define SP			2	//	-----		Stack pointer
#define GP			3	//	-----		Global pointer
#define TP			4	//	Caller		Thread pointer
#define T0			5	//	Caller		Temporary / alternate link register
#define T1			6	//	Caller		Temporary
#define T2			7	//	Caller		Temporary
#define S0			8	//	Callee		Saved register / frame pointer
#define FP			8	//	Callee		Frame pointer
#define S1			9	//	Callee		Saved register
#define A0			10	//	Caller		Function arguments / return values
#define A1			11	//	Caller		Function arguments
#define A2			12	//	Caller		Function arguments
#define A3			13	//	Caller		Function arguments
#define A4			14	//	Caller		Function arguments
#define A5			15	//	Caller		Function arguments
#define A6			16	//	Caller		Function arguments
#define A7			17	//	Caller		Function arguments
#define S2			18	//	Callee		Saved register
#define S3			19	//	Callee		Saved register
#define S4			20	//	Callee		Saved register
#define S5			21	//	Callee		Saved register
#define S6			22	//	Callee		Saved register
#define S7			23	//	Callee		Saved register
#define S8			24	//	Callee		Saved register
#define S9			25	//	Callee		Saved register
#define S10			26	//	Callee		Saved register
#define S11			27	//	Callee		Saved register
#define T3			28	//	Caller		Temporaries
#define T4			29	//	Caller		Temporaries
#define T5			30	//	Caller		Temporaries
#define T6			31	//	Caller		Temporaries

// Opcodes
#define OP_BASECODE		0x3
#define OP_LOAD			((0x00 << 2) | OP_BASECODE) 
	#define F3_LOAD_LB			0x0
	#define F3_LOAD_LH			0x1
	#define F3_LOAD_LW			0x2
	#define F3_LOAD_LBU			0x4
	#define F3_LOAD_LHU			0x5
#define OP_LOAD_FP		((0x01 << 2) | OP_BASECODE) 
#define OP_CUSTOM_0		((0x02 << 2) | OP_BASECODE) 
#define OP_MISC_MEM		((0x03 << 2) | OP_BASECODE) 
	#define F3_MISC_MEM_FENCE	0x0
#define OP_OP_IMM		((0x04 << 2) | OP_BASECODE) 
	#define F3_OP_IMM_ADDI		0x0
	#define F3_OP_IMM_SLTI		0x2
	#define F3_OP_IMM_SLTIU		0x3
	#define F3_OP_IMM_XORI		0x4
	#define F3_OP_IMM_ORI		0x6
	#define F3_OP_IMM_ANDI		0x7
	#define F3_OP_IMM_SLLI		0x1
	#define F3_OP_IMM_SRXI		0x5
		#define F7_OP_IMM_SRXI_SRLI		0x00
		#define F7_OP_IMM_SRXI_SRAI		0x20
#define OP_AUIPC		((0x05 << 2) | OP_BASECODE) 
#define OP_OP_IMM_32	((0x06 << 2) | OP_BASECODE) 

#define OP_STORE		((0x08 << 2) | OP_BASECODE) 
	#define F3_STORE_SB			0x0
	#define F3_STORE_SH			0x1
	#define F3_STORE_SW			0x2
#define OP_STORE_FP		((0x09 << 2) | OP_BASECODE) 
#define OP_CUSTOM_1		((0x0A << 2) | OP_BASECODE) 
#define OP_AMO			((0x0B << 2) | OP_BASECODE) 
#define OP_OP			((0x0C << 2) | OP_BASECODE) 
	#define	F3_OP_AS			0x0
		#define F7_OP_AS_ADD			0x00
		#define F7_OP_AS_SUB			0x20
	#define	F3_OP_SLL			0x1
	#define	F3_OP_SLT			0x2
	#define	F3_OP_SLTU			0x3
	#define	F3_OP_XOR			0x4
	#define	F3_OP_SRLA			0x5
		#define	F7_OP_SRLA_SRL			0x00
		#define	F7_OP_SRLA_SRA			0x20
	#define	F3_OP_OR			0x6
	#define	F3_OP_AND			0x7
#define OP_LUI			((0x0D << 2) | OP_BASECODE) 
#define OP_OP_32		((0x0E << 2) | OP_BASECODE) 

#define OP_MADD			((0x10 << 2) | OP_BASECODE) 
#define OP_MSUB			((0x11 << 2) | OP_BASECODE) 
#define OP_NMSUB		((0x12 << 2) | OP_BASECODE) 
#define OP_NMADD		((0x13 << 2) | OP_BASECODE) 
#define OP_OP_FP		((0x14 << 2) | OP_BASECODE) 
#define OP_RESERVED_0	((0x15 << 2) | OP_BASECODE) 
#define OP_CUSTOM_2		((0x16 << 2) | OP_BASECODE) 

#define OP_BRANCH		((0x18 << 2) | OP_BASECODE) 
	#define F3_BRANCH_BEQ		0x0
	#define F3_BRANCH_BNE		0x1
	#define F3_BRANCH_BLT		0x4
	#define F3_BRANCH_BGE		0x5
	#define F3_BRANCH_BLTU		0x6
	#define F3_BRANCH_BGEU		0x7
#define OP_JALR			((0x19 << 2) | OP_BASECODE) 
#define OP_RESERVED_1	((0x1A << 2) | OP_BASECODE) 
#define OP_JAL			((0x1B << 2) | OP_BASECODE) 
#define OP_SYSTEM		((0x1C << 2) | OP_BASECODE) 
	#define F3_SYSTEM_PRIV		0x0
			#define F12_SYSTEM_PRIV_ECALL	0x000
			#define F12_SYSTEM_PRIV_EBREAK	0x001
#define OP_RESERVED_2	((0x1D << 2) | OP_BASECODE) 
#define OP_CUSTOM_3		((0x1E << 2) | OP_BASECODE) 

typedef struct{
	reg_kt reg[32];
	pc_kt pc;
	uint8_t *mem;
	size_t mem_size;
	size_t stack_top;
	size_t stack_bot;
}RISCV_st;

typedef struct{
	size_t mem_size;
	size_t stack_size;
	bool set_to_0;
}RISCV_init_op_st;

typedef struct{
	int foo;
}RISCV_rst_op_st;


RISCV_st* RISCV_init(RISCV_init_op_st *options);
void RISCV_deinit(RISCV_st *cpu);
void RISCV_reset(RISCV_st *cpu, uint8_t *elf, size_t elf_size, 
				RISCV_rst_op_st *options);
void RISCV_step(RISCV_st *cpu);

void RISCV_print_reg(RISCV_st *cpu);
void RISCV_print_mem(RISCV_st *cpu, size_t start, size_t size);

uint32_t RISCV_fetch_instr(RISCV_st *cpu);

// Decoding instructions
// TODO: use const pointer to const instr instead ? to let compiler optimize things ? (to minimize cache miss ?)
//	Opcodes
uint8_t instr_decode_opcode(const uint32_t instr);
uint8_t instr_decode_funct3(const uint32_t instr);
uint8_t instr_decode_funct7(const uint32_t instr);
uint8_t instr_decode_funct12(const uint32_t instr);
//	Arguments fields
uint8_t instr_decode_rd(const uint32_t instr);
uint8_t instr_decode_rs1(const uint32_t instr);
uint8_t instr_decode_rs2(const uint32_t instr);
int16_t instr_decode_imm_11_0(const uint32_t instr);
uint16_t instr_decode_imm_11_0_u(const uint32_t instr);
int32_t instr_decode_imm_31_12(const uint32_t instr);
int32_t instr_decode_imm_jal(const uint32_t instr);
int16_t instr_decode_imm_branch(const uint32_t instr);
int16_t instr_decode_imm_store(const uint32_t instr);
uint8_t instr_decode_imm_shamt(const uint32_t instr);

// Instructions implementation
void RISCV_instr_lui(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_auipc(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_jal(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_jalr(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_beq(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_bne(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_blt(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_bge(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_bltu(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_bgeu(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_lb(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_lh(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_lw(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_lbu(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_lhu(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_sb(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_sh(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_sw(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_addi(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_slti(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_sltiu(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_xori(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_ori(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_andi(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_slli(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_srli(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_srai(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_add(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_sub(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_sll(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_slt(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_sltu(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_xor(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_srl(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_sra(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_or(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_and(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_fence(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_ecall(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_ebreak(RISCV_st *cpu, uint32_t instr);

#endif // POLYRISC_V_H
