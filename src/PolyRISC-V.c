#include "PolyRISC-V.h"

const char REG_NAMES[32][6] = {
	"zero", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
	"s0", "s1","a0", "a1", "a2", "a3", "a4", "a5",
	"a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
	"s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

const char REG_NAMES_TAB[32][6] = {
	"zero", "ra\t", "sp\t", "gp\t", "tp\t", "t0\t", "t1\t", "t2\t",
	"s0/fp", "s1\t","a0\t", "a1\t", "a2\t", "a3\t", "a4\t", "a5\t",
	"a6\t", "a7\t", "s2\t", "s3\t", "s4\t", "s5\t", "s6\t", "s7\t",
	"s8\t", "s9\t", "s10", "s11", "t3\t", "t4\t", "t5\t", "t6\t"
};

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
	memset(cpu, 0, sizeof(RISCV_st)); // init all struct to 0
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
	cpu->stack_top = cpu->mem_size - 1;

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

void RISCV_load_raw_program(RISCV_st *cpu, const uint8_t *elf, size_t elf_size)
{
	assert(cpu);
	assert(elf);
	assert(elf_size > 0);
	assert(elf_size < cpu->mem_size);

	// Set stack lower boundary
	cpu->stack_bot = elf_size;

	// Copy program to memory
	memcpy(cpu->mem, elf, elf_size);
}

void RISCV_reset(RISCV_st *cpu)
{
	assert(cpu);

	DEBUG_PRINT("%s", "Program reset.\n");

	// Set registers to 0
	memset(cpu->reg, 0, sizeof(cpu->reg));

	// Set stack pointer register
	cpu->reg[SP] = cpu->stack_top;
	
	// Set program counter
	cpu->pc = 0;
}

void RISCV_step(RISCV_st *cpu)
{
	uint32_t instr = 0;
	uint8_t opcode = 0;
	uint8_t funct3 = 0;
	uint8_t funct7 = 0;
	uint8_t funct12 = 0;

	assert(cpu);
	assert(cpu->mem);

	// Fetch instruction
	instr = RISCV_fetch_instr(cpu);

	DEBUG_PRINT("Executing instruction Ox%08x at pc: 0x%08x.\n", instr, cpu->pc);

	// Decode instruction
	opcode = instr_decode_opcode(instr);
		// funct3 = instr_decode_funct3(instr);
		// funct7 = instr_decode_funct7(instr);
		// funct12 = instr_decode_funct12(instr);
	
	// 3 possible methods
	//	-> switch case
	//	-> callback lookup table
	//	-> computed goto (gcc only)
	// Go for switch case because opcodes aren't continuous
	switch(opcode){
		case OP_LUI:{
			RISCV_instr_lui(cpu, instr);
		}break;

		case OP_AUIPC:{
			RISCV_instr_auipc(cpu, instr);
		}break;

		case OP_JAL:{
			RISCV_instr_jal(cpu, instr);
		}break;

		case OP_JALR:{
			RISCV_instr_jalr(cpu, instr);
		}break;

		case OP_BRANCH:{
			funct3 = instr_decode_funct3(instr);
			switch(funct3){
				case F3_BRANCH_BEQ:{
					RISCV_instr_beq(cpu, instr);
				}break;

				case F3_BRANCH_BNE:{
					RISCV_instr_bne(cpu, instr);
				}break;

				case F3_BRANCH_BLT:{
					RISCV_instr_blt(cpu, instr);
				}break;

				case F3_BRANCH_BGE:{
					RISCV_instr_bge(cpu, instr);
				}break;

				case F3_BRANCH_BLTU:{
					RISCV_instr_bltu(cpu, instr);
				}break;

				case F3_BRANCH_BGEU:{
					RISCV_instr_bgeu(cpu, instr);
				}break;

				default:{
					fprintf(stderr,
							"Error, bad funct3 code: 0x%08x (opcode: 0x%08x).\n",
							funct3, opcode
							);
					assert(0 && "Invalid funct3 code!");
				}
			}
		}break;

		case OP_LOAD:{
			funct3 = instr_decode_funct3(instr);
			switch(funct3){
				case F3_LOAD_LB:{
					RISCV_instr_lb(cpu, instr);
				}break;

				case F3_LOAD_LH:{
					RISCV_instr_lh(cpu, instr);
				}break;

				case F3_LOAD_LW:{
					RISCV_instr_lw(cpu, instr);
				}break;

				case F3_LOAD_LBU:{
					RISCV_instr_lbu(cpu, instr);
				}break;

				case F3_LOAD_LHU:{
					RISCV_instr_lhu(cpu, instr);
				}break;

				default:{
					fprintf(stderr,
							"Error, bad funct3 code: 0x%08x (opcode: 0x%08x).\n",
							funct3, opcode
							);
					assert(0 && "Invalid funct3 code!");
				}
			}
		}break;

		case OP_STORE:{
			funct3 = instr_decode_funct3(instr);
			switch(funct3){
				case F3_STORE_SB:{
					RISCV_instr_sb(cpu, instr);
				}break;

				case F3_STORE_SH:{
					RISCV_instr_sh(cpu, instr);
				}break;

				case F3_STORE_SW:{
					RISCV_instr_sw(cpu, instr);
				}break;

				default:{
					fprintf(stderr,
							"Error, bad funct3 code: 0x%08x (opcode: 0x%08x).\n",
							funct3, opcode
							);
					assert(0 && "Invalid funct3 code!");
				}
			}
		}break;

		case OP_OP_IMM:{
			funct3 = instr_decode_funct3(instr);
			switch(funct3){
				case F3_OP_IMM_ADDI:{
					RISCV_instr_addi(cpu, instr);
				}break;

				case F3_OP_IMM_SLTI:{
					RISCV_instr_slti(cpu, instr);
				}break;

				case F3_OP_IMM_SLTIU:{
					RISCV_instr_sltiu(cpu, instr);
				}break;

				case F3_OP_IMM_XORI:{
					RISCV_instr_xori(cpu, instr);
				}break;

				case F3_OP_IMM_ORI:{
					RISCV_instr_ori(cpu, instr);
				}break;

				case F3_OP_IMM_ANDI:{
					RISCV_instr_andi(cpu, instr);
				}break;

				case F3_OP_IMM_SRXI:{
					funct7 = instr_decode_funct7(instr);
					switch(funct7){
						case F7_OP_IMM_SRXI_SRLI:{
							RISCV_instr_srli(cpu, instr);
						}break;

						case F7_OP_IMM_SRXI_SRAI:{
							RISCV_instr_srai(cpu, instr);
						}break;

						default:{
							fprintf(stderr,
									"Error, bad funct7 code: 0x%08x (opcode: 0x%08x, funct3: 0x%08x).\n",
									funct7, opcode, funct3
							);
							assert(0 && "Invalid funct7 code!");
						}
					}
				}break;

				default:{
					fprintf(stderr,
							"Error, bad funct3 code: 0x%08x (opcode: 0x%08x).\n",
							funct3, opcode
							);
					assert(0 && "Invalid funct3 code!");
				}
			}
		}break;

		case OP_OP:{
			funct3 = instr_decode_funct3(instr);
			switch(funct3){
				case F3_OP_AS:{
					funct7 = instr_decode_funct7(instr);
					switch(funct7){
						case F7_OP_AS_ADD:{
							RISCV_instr_add(cpu, instr);
						}break;

						case F7_OP_AS_SUB:{
							RISCV_instr_sub(cpu, instr);
						}break;

						default:{
							fprintf(stderr,
									"Error, bad funct7 code: 0x%08x (opcode: 0x%08x, funct3: 0x%08x).\n",
									funct7, opcode, funct3
							);
							assert(0 && "Invalid funct7 code!");
						}
					}
				}break;

				case F3_OP_SLL:{
					RISCV_instr_sll(cpu, instr);
				}break;

				case F3_OP_SLT:{
					RISCV_instr_slt(cpu, instr);
				}break;

				case F3_OP_SLTU:{
					RISCV_instr_sltu(cpu, instr);
				}break;

				case F3_OP_XOR:{
					RISCV_instr_xor(cpu, instr);
				}break;

				case F3_OP_SRLA:{
					funct7 = instr_decode_funct7(instr);
					switch(funct7){
						case F7_OP_SRLA_SRL:{
							RISCV_instr_srl(cpu, instr);
						}break;

						case F7_OP_SRLA_SRA:{
							RISCV_instr_sra(cpu, instr);
						}break;

						default:{
							fprintf(stderr,
									"Error, bad funct7 code: 0x%08x (opcode: 0x%08x, funct3: 0x%08x).\n",
									funct7, opcode, funct3
							);
							assert(0 && "Invalid funct7 code!");
						}
					}
				}break;

				case F3_OP_OR:{
					RISCV_instr_or(cpu, instr);
				}break;

				case F3_OP_AND:{
					RISCV_instr_and(cpu, instr);
				}break;

				default:{
					fprintf(stderr,
							"Error, bad funct3 code: 0x%08x (opcode: 0x%08x).\n",
							funct3, opcode
							);
					assert(0 && "Invalid funct3 code!");
				}
			}
		}break;

		case OP_MISC_MEM:{
			funct3 = instr_decode_funct3(instr);
			switch(funct3){
				case F3_MISC_MEM_FENCE:{
					assert(0 && "FENCE Not implemented");
					RISCV_instr_fence(cpu, instr);
				}break;

				default:{
					fprintf(stderr,
							"Error, bad funct3 code: 0x%08x (opcode: 0x%08x).\n",
							funct3, opcode
							);
					assert(0 && "Invalid funct3 code!");
				}
			}
		}
		case OP_SYSTEM:{
			funct3 = instr_decode_funct3(instr);
			switch(funct3){
				case F3_SYSTEM_PRIV:{
					funct12 = instr_decode_funct12(instr);
					switch(funct12){
						case F12_SYSTEM_PRIV_ECALL:{
							assert(0 && "ECALL Not implemented");
							RISCV_instr_ecall(cpu, instr);
						}break;

						case F12_SYSTEM_PRIV_EBREAK:{
							assert(0 && "EBREAK Not implemented");
							RISCV_instr_ebreak(cpu, instr);
						}break;

						default:{
							fprintf(stderr,
									"Error, bad funct12 code: 0x%08x (opcode: 0x%08x, funct3: 0x%08x).\n",
									funct12, opcode, funct3
							);
							assert(0 && "Invalid funct12 code!");
						}
					}
				}break;

				default:{
					fprintf(stderr,
							"Error, bad funct3 code: 0x%08x (opcode: 0x%08x).\n",
							funct3, opcode
							);
					assert(0 && "Invalid funct3 code!");
				}
			}
		}break;

		default:{
			// TODO: cleaner error handling ? 
			fprintf(stderr, "Error, bad opcode: 0x%08x.\n", opcode);
			assert(0 && "Invalid opcode!");
		}
	}
	// ZERO is always 0
	cpu->reg[ZERO] = 0;
}

void RISCV_print_reg(RISCV_st *cpu)
{
	assert(cpu);

	printf(" Register\t| Hex\t\t| Dec\n");
	printf("-------------------------------------------------\n");
	for(int i=0 ; i<32 ; i++){
		printf(" x%02d %s\t| 0x %08x\t| %d\n", i, REG_NAMES_TAB[i], cpu->reg[i], cpu->reg[i]);
	}
}

void RISCV_print_pc(RISCV_st *cpu)
{
	assert(cpu);

	printf("pc:\t0x %08x\n", cpu->pc);
}

void RISCV_print_mem(RISCV_st *cpu, size_t start, size_t size)
{
	assert(cpu);
	assert(cpu->mem_size <= start + size);

	printf("  Offset: 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
	
	for(size_t i=start ; i<start+size ; i++){
		if(!(i % 0xF))
			printf("\n%08lu:", i);
		printf(" %02x", cpu->mem[i]);
	}
}

uint32_t RISCV_fetch_instr(RISCV_st *cpu)
{
	uint32_t instr = 
		(uint32_t)cpu->mem[cpu->pc] | 
		((uint32_t)cpu->mem[cpu->pc + 1] << 8) |
		((uint32_t)cpu->mem[cpu->pc + 2] << 16) |
		((uint32_t)cpu->mem[cpu->pc + 3] << 24);
	cpu->pc += 4;

	return instr;
}

// Decoding instructions
//	Opcodes
uint8_t instr_decode_opcode(const uint32_t instr)
{
	// bits n°0 to 6
	// ... x111 1111
	return instr & 0x7F;
}

uint8_t instr_decode_funct3(const uint32_t instr)
{
	// bits n°12 to 14
	// ... x111 xxxx xxxx xxxx
	return (instr >> 12) & 0x7;
}

uint8_t instr_decode_funct7(const uint32_t instr)
{
	// bits n°25 to 31
	// 1111 111x xxxx xxxx xxxx xxxx xxxx xxxx
	return instr >> 25;
}

uint8_t instr_decode_funct12(const uint32_t instr)
{
	// bits n°20 to 31
	// 1111 1111 1111 xxxx xxxx xxxx xxxx xxxx
	return instr >> 20;
}

//	Arguments fields
uint8_t instr_decode_rd(const uint32_t instr)
{
	// bits n°7 to 11
	// ... 1111 1xxx xxxx
	return (instr >> 7) & 0x1F;
}

uint8_t instr_decode_rs1(const uint32_t instr)
{
	// bits n°15 to 19
	// ... 1111 1xxx xxxx xxxx xxxx
	return (instr >> 15) & 0x1F;
}

uint8_t instr_decode_rs2(const uint32_t instr)
{
	// bits n°20 to 24
	// ... xxx1 1111 xxxx xxxx xxxx xxxx xxxx
	return (instr >> 20) & 0x1F;
}

int16_t instr_decode_imm_11_0(const uint32_t instr)
{
	// 12 bits signed integer
	// bits n°20 to 31
	// 1111 1111 1111 xxxx xxxx xxxx xxxx xxxx
	uint16_t imm = instr >> 20;
	// Check sign bit (n°11). If 1, set all left bits to 1.
	return (int16_t)((imm & 0x800)? (imm | 0xF000) : imm);
}

uint16_t instr_decode_imm_11_0_u(const uint32_t instr)
{
	// 12 bits signed integer
	// bits n°20 to 31
	// 1111 1111 1111 xxxx xxxx xxxx xxxx xxxx
	return instr >> 20;
}

int32_t instr_decode_imm_31_12(const uint32_t instr)
{
	return (int32_t)(instr & 0xFFFFF000);
}

int32_t instr_decode_imm_jal(const uint32_t instr)
{
	// 20 bits signed integer
	// bits n°12 to 31
	// 1111 1111 1111 1111 1111 xxxx xxxx xxxx
	//		instr	   ->		imm
	//				31 -> 20
	//		  30 to 21 -> 10 to 1
	//				20 -> 11
	//		  19 to 12 -> 19 to 12
	//
	// n°0 is 0
	uint32_t imm =
		((instr >> 11) & 0x100000) |
	// 1xxx X X X X X X X -> X X xxx1 X X X X X
		((instr >> 20) & 0x7FE) | 
	// x111 1111 111x X X X X X -> X X X X X x111 1111 1110
		((instr >> 9) & 0x800) | 
	// X X xxx1 X X X X X -> X X X X X 1xxx X X
		(instr & 0xFF000);
	// X X X xx11 1111 11xx X X -> X X X 1111 1111 X X X
	
	// Check sign bit (n°20). If 1, set all leftmost bits to 1.
	return (int32_t)((imm & 0x100000)? (imm | 0xFFF00000) : imm);
}

int16_t instr_decode_imm_branch(const uint32_t instr)
{
	// 12 bits signed integer
	// bits n°7 to 11 & 25 to 31
	// 1111 111x xxxx xxxx xxxx 1111 1xxx xxxx
	//		instr	   ->		imm
	//				31 -> 12
	//		  30 to 25 -> 10 to 5
	//		   11 to 8 -> 4 to 1
	//				 7 -> 11
	//
	// n°0 is 0
	uint16_t imm =
		((instr >> 19)	& 0x1000) |
	// 1xxx X X X X X X X -> xxx1 X X X
		((instr >> 20)	& 0x7E0) | 
	// x111 111x X X X X X X -> X x111 111x X
		((instr >> 7)	& 0x1E) | 
	// X X X X X 1111 X X -> X X xxx1 1110
		((instr << 4)	& 0x800);
	// X X X X X X 1xxx X -> X 1xxx X X
	
	// Check sign bit (n°12). If 1, set all leftmost bits to 1.
	return (imm & 0x1000)? (imm | 0xF000) : imm;
}

int16_t instr_decode_imm_store(const uint32_t instr)
{
	// 12 bits signed integer
	// bits n°7 to 11 & 25 to 31
	// 1111 111x xxxx xxxx xxxx 1111 1xxx xxxx
	//		instr	   ->		imm
	//		  31 to 25 -> 11 to 5
	//		   11 to 7 -> 4 to 0
	uint16_t imm =
		((instr >> 19)	& 0xFE0) |
	// 1111 111x X X X X X X -> 1111 111x X
		((instr >> 7)	& 0x1F);
	// X X X X X 1111 1xxx X -> X X xxx1 1111
	
	// Check sign bit (n°12). If 1, set all leftmost bits to 1.
	return (int16_t)((imm & 0x100000)? (imm | 0xF000) : imm);
}

uint8_t instr_decode_imm_shamt(const uint32_t instr)
{
	// 5 bits unsigned integer
	// bits n° 0 to 4
	// ... xxx1 1111
	
	return instr & 0x1F;
}

// Instructions implementation
void RISCV_instr_lui(RISCV_st *cpu, uint32_t instr)
{
	DEBUG_PRINT("%s", "instr: lui\n");
	cpu->reg[instr_decode_rd(instr)] = instr_decode_imm_31_12(instr);
}

void RISCV_instr_auipc(RISCV_st *cpu, uint32_t instr)
{
	DEBUG_PRINT("%s", "instr: auipc\n");
	// TODO: check this not sure, sould it be cpu->pc += imm ?
	cpu->reg[instr_decode_rd(instr)] = cpu->pc + instr_decode_imm_31_12(instr);
}
void RISCV_instr_jal(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rd = instr_decode_rd(instr);
	int32_t imm = instr_decode_imm_jal(instr);

	DEBUG_PRINT("instr: jal %s, 0x%08x\n", REG_NAMES[rd], imm);

	// pc has been incremented in fetch_instr(), so pc points to the next instr
	cpu->reg[rd] = cpu->pc; // store pc+4
	cpu->pc += imm - 4; // offset pc by imm
}

void RISCV_instr_jalr(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rd = instr_decode_rd(instr);
	uint8_t rs1 = instr_decode_rs1(instr);
	int16_t imm = instr_decode_imm_11_0(instr);

	DEBUG_PRINT("instr: jalr %s, %d(%s)\n", REG_NAMES[rd], imm, REG_NAMES[rs1]);

	// pc has been incremented in fetch_instr(), so pc points to the next instr
	cpu->reg[rd] = cpu->pc; // store pc+4
	cpu->pc = (cpu->reg[rs1] + imm) & ~0x1;
}

void RISCV_instr_beq(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rs1 = instr_decode_rs1(instr);
	uint8_t rs2 = instr_decode_rs2(instr);
	int16_t imm = instr_decode_imm_branch(instr);

	DEBUG_PRINT("instr: beq %s, %s, 0x%08x\n", REG_NAMES[rs1], REG_NAMES[rs2], imm);

	// pc has been incremented in fetch_instr(), so pc points to the next instr
	if(cpu->reg[rs1] == cpu->reg[rs2])
		cpu->pc += imm - 4;// offset pc by imm
}

void RISCV_instr_bne(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rs1 = instr_decode_rs1(instr);
	uint8_t rs2 = instr_decode_rs2(instr);
	int16_t imm = instr_decode_imm_branch(instr);

	DEBUG_PRINT("instr: bne %s, %s, 0x%08x\n", REG_NAMES[rs1], REG_NAMES[rs2], imm);

	// pc has been incremented in fetch_instr(), so pc points to the next instr
	if(cpu->reg[rs1] != cpu->reg[rs2])
		cpu->pc += imm - 4;// offset pc by imm
}

void RISCV_instr_blt(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rs1 = instr_decode_rs1(instr);
	uint8_t rs2 = instr_decode_rs2(instr);
	int16_t imm = instr_decode_imm_branch(instr);

	DEBUG_PRINT("instr: blt %s, %s, 0x%08x\n", REG_NAMES[rs1], REG_NAMES[rs2], imm);

	// pc has been incremented in fetch_instr(), so pc points to the next instr
	if(cpu->reg[rs1] < cpu->reg[rs2])
		cpu->pc += imm - 4;// offset pc by imm
}

void RISCV_instr_bge(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rs1 = instr_decode_rs1(instr);
	uint8_t rs2 = instr_decode_rs2(instr);
	int16_t imm = instr_decode_imm_branch(instr);

	DEBUG_PRINT("instr: bge %s, %s, 0x%08x\n", REG_NAMES[rs1], REG_NAMES[rs2], imm);

	// pc has been incremented in fetch_instr(), so pc points to the next instr
	if(cpu->reg[rs1] >= cpu->reg[rs2])
		cpu->pc += imm - 4;// offset pc by imm
}

void RISCV_instr_bltu(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rs1 = instr_decode_rs1(instr);
	uint8_t rs2 = instr_decode_rs2(instr);
	int16_t imm = instr_decode_imm_branch(instr);

	DEBUG_PRINT("instr: bltu %s, %s, 0x%08x\n", REG_NAMES[rs1], REG_NAMES[rs2], imm);

	// pc has been incremented in fetch_instr(), so pc points to the next instr
	if((uint32_t)cpu->reg[rs1] < (uint32_t)cpu->reg[rs2])
		cpu->pc += imm - 4;// offset pc by imm
}

void RISCV_instr_bgeu(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rs1 = instr_decode_rs1(instr);
	uint8_t rs2 = instr_decode_rs2(instr);
	int16_t imm = instr_decode_imm_branch(instr);

	DEBUG_PRINT("instr: bgeu %s, %s, 0x%08x\n", REG_NAMES[rs1], REG_NAMES[rs2], imm);

	// pc has been incremented in fetch_instr(), so pc points to the next instr
	if((uint32_t)cpu->reg[rs1] >= (uint32_t)cpu->reg[rs2])
		cpu->pc += imm - 4;// offset pc by imm
}

void RISCV_instr_lb(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rd = instr_decode_rd(instr);
	uint8_t rs1 = instr_decode_rs1(instr);
	int16_t imm = instr_decode_imm_11_0(instr);
	
	DEBUG_PRINT("instr: lb %s, %d(%s)\n", REG_NAMES[rd], imm, REG_NAMES[rs1]);

	// TODO: exception if rd is zero register
	// TODO: check addr > 0 ?
	// uint32_t value = (uint32_t)cpu->mem[instr_decode_rs1(instr) + instr_decode_imm_11_0(instr)];
	// Check sign bit (n°7). If 1, set all leftmost bits to 1.
	// cpu->reg[instr_decode_rd(instr)] = ((value & 0x80)? (value | 0xFFFFFF80) : value); 
	cpu->reg[rd] = (int8_t) cpu->mem[cpu->reg[rs1] + imm];
}

void RISCV_instr_lh(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rd = instr_decode_rd(instr);
	uint8_t rs1 = instr_decode_rs1(instr);
	int16_t imm = instr_decode_imm_11_0(instr);
	int32_t addr = cpu->reg[rs1] + imm;

	DEBUG_PRINT("instr: lh %s, %d(%s)\n", REG_NAMES[rd], imm, REG_NAMES[rs1]);

	// TODO: exception if rd is zero register
	// TODO: check addr > 0 ?
	cpu->reg[rd] = (int16_t)
		((uint16_t)cpu->mem[addr] | ((uint16_t)cpu->mem[addr + 1] << 8));
}

void RISCV_instr_lw(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rd = instr_decode_rd(instr);
	uint8_t rs1 = instr_decode_rs1(instr);
	int16_t imm = instr_decode_imm_11_0(instr);
	int32_t addr = cpu->reg[rs1] + imm;
	
	DEBUG_PRINT("instr: lw %s, %d(%s)\n", REG_NAMES[rd], imm, REG_NAMES[rs1]);

	// TODO: exception if rd is zero register
	// TODO: check addr > 0 ?
	cpu->reg[rd] = 
		(uint32_t)cpu->mem[addr] | 
		((uint32_t)cpu->mem[addr + 1] << 8) |
		((uint32_t)cpu->mem[addr + 2] << 16) |
		((uint32_t)cpu->mem[addr + 3] << 24);
}

void RISCV_instr_lbu(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rd = instr_decode_rd(instr);
	uint8_t rs1 = instr_decode_rs1(instr);
	int16_t imm = instr_decode_imm_11_0(instr);
	
	DEBUG_PRINT("instr: lbu %s, %d(%s)\n", REG_NAMES[rd], imm, REG_NAMES[rs1]);

	// TODO: exception if rd is zero register
	// TODO: check addr > 0 ?
	cpu->reg[rd] = cpu->mem[cpu->reg[rs1] + imm];
}

void RISCV_instr_lhu(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rd = instr_decode_rd(instr);
	uint8_t rs1 = instr_decode_rs1(instr);
	int16_t imm = instr_decode_imm_11_0(instr);
	int32_t addr = cpu->reg[rs1] + imm;
	
	DEBUG_PRINT("instr: lhu %s, %d(%s)\n", REG_NAMES[rd], imm, REG_NAMES[rs1]);

	// TODO: exception if rd is zero register
	// TODO: check addr > 0 ?
	cpu->reg[rd] = 
		((uint16_t)cpu->mem[addr] | ((uint16_t)cpu->mem[addr + 1] << 8));
}

void RISCV_instr_sb(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rs1 = instr_decode_rs1(instr);
	uint8_t rs2 = instr_decode_rs2(instr);
	int16_t imm = instr_decode_imm_store(instr);

	DEBUG_PRINT("instr: sb %s, %d(%s)\n", REG_NAMES[rs1], imm, REG_NAMES[rs2]);

	// TODO: check addr > 0 ?
	cpu->mem[cpu->reg[rs1] + imm] = cpu->reg[rs2];
}

void RISCV_instr_sh(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rs1 = instr_decode_rs1(instr);
	uint8_t rs2 = instr_decode_rs2(instr);
	int16_t imm = instr_decode_imm_store(instr);
	int32_t addr = cpu->reg[rs1] + imm;

	DEBUG_PRINT("instr: sh %s, %d(%s)\n", REG_NAMES[rs1], imm, REG_NAMES[rs2]);

	// TODO: check addr > 0 ?
	cpu->mem[addr] = cpu->reg[rs2];
	cpu->mem[addr + 1] = cpu->reg[rs2] >> 8;
}

void RISCV_instr_sw(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rs1 = instr_decode_rs1(instr);
	uint8_t rs2 = instr_decode_rs2(instr);
	int16_t imm = instr_decode_imm_store(instr);
	int32_t addr = cpu->reg[rs1] + imm;

	DEBUG_PRINT("instr: sw %s, %d(%s)\n", REG_NAMES[rs1], imm, REG_NAMES[rs2]);

	// TODO: check addr > 0 ?
	cpu->mem[addr] = cpu->reg[rs2];
	cpu->mem[addr + 1] = cpu->reg[rs2] >> 8;
	cpu->mem[addr + 2] = cpu->reg[rs2] >> 16;
	cpu->mem[addr + 3] = cpu->reg[rs2] >> 24;
}

void RISCV_instr_addi(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rd = instr_decode_rd(instr);
	uint8_t rs1 = instr_decode_rs1(instr);
	int16_t imm = instr_decode_imm_11_0(instr);

	DEBUG_PRINT("instr: addi %s, %s, %d\n", REG_NAMES[rd], REG_NAMES[rs1], imm);

	cpu->reg[rd] = cpu->reg[rs1] + imm;
}

void RISCV_instr_slti(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rd = instr_decode_rd(instr);
	uint8_t rs1 = instr_decode_rs1(instr);
	int16_t imm = instr_decode_imm_11_0(instr);

	DEBUG_PRINT("instr: slti %s, %s, %d\n", REG_NAMES[rd], REG_NAMES[rs1], imm);

	cpu->reg[rd] = cpu->reg[rs1] < imm;
}

void RISCV_instr_sltiu(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rd = instr_decode_rd(instr);
	uint8_t rs1 = instr_decode_rs1(instr);
	uint16_t imm = instr_decode_imm_11_0_u(instr);

	DEBUG_PRINT("instr: addi %s, %s, %d\n", REG_NAMES[rd], REG_NAMES[rs1], imm);

	cpu->reg[rd] = cpu->reg[rs1] < imm;
}

void RISCV_instr_xori(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rd = instr_decode_rd(instr);
	uint8_t rs1 = instr_decode_rs1(instr);
	int16_t imm = instr_decode_imm_11_0(instr);

	DEBUG_PRINT("instr: xori %s, %s, %d\n", REG_NAMES[rd], REG_NAMES[rs1], imm);

	// TODO: check this
	cpu->reg[rd] = cpu->reg[rs1] ^ imm;
}

void RISCV_instr_ori(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rd = instr_decode_rd(instr);
	uint8_t rs1 = instr_decode_rs1(instr);
	int16_t imm = instr_decode_imm_11_0(instr);

	DEBUG_PRINT("instr: ori %s, %s, %d\n", REG_NAMES[rd], REG_NAMES[rs1], imm);

	// TODO: check this
	cpu->reg[rd] = cpu->reg[rs1] | imm;
}

void RISCV_instr_andi(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rd = instr_decode_rd(instr);
	uint8_t rs1 = instr_decode_rs1(instr);
	int16_t imm = instr_decode_imm_11_0(instr);

	DEBUG_PRINT("instr: andi %s, %s, %d\n", REG_NAMES[rd], REG_NAMES[rs1], imm);

	// TODO: check this
	cpu->reg[rd] = cpu->reg[rs1] & imm;
}

void RISCV_instr_slli(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rd = instr_decode_rd(instr);
	uint8_t rs1 = instr_decode_rs1(instr);
	uint8_t shamt = instr_decode_imm_shamt(instr);

	DEBUG_PRINT("instr: slli %s, %s, %d\n", REG_NAMES[rd], REG_NAMES[rs1], shamt);

	// TODO: check this
	cpu->reg[rd] = (uint32_t)cpu->reg[rs1] << shamt;
}

void RISCV_instr_srli(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rd = instr_decode_rd(instr);
	uint8_t rs1 = instr_decode_rs1(instr);
	uint8_t shamt = instr_decode_imm_shamt(instr);

	DEBUG_PRINT("instr: srli %s, %s, %d\n", REG_NAMES[rd], REG_NAMES[rs1], shamt);

	// TODO: check this
	cpu->reg[rd] = (uint32_t)cpu->reg[rs1] >> shamt;
}

void RISCV_instr_srai(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rd = instr_decode_rd(instr);
	uint8_t rs1 = instr_decode_rs1(instr);
	uint8_t shamt = instr_decode_imm_shamt(instr);
	int32_t vrs1 = cpu->reg[rs1];

	DEBUG_PRINT("instr: srai %s, %s, %d\n", REG_NAMES[rd], REG_NAMES[rs1], shamt);

	// TODO: check this
	cpu->reg[rd] = (vrs1 < 0)? ~(~vrs1 >> shamt) : vrs1 >> shamt;
}

void RISCV_instr_add(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rd = instr_decode_rd(instr);
	uint8_t rs1 = instr_decode_rs1(instr);
	uint8_t rs2 = instr_decode_rs2(instr);

	DEBUG_PRINT("instr: add %s, %s, %s\n", REG_NAMES[rd], REG_NAMES[rs1], REG_NAMES[rs2]);

	cpu->reg[rd] = cpu->reg[rs1] + cpu->reg[rs2];
}

void RISCV_instr_sub(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rd = instr_decode_rd(instr);
	uint8_t rs1 = instr_decode_rs1(instr);
	uint8_t rs2 = instr_decode_rs2(instr);

	DEBUG_PRINT("instr: sub %s, %s, %s\n", REG_NAMES[rd], REG_NAMES[rs1], REG_NAMES[rs2]);

	cpu->reg[rd] = cpu->reg[rs1] - cpu->reg[rs2];
}

void RISCV_instr_sll(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rd = instr_decode_rd(instr);
	uint8_t rs1 = instr_decode_rs1(instr);
	uint8_t rs2 = instr_decode_rs2(instr);

	DEBUG_PRINT("instr: sll %s, %s, %s\n", REG_NAMES[rd], REG_NAMES[rs1], REG_NAMES[rs2]);

	cpu->reg[rd] =
		(uint32_t)cpu->reg[rs1] << (uint8_t)(cpu->reg[rs2] & 0x1F); // 5 last bits
}

void RISCV_instr_slt(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rd = instr_decode_rd(instr);
	uint8_t rs1 = instr_decode_rs1(instr);
	uint8_t rs2 = instr_decode_rs2(instr);

	DEBUG_PRINT("instr: slt %s, %s, %s\n", REG_NAMES[rd], REG_NAMES[rs1], REG_NAMES[rs2]);

	cpu->reg[rd] = cpu->reg[rs1] < cpu->reg[rs2];
}

void RISCV_instr_sltu(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rd = instr_decode_rd(instr);
	uint8_t rs1 = instr_decode_rs1(instr);
	uint8_t rs2 = instr_decode_rs2(instr);

	DEBUG_PRINT("instr: sltu %s, %s, %s\n", REG_NAMES[rd], REG_NAMES[rs1], REG_NAMES[rs2]);

	cpu->reg[rd] = (uint32_t)cpu->reg[rs1] < (uint32_t)cpu->reg[rs2];
}
void RISCV_instr_xor(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rd = instr_decode_rd(instr);
	uint8_t rs1 = instr_decode_rs1(instr);
	uint8_t rs2 = instr_decode_rs2(instr);

	DEBUG_PRINT("instr: xor %s, %s, %s\n", REG_NAMES[rd], REG_NAMES[rs1], REG_NAMES[rs2]);

	cpu->reg[rd] = cpu->reg[rs1] ^ cpu->reg[rs2];
}

void RISCV_instr_srl(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rd = instr_decode_rd(instr);
	uint8_t rs1 = instr_decode_rs1(instr);
	uint8_t rs2 = instr_decode_rs2(instr);

	DEBUG_PRINT("instr: srl %s, %s, %s\n", REG_NAMES[rd], REG_NAMES[rs1], REG_NAMES[rs2]);

	cpu->reg[rd] =
		(uint32_t)cpu->reg[rs1] >> 
		(uint8_t)(cpu->reg[rs2] & 0x1F); // 5 last bits
}

void RISCV_instr_sra(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rd = instr_decode_rd(instr);
	uint8_t rs1 = instr_decode_rs1(instr);
	uint8_t rs2 = instr_decode_rs2(instr);
	int32_t vrs1 = cpu->reg[rs1];
	uint8_t vrs2 = cpu->reg[rs2] & 0x1F; // 5 last bits

	DEBUG_PRINT("instr: add %s, %s, %s\n", REG_NAMES[rd], REG_NAMES[rs1], REG_NAMES[rs2]);

	cpu->reg[rd] = (vrs1 < 0)? ~(~vrs1 >> vrs2) : vrs1 >> vrs2;
}

void RISCV_instr_or(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rd = instr_decode_rd(instr);
	uint8_t rs1 = instr_decode_rs1(instr);
	uint8_t rs2 = instr_decode_rs2(instr);

	DEBUG_PRINT("instr: or %s, %s, %s\n", REG_NAMES[rd], REG_NAMES[rs1], REG_NAMES[rs2]);

	cpu->reg[rd] = cpu->reg[rs1] | cpu->reg[rs2];
}

void RISCV_instr_and(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rd = instr_decode_rd(instr);
	uint8_t rs1 = instr_decode_rs1(instr);
	uint8_t rs2 = instr_decode_rs2(instr);

	DEBUG_PRINT("instr: and %s, %s, %s\n", REG_NAMES[rd], REG_NAMES[rs1], REG_NAMES[rs2]);

	cpu->reg[rd] = cpu->reg[rs1] & cpu->reg[rs2];
}

void RISCV_instr_fence(RISCV_st *cpu, uint32_t instr)
{
	// TODO
	do{}while(0);
}
void RISCV_instr_ecall(RISCV_st *cpu, uint32_t instr)
{
	// TODO
	do{}while(0);
}
void RISCV_instr_ebreak(RISCV_st *cpu, uint32_t instr)
{
	// TODO
	do{}while(0);
}
