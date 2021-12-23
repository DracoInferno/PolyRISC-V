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
typedef uint32_t word_kt;

// Register Aliases
//		ALIAS		REG			SAVER		DESCRIPTION
#define ZERO	(reg[0])	//	-----		Hard-wired zero
#define RA		(reg[1])	//	Caller		Stack pointer
#define SP		(reg[2])	//	-----		Global pointer
#define GP		(reg[3])	//	-----		Thread pointer
#define TP		(reg[4])	//	Caller		Temporary / alternate link register
#define T0		(reg[5])	//	Caller		Temporary
#define T1		(reg[6])	//	Caller		Temporary
#define T2		(reg[7])	//	Caller		Temporary
#define S0		(reg[8])	//	Callee		Saved register / frame pointer
#define FP		(reg[8])
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
#define OPCODE(instr) (instr & 0x7F) // 7 first bits
// In base set, first two bits are 0b11 so:
#define BOPCODE(instr) ((instr >> 2) & 0x7) // bits n°2 to n°6 (5b)
#define FUNCT3(instr) ((instr >> 12) & 0x7) // bits n°12 to 14 (3b)
#define FUNCT7(instr) ((instr >> 25)) // bits n°25 to 31 (7b)

// Opcodes
#define OP_BASECODE		0x3
#define OP_LOAD			((0x00 << 2) | BASECODE) 
#define OP_LOAD_FP		((0x01 << 2) | BASECODE) 
#define OP_CUSTOM_0		((0x02 << 2) | BASECODE) 
#define OP_MISC_MEM		((0x03 << 2) | BASECODE) 
#define OP_OP_IMM		((0x04 << 2) | BASECODE) 
#define OP_AUIPC		((0x05 << 2) | BASECODE) 
#define OP_OP_IMM_32	((0x06 << 2) | BASECODE) 

#define OP_STORE		((0x08 << 2) | BASECODE) 
#define OP_STORE_FP		((0x09 << 2) | BASECODE) 

typedef struct{
	// Set at init
	uint8_t *mem;
	size_t mem_size;
	size_t stack_start;

	// Set at reset
	word_kt reg[BITS];
	word_kt pc;
	word_kt *instr;
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
static void RISCV_fetch_instr(RISCV_st *cpu);

#endif // POLYRISC_V_H
