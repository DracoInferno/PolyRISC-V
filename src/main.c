#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "PolyRISC-V.h"

int main(int argc, char *argv[])
{
	RISCV_st *cpu = NULL;
	RISCV_init_op_st iop = {1024, 512, false};
	uint8_t *code = NULL;
	size_t code_size = 0;


	(void)argc;
	(void)argv;

	cpu = RISCV_init(&iop);
	if(!cpu)
		return EXIT_FAILURE;

	code = malloc(sizeof(uint8_t)*256);
	((uint32_t*)code)[0] = 0xFB000113;
	code_size = 4;
	

	RISCV_reset(cpu, code, code_size, NULL);

	RISCV_step(cpu);

	RISCV_print_reg(cpu);

	RISCV_deinit(cpu);
	return EXIT_SUCCESS;
}
