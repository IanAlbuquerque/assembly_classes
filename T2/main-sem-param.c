#include <stdio.h>
#include <stdlib.h>
#include "geracod.h"

funcp code;
	
int main(int argn, char* argc[])
{
	FILE* input_file;

	if(argn != 2)
	{
		printf("mincc <input file name>\n");
		exit(1);
	}
	input_file = fopen(argc[1],"r");
	if(input_file == NULL)
	{
		printf("Error opening input file.\n");
		exit(1);
	}
	
	code = geracod(input_file);
	
	printf("Compilacao Concluida\n");
	
	printf("A funcao retornou %d\n",code());
	
	fclose(input_file);
	
	return 0;
}
