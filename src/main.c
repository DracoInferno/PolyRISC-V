#include <stdlib.h>
#include <stdio.h>
#include "PolyRISC-V.h"

int main(int argc, char *argv[])
{
	RISCV_st *cpu = NULL;
	RISCV_init_op_st iop = {1024, 512, false};
	
	cpu = RISCV_init(&iop);
	if(!cpu)
		return EXIT_FAILLURE;

	RISCV_reset(cpu, code, code_size, NULL);

	RISCV_deinit(cpu);
	return EXIT_SUCCESS;
}
