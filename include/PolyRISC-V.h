#ifndef POLYRISC_V_H
#define POLYRISC_V_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <math.h>

// RV32I
#define BITS 32
//#define MEM_SIZE 0xFFFF // no need for more
//#define STACK_SIZE 0x7FFF
typedef int8_t byte_kt;
typedef uint8_t ubyte_kt;
typedef int16_t half_kt;
typedef uint16_t uhalf_kt;
typedef int32_t word_kt;
typedef uint32_t uword_kt;
typedef int32_t reg_kt;

// Register Aliases
//		ALIAS		REG			SAVER		DESCRIPTION
#define ZERO	(reg[0])	//	-----		Hard-wired zero
#define RA		(reg[1])	//	Caller		Return address
#define SP		(reg[2])	//	-----		Stack pointer
#define GP		(reg[3])	//	-----		Global pointer
#define TP		(reg[4])	//	Caller		Thread pointer
#define T0		(reg[5])	//	Caller		Temporary / alternate link register
#define T1		(reg[6])	//	Caller		Temporary
#define T2		(reg[7])	//	Caller		Temporary
#define S0		(reg[8])	//	Callee		Saved register / frame pointer
#define FP		(reg[8])	//	Callee		Frame pointer
#define S1		(reg[9])	//	Callee		Saved register
#define A0		(reg[10])	//	Caller		Function arguments / return values
#define A1		(reg[11])	//	Caller		Function arguments
#define A2		(reg[12])	//	Caller		Function arguments
#define A3		(reg[13])	//	Caller		Function arguments
#define A4		(reg[14])	//	Caller		Function arguments
#define A5		(reg[15])	//	Caller		Function arguments
#define A6		(reg[16])	//	Caller		Function arguments
#define A7		(reg[17])	//	Caller		Function arguments
#define S2		(reg[18])	//	Callee		Saved register
#define S3		(reg[19])	//	Callee		Saved register
#define S4		(reg[20])	//	Callee		Saved register
#define S5		(reg[21])	//	Callee		Saved register
#define S6		(reg[22])	//	Callee		Saved register
#define S7		(reg[23])	//	Callee		Saved register
#define S8		(reg[24])	//	Callee		Saved register
#define S9		(reg[25])	//	Callee		Saved register
#define S10		(reg[26])	//	Callee		Saved register
#define S11		(reg[27])	//	Callee		Saved register
#define T3		(reg[28])	//	Caller		Temporaries
#define T4		(reg[29])	//	Caller		Temporaries
#define T5		(reg[30])	//	Caller		Temporaries
#define T6		(reg[31])	//	Caller		Temporaries

// Instruction decoding
#define OPCODE(instr)	(instr & 0x7F) // 7 first bits
// In base set, first two bits are 0b11 so:
#define BOPCODE(instr)	((instr >> 2) & 0x7) // bits n°2 to n°6 (5b)
#define FUNCT3(instr)	((instr >> 12) & 0x7) // bits n°12 to 14 (3b)
#define FUNCT7(instr)	((instr >> 25)) // bits n°25 to 31 (7b)
#define FUNCT12(instr)	((instr >> 20)) // bits n°20 to 31 (7b)

// Opcodes
#define OP_BASECODE		0x3
#define OP_LOAD			((0x00 << 2) | BASECODE) 
	#define F3_LOAD_LB			0x0
	#define F3_LOAD_LH			0x1
	#define F3_LOAD_LW			0x2
	#define F3_LOAD_LBU			0x4
	#define F3_LOAD_LHU			0x5
#define OP_LOAD_FP		((0x01 << 2) | BASECODE) 
#define OP_CUSTOM_0		((0x02 << 2) | BASECODE) 
#define OP_MISC_MEM		((0x03 << 2) | BASECODE) 
	#define F3_MISC_MEM_FENCE	0x0
#define OP_OP_IMM		((0x04 << 2) | BASECODE) 
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
#define OP_AUIPC		((0x05 << 2) | BASECODE) 
#define OP_OP_IMM_32	((0x06 << 2) | BASECODE) 

#define OP_STORE		((0x08 << 2) | BASECODE) 
	#define F3_STORE_SB			0x0
	#define F3_STORE_SH			0x1
	#define F3_STORE_SW			0x2
#define OP_STORE_FP		((0x09 << 2) | BASECODE) 
#define OP_CUSTOM_1		((0x0A << 2) | BASECODE) 
#define OP_AMO			((0x0B << 2) | BASECODE) 
#define OP_OP			((0x0C << 2) | BASECODE) 
	#define	F3_OP_AS			0x0
		#define F7_OP_AS_ADD			0x00
		#define F7_OP_AS_SUB			0x20
	#define	F3_OP_SLL			0x1
	#define	F3_OP_SLT			0x2
	#define	F3_OP_SLTU			0x3
	#define	F3_OP_XOR			0x4
	#define	F3_OP_SRLA			0x5
		#define	F3_OP_SRLA_SRL			0x00
		#define	F3_OP_SRLA_SRA			0x20
	#define	F3_OP_OR			0x6
	#define	F3_OP_AND			0x7
#define OP_LUI			((0x0D << 2) | BASECODE) 
#define OP_OP_32		((0x0E << 2) | BASECODE) 

#define OP_MADD			((0x10 << 2) | BASECODE) 
#define OP_MSUB			((0x11 << 2) | BASECODE) 
#define OP_NMSUB		((0x12 << 2) | BASECODE) 
#define OP_NMADD		((0x13 << 2) | BASECODE) 
#define OP_OP_FP		((0x14 << 2) | BASECODE) 
#define OP_RESERVED_0	((0x15 << 2) | BASECODE) 
#define OP_CUSTOM_2		((0x16 << 2) | BASECODE) 

#define OP_BRANCH		((0x18 << 2) | BASECODE) 
	#define F3_BRANCH_BEQ		0x0
	#define F3_BRANCH_BNE		0x1
	#define F3_BRANCH_BLT		0x4
	#define F3_BRANCH_BGE		0x5
	#define F3_BRANCH_BLTU		0x6
	#define F3_BRANCH_BGEU		0x7
#define OP_JALR			((0x19 << 2) | BASECODE) 
#define OP_RESERVED_1	((0x1A << 2) | BASECODE) 
#define OP_JAL			((0x1B << 2) | BASECODE) 
#define OP_SYSTEM		((0x1C << 2) | BASECODE) 
	#define F3_SYSTEM_PRIV		0x0
			#define F12_SYSTEM_PRIV_ECALL	0x000
			#define F12_SYSTEM_PRIV_EBREAK	0x001
#define OP_RESERVED_2	((0x1D << 2) | BASECODE) 
#define OP_CUSTOM_3		((0x1E << 2) | BASECODE) 

#define DECODE_RD(instr)			((instr >> 7) & 0x3F)
#define DECODE_RS1(instr)			((instr >> 15) & 0x1F)
#define DECODE_IMM_11_0(instr)		(instr >> 20) 
#define DECODE_IMMI_11_0(instr)		((instr & 0x800) ? ((instr >> 20) | 0xFFFFF000) : (instr >> 20))
#define DECODE_IMM_JAL(instr)			\
	(((instr >> 20) & 0x100000)		|	\ 
	 ((instr >> 1)	& 0x0FFC00)		|	\ 
	 ((instr >> 2)	& 0x000200)		|	\ 
	 ((instr >> 11) & 0x0001FE))
	// 1 .... .... .... .... ....
	// . 1111 1111 11.. .... ....
	// . .... .... ..1. .... ....
	// . .... .... ...1 1111 1110
#define DECODE_IMM_BRANCH(instr)	(((instr >> 12) & 0x1) | ((instr >> 5) & 0x3F) | ((instr >> 1) & 0xF) | ((instr >> 11) & 0x1))

typedef struct{
	// Set at init
	ubyte_kt *mem;
	size_t mem_size;
	size_t stack_start;

	// Set at reset
	reg_kt reg[BITS];
	reg_kt pc;
	int32_t *instr;
	size_t instr_size;
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
void RISCV_reset(RISCV_st *cpu, word_kt *instr_list, size_t list_size 
				RISCV_rst_op_st *options);
void RISCV_step(RISCV_st *cpu);

// Decoding instructions
// TODO: use pointer to const instr instead ? to minimize cache miss ?
//	Opcodes
static uint8_t instr_decode_opcode(uword_kt instr);
static uint8_t instr_decode_funct3(uword_kt instr);
static uint8_t instr_decode_funct7(uword_kt instr);
static uint8_t instr_decode_funct12(uword_kt instr);
//	Arguments fields
static uint8_t instr_decode_rd(uword_kt instr);
static uint8_t instr_decode_rs1(uword_kt instr);
static uint8_t instr_decode_rs2(uword_kt instr);
static int16_t instr_decode_imm_11_0(uword_kt instr);
static uint16_t instr_decode_imm_11_0_u(uword_kt instr);
static int32_t instr_decode_imm_31_12(uword_kt instr);
static int32_t instr_decode_imm_jal(uword_kt instr);
static int16_t instr_decode_imm_branch(uword_kt instr);
static int16_t instr_decode_imm_store(uword_kt instr);

// Instructions implementation
static void RISCV_instr_lui(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_auipc(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_jal(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_jalr(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_beq(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_bne(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_blt(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_bge(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_bltu(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_bgeu(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_lb(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_lh(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_lw(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_lbu(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_lhu(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_sb(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_sh(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_sw(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_addi(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_slti(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_sltiu(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_xori(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_ori(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_andi(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_slli(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_srli(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_srai(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_add(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_sub(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_sll(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_slt(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_sltu(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_xor(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_srl(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_sra(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_or(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_and(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_fence(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_ecall(RISCV_st *cpu, word_kt instr);
static void RISCV_instr_ebreak(RISCV_st *cpu, word_kt instr);

#endif // POLYRISC_V_H
