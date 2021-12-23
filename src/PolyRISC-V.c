#include "PolyRISC-V.h"

RISCV_st* RISCV_init(RISCV_init_op_st *options)
{
	RISCV_st *cpu = NULL;

	assert(options);
	// 0 < stack_size <= mem_size < 2^BITS
	assert(options->stack_size > 0);
	assert(options->mem_size >= options->stack_size);
	assert(options->mem_size < pow(2, BITS));

	// Allocate RISCV_st object
	cpu = malloc(sizeof(RISCV_st));
	if(!cpu)
		return NULL;
	*cpu = {0}; // init all struct to 0
	cpu->mem_size = options->mem_size;

	// Allocate cpu memory
	cpu->mem = malloc(sizeof(uint8_t) * options->mem_size);
	if(!cpu->mem){
		RISCV_deinit(cpu);
		return NULL;
	}
	if(options->set_to_0){
		memset(cpu->mem, 0, cpu->mem_size);
	}
	cpu->stack_start = options->stack_size - 1;

	return cpu;
}

void RISCV_deinit(RISCV_st *cpu)
{
	if(!cpu)
		return;
	if(!cpu->mem)
		free(cpu->mem);
	free(cpu);
}

void RISCV_reset(RISCV_st *cpu, word_kt *instr_list, size_t list_size 
				RISCV_rst_op_st *options)
{
	assert(cpu);
	assert(instr_list);
	assert(list_size > 0);

	// Set registers to 0
	memset(cpu->reg, 0, sizeof(cpu->reg));

	// Set stack pointer register
	cpu->SP = cpu->stack_start;

}

void RISCV_step(RISCV_st *cpu)
{
	word_kt instr = 0;
	uint8_t opcode = 0;
	uint8_t funct3 = 0;
	uint8_t funct7 = 0;

	// Fetch instruction
	instr = cpu->instr[cpu->pc++];

	// Decode instruction
	opcode = OPCODE(instr);
	funct3 = FUNCT3(instr);
	funct7 = FUNCT7(instr);
	
		// 3 possible methods
		//	-> switch case
		//	-> callback lookup table
		//	-> computed goto
		// Go for switch case because opcodes aren't continuous
	switch(opcode){

	}

	// Do operation
}


static RISCV_instr_st RISCV_fetch_instr(RISCV_st *cpu)
{
	
}
