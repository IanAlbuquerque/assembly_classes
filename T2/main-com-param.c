#include <stdio.h>
#include <stdlib.h>
#include "geracod.h"
#include <string.h>
funcp code;
	
	
int main(int argn, char* argc[])
{
	FILE* input_file;
	int param[5];
	
	
	if(argn != 4) //testando parametros
	{
		printf("mincc <input file name> <param0> <param1> \n");
		exit(1);
	}
	input_file = fopen(argc[1],"r");
	if(input_file == NULL)
	{
		printf("Error opening input file.\n");
		exit(1);
	}
	
	param[0] = atoi(argc[2]);
	param[1] = atoi(argc[3]);
	
	printf("%d %d\n\n",param[0],param[1]);
	code = geracod(input_file);
	
	printf("Compilacao Concluida\n");
	
	printf("A funcao retornou %d\n",code(param[0],param[1]));
	
	fclose(input_file);
	
	return 0;
}
