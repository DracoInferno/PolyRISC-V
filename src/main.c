#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include "PolyRISC-V.h"

#define INPUT_BUFFER_SIZE 256

void interactive_run(RISCV_st *cpu);
void interactive_run_help(void);

int main(int argc, char *argv[])
{
	int status = EXIT_SUCCESS;
	RISCV_st *cpu = NULL;
	RISCV_init_op_st iop = {1024, 512, false};
	uint8_t *code = NULL;
	size_t code_size = 0;
	char *fraw_def_path = "./bin/rawriscv";
	char *fraw_path = fraw_def_path;
	FILE *fraw = NULL;

	if(argc > 1)
		fraw_path = argv[1];

	fraw = fopen(fraw_path, "r");
	if(!fraw){
		fprintf(stderr, "Cannot open the file. Path: %s\nErrno: %s\n", fraw_path, strerror(errno));
		status = EXIT_FAILURE;
		goto deinit;
	}

	fseek(fraw, 0, SEEK_END);
	code_size = ftell(fraw);
	fseek(fraw, 0, SEEK_SET);
	
	code = malloc(code_size);
	if(!code){
		fprintf(stderr, "Could not allocate %lu bytes for riscv instr code.\n", code_size);
		status = EXIT_FAILURE;
		goto deinit;
	}
	
	fread(code, code_size, 1, fraw);
	fclose(fraw);
	fraw = NULL;

	cpu = RISCV_init(&iop);
	if(!cpu){
		fprintf(stderr, "Error initializing RISCV cpu.\n");
		status = EXIT_FAILURE;
		goto deinit;
	}

	RISCV_load_raw_program(cpu, code, code_size);
	RISCV_reset(cpu);
	interactive_run(cpu);

deinit:
	if(cpu){
		RISCV_deinit(cpu);
		cpu = NULL;
	}	

	if(code){
		free(code);
		code = NULL;
	}

	if(fraw){
		fclose(fraw);
		fraw = NULL;
	}

	return status;
}

void interactive_run(RISCV_st *cpu)
{
	char cmd = 0;
	// char buffer[INPUT_BUFFER_SIZE] = {0};

	assert(cpu);

	while(cmd != 'q'){
		if(cmd != '\n')
			printf("cmd: (r, s, q, p)? ");
		scanf("%c", &cmd);
		switch(cmd){
			case 'r':{
				RISCV_reset(cpu);
			 }break;
			case 's':{
				RISCV_step(cpu);
			 }break;
			case 'p':{
				RISCV_print_reg(cpu);
				RISCV_print_pc(cpu);
			 }break;
			case 'q':{
				// Do nothing, just stop the loop
			 }break;
			default:
				;
				// Do nothing
		}
	}

}

void interactive_run_help(void)
{
	printf("Usage: [cmd] [OPTION]...\n");
	printf("cmd: r (reset), s (step), q (quit), p (print)\n");
	printf("print options: \n");
	printf("\tc\tPC\n");
	printf("\ti\tNext instruction\n");
}
