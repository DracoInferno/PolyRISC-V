#include "PolyRISC-V.h"

const char REG_NAMES[32][6] = {
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

void RISCV_reset(RISCV_st *cpu, uint8_t *elf, size_t elf_size, 
				RISCV_rst_op_st *options)
{
	assert(cpu);
	assert(elf);
	assert(elf_size > 0);
	assert(elf_size < cpu->mem_size);

	(void)options;

	// Set registers to 0
	memset(cpu->reg, 0, sizeof(cpu->reg));

	// Set stack pointer register
	cpu->reg[SP] = cpu->stack_top;
	
	// Set stack lower boundary
	cpu->stack_bot = elf_size;

	// Copy program to memory
	memcpy(cpu->mem, elf, elf_size);
}

void RISCV_step(RISCV_st *cpu)
{
	uint32_t instr = 0;
	uint8_t opcode = 0;
	uint8_t funct3 = 0;
	uint8_t funct7 = 0;
	uint8_t funct12 = 0;

	// Fetch instruction
	instr = RISCV_fetch_instr(cpu);

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
							"Error, bad funct3 code: %x (opcode: %x).\n",
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
							"Error, bad funct3 code: %x (opcode: %x).\n",
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
							"Error, bad funct3 code: %x (opcode: %x).\n",
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
							assert(0 && "SRLI Not implemented");
						}break;

						case F7_OP_IMM_SRXI_SRAI:{
							RISCV_instr_srai(cpu, instr);
							assert(0 && "SRAI Not implemented");
						}break;

						default:{
							fprintf(stderr,
									"Error, bad funct7 code: %x (opcode: %x, funct3: %x).\n",
									funct7, opcode, funct3
							);
							assert(0 && "Invalid funct7 code!");
						}
					}
				}break;

				default:{
					fprintf(stderr,
							"Error, bad funct3 code: %x (opcode: %x).\n",
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
									"Error, bad funct7 code: %x (opcode: %x, funct3: %x).\n",
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
									"Error, bad funct7 code: %x (opcode: %x, funct3: %x).\n",
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
							"Error, bad funct3 code: %x (opcode: %x).\n",
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
							"Error, bad funct3 code: %x (opcode: %x).\n",
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
									"Error, bad funct12 code: %x (opcode: %x, funct3: %x).\n",
									funct12, opcode, funct3
							);
							assert(0 && "Invalid funct12 code!");
						}
					}
				}break;

				default:{
					fprintf(stderr,
							"Error, bad funct3 code: %x (opcode: %x).\n",
							funct3, opcode
							);
					assert(0 && "Invalid funct3 code!");
				}
			}
		}break;

		default:{
			// TODO: cleaner error handling ? 
			fprintf(stderr, "Error, bad opcode: %x.\n", opcode);
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
		printf(" x%02d %s\t| 0x %08x\t| %d\n", i, REG_NAMES[i], cpu->reg[i], cpu->reg[i]);
	}
}

void RISCV_print_mem(RISCV_st *cpu, size_t start, size_t size)
{
	assert(cpu);
	assert(cpu->mem_size <= start + size);

	printf("  Offset: 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F");
	
	for(size_t i=start ; i<start+size ; i++){
		if(!(i % 0xF))
			printf("\n%08x:", i);
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
	cpu->pc++;

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
		((instr >> 20) & 0x3FE) | 
	// x111 1111 111x X X X X X -> X X X X X xx11 1111 1110
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
	return (int16_t)((imm & 0x100000)? (imm | 0xF000) : imm);
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
	cpu->reg[instr_decode_rd(instr)] = instr_decode_imm_31_12(instr);
}

void RISCV_instr_auipc(RISCV_st *cpu, uint32_t instr)
{
	// TODO: check this not sure, sould it be cpu->pc += imm ?
	cpu->reg[instr_decode_rd(instr)] = cpu->pc + instr_decode_imm_31_12(instr);
}
void RISCV_instr_jal(RISCV_st *cpu, uint32_t instr)
{
	cpu->reg[instr_decode_rd(instr)] = cpu->pc + 4;
	cpu->pc += instr_decode_imm_jal(instr);
}

void RISCV_instr_jalr(RISCV_st *cpu, uint32_t instr)
{
	cpu->reg[instr_decode_rd(instr)] = cpu->pc + 4;
	cpu->pc = (cpu->reg[instr_decode_rs1(instr)] + instr_decode_imm_11_0(instr)) & ~0x1;
}

void RISCV_instr_beq(RISCV_st *cpu, uint32_t instr)
{
	if(instr_decode_rs1(instr) == instr_decode_rs2(instr))
		cpu->pc += instr_decode_imm_branch(instr);
}

void RISCV_instr_bne(RISCV_st *cpu, uint32_t instr)
{
	if(instr_decode_rs1(instr) != instr_decode_rs2(instr))
		cpu->pc += instr_decode_imm_branch(instr);
}

void RISCV_instr_blt(RISCV_st *cpu, uint32_t instr)
{
	if(instr_decode_rs1(instr) < instr_decode_rs2(instr))
		cpu->pc += instr_decode_imm_branch(instr);
}

void RISCV_instr_bge(RISCV_st *cpu, uint32_t instr)
{
	if(instr_decode_rs1(instr) >= instr_decode_rs2(instr))
		cpu->pc += instr_decode_imm_branch(instr);
}

void RISCV_instr_bltu(RISCV_st *cpu, uint32_t instr)
{
	if((uint32_t)instr_decode_rs1(instr) < (uint32_t)instr_decode_rs2(instr))
		cpu->pc += instr_decode_imm_branch(instr);
}

void RISCV_instr_bgeu(RISCV_st *cpu, uint32_t instr)
{
	if((uint32_t)instr_decode_rs1(instr) >= (uint32_t)instr_decode_rs2(instr))
		cpu->pc += instr_decode_imm_branch(instr);
}

void RISCV_instr_lb(RISCV_st *cpu, uint32_t instr)
{
	// TODO: exception if rd is zero register
	// TODO: check addr > 0 ?
	// uint32_t value = (uint32_t)cpu->mem[instr_decode_rs1(instr) + instr_decode_imm_11_0(instr)];
	// Check sign bit (n°7). If 1, set all leftmost bits to 1.
	// cpu->reg[instr_decode_rd(instr)] = ((value & 0x80)? (value | 0xFFFFFF80) : value); 
	cpu->reg[instr_decode_rd(instr)] =
		(int8_t)cpu->mem[cpu->reg[instr_decode_rs1(instr)] + instr_decode_imm_11_0(instr)];
}

void RISCV_instr_lh(RISCV_st *cpu, uint32_t instr)
{
	// TODO: exception if rd is zero register
	// TODO: check addr > 0 ?
	int32_t addr = cpu->reg[instr_decode_rs1(instr)] + instr_decode_imm_11_0(instr);
	cpu->reg[instr_decode_rd(instr)] = 
		(int16_t)((uint16_t)cpu->mem[addr] | ((uint16_t)cpu->mem[addr + 1] << 8));
}

void RISCV_instr_lw(RISCV_st *cpu, uint32_t instr)
{
	// TODO: exception if rd is zero register
	// TODO: check addr > 0 ?
	int32_t addr = cpu->reg[instr_decode_rs1(instr)] + instr_decode_imm_11_0(instr);
	cpu->reg[instr_decode_rd(instr)] = 
		(uint32_t)cpu->mem[addr] | 
		((uint32_t)cpu->mem[addr + 1] << 8) |
		((uint32_t)cpu->mem[addr + 2] << 16) |
		((uint32_t)cpu->mem[addr + 3] << 24);
}

void RISCV_instr_lbu(RISCV_st *cpu, uint32_t instr)
{
	// TODO: exception if rd is zero register
	// TODO: check addr > 0 ?
	cpu->reg[instr_decode_rd(instr)] = 
		cpu->mem[cpu->reg[instr_decode_rs1(instr)] + instr_decode_imm_11_0(instr)];
}

void RISCV_instr_lhu(RISCV_st *cpu, uint32_t instr)
{
	// TODO: exception if rd is zero register
	// TODO: check addr > 0 ?
	int32_t addr = cpu->reg[instr_decode_rs1(instr)] + instr_decode_imm_11_0(instr);
	cpu->reg[instr_decode_rd(instr)] = 
		((uint16_t)cpu->mem[addr] | ((uint16_t)cpu->mem[addr + 1] << 8));
}

void RISCV_instr_sb(RISCV_st *cpu, uint32_t instr)
{
	// TODO: check addr > 0 ?
	cpu->mem[cpu->reg[instr_decode_rs1(instr)] + instr_decode_imm_store(instr)] = cpu->reg[instr_decode_rs2(instr)];
}

void RISCV_instr_sh(RISCV_st *cpu, uint32_t instr)
{
	// TODO: check addr > 0 ?
	int32_t addr = cpu->reg[instr_decode_rs1(instr)] + instr_decode_imm_store(instr);
	uint16_t value = cpu->reg[instr_decode_rs2(instr)];
	cpu->mem[addr] = value;
	cpu->mem[addr + 1] = value >> 8;
}

void RISCV_instr_sw(RISCV_st *cpu, uint32_t instr)
{
	// TODO: check addr > 0 ?
	int32_t addr = cpu->reg[instr_decode_rs1(instr)] + instr_decode_imm_store(instr);
	uint32_t value = cpu->reg[instr_decode_rs2(instr)];
	cpu->mem[addr] = value;
	cpu->mem[addr + 1] = value >> 8;
	cpu->mem[addr + 2] = value >> 16;
	cpu->mem[addr + 3] = value >> 24;
}

void RISCV_instr_addi(RISCV_st *cpu, uint32_t instr)
{
	cpu->reg[instr_decode_rd(instr)] = cpu->reg[instr_decode_rs1(instr)] + instr_decode_imm_11_0(instr);
}

void RISCV_instr_slti(RISCV_st *cpu, uint32_t instr)
{
	cpu->reg[instr_decode_rd(instr)] = cpu->reg[instr_decode_rs1(instr)] < instr_decode_imm_11_0(instr);
}

void RISCV_instr_sltiu(RISCV_st *cpu, uint32_t instr)
{
	cpu->reg[instr_decode_rd(instr)] = cpu->reg[instr_decode_rs1(instr)] < instr_decode_imm_11_0_u(instr);
}

void RISCV_instr_xori(RISCV_st *cpu, uint32_t instr)
{
	// TODO: check this
	cpu->reg[instr_decode_rd(instr)] = cpu->reg[instr_decode_rs1(instr)] ^ instr_decode_imm_11_0(instr);
}

void RISCV_instr_ori(RISCV_st *cpu, uint32_t instr)
{
	// TODO: check this
	cpu->reg[instr_decode_rd(instr)] = cpu->reg[instr_decode_rs1(instr)] | instr_decode_imm_11_0(instr);
}

void RISCV_instr_andi(RISCV_st *cpu, uint32_t instr)
{
	// TODO: check this
	cpu->reg[instr_decode_rd(instr)] = cpu->reg[instr_decode_rs1(instr)] & instr_decode_imm_11_0(instr);
}

void RISCV_instr_slli(RISCV_st *cpu, uint32_t instr)
{
	// TODO: check this
	cpu->reg[instr_decode_rd(instr)] = (uint32_t)cpu->reg[instr_decode_rs1(instr)] << instr_decode_imm_shamt(instr);
}

void RISCV_instr_srli(RISCV_st *cpu, uint32_t instr)
{
	// TODO: check this
	cpu->reg[instr_decode_rd(instr)] = (uint32_t)cpu->reg[instr_decode_rs1(instr)] >> instr_decode_imm_shamt(instr);
}

void RISCV_instr_srai(RISCV_st *cpu, uint32_t instr)
{
	// TODO: check this
	int32_t vrs1 = cpu->reg[instr_decode_rs1(instr)];
	uint32_t shamt = instr_decode_rs2(instr); // 5 last bits
	cpu->reg[instr_decode_rd(instr)] = (vrs1 < 0)? ~(~vrs1 >> shamt) : vrs1 >> shamt;
}

void RISCV_instr_add(RISCV_st *cpu, uint32_t instr)
{
	cpu->reg[instr_decode_rd(instr)] = cpu->reg[instr_decode_rs1(instr)] + cpu->reg[instr_decode_rs2(instr)];
}

void RISCV_instr_sub(RISCV_st *cpu, uint32_t instr)
{
	cpu->reg[instr_decode_rd(instr)] = cpu->reg[instr_decode_rs1(instr)] - cpu->reg[instr_decode_rs2(instr)];
}

void RISCV_instr_sll(RISCV_st *cpu, uint32_t instr)
{
	cpu->reg[instr_decode_rd(instr)] =
		(uint32_t)cpu->reg[instr_decode_rs1(instr)] <<
		(uint32_t)(cpu->reg[instr_decode_rs2(instr)] & 0x1F); // 5 last bits
}

void RISCV_instr_slt(RISCV_st *cpu, uint32_t instr)
{
	cpu->reg[instr_decode_rd(instr)] = cpu->reg[instr_decode_rs1(instr)] < cpu->reg[instr_decode_rs2(instr)];
}

void RISCV_instr_sltu(RISCV_st *cpu, uint32_t instr)
{
	cpu->reg[instr_decode_rd(instr)] =
		(uint32_t)cpu->reg[instr_decode_rs1(instr)] <
		(uint32_t)cpu->reg[instr_decode_rs2(instr)];
}
void RISCV_instr_xor(RISCV_st *cpu, uint32_t instr)
{
	cpu->reg[instr_decode_rd(instr)] = cpu->reg[instr_decode_rs1(instr)] ^ cpu->reg[instr_decode_rs2(instr)];
}

void RISCV_instr_srl(RISCV_st *cpu, uint32_t instr)
{
	cpu->reg[instr_decode_rd(instr)] =
		(uint32_t)cpu->reg[instr_decode_rs1(instr)] >> 
		(uint32_t)(cpu->reg[instr_decode_rs2(instr)] & 0x1F); // 5 last bits
}

void RISCV_instr_sra(RISCV_st *cpu, uint32_t instr)
{
	uint8_t rd = instr_decode_rd(instr);
	int32_t vrs1 = cpu->reg[instr_decode_rs1(instr)];
	uint32_t vrs2 = cpu->reg[instr_decode_rs2(instr)] & 0x1F; // 5 last bits

	cpu->reg[rd] = (vrs1 < 0)? ~(~vrs1 >> vrs2) : vrs1 >> vrs2;
}

void RISCV_instr_or(RISCV_st *cpu, uint32_t instr)
{
	cpu->reg[instr_decode_rd(instr)] = cpu->reg[instr_decode_rs1(instr)] | cpu->reg[instr_decode_rs2(instr)];
}

void RISCV_instr_and(RISCV_st *cpu, uint32_t instr)
{
	cpu->reg[instr_decode_rd(instr)] = cpu->reg[instr_decode_rs1(instr)] & cpu->reg[instr_decode_rs2(instr)];
}

void RISCV_instr_fence(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_ecall(RISCV_st *cpu, uint32_t instr);
void RISCV_instr_ebreak(RISCV_st *cpu, uint32_t instr);
