#include <stdio.h>

int main(int arg, char *argv[]){
	printf("Toto\n");

	FILE *f = fopen("./truc.txt", "w+");
	fprintf(f, "hello");
	fclose(f);

	return 0;
}
