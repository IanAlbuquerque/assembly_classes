#include <stdio.h>
#include <stdlib.h>
#include "geracod.h"

funcp code;
	
int main(int argn, char* argc[])
{
	FILE* input_file;
	int number_of_parameters;
	int param[5];
	int valor_retorno;
	int i;

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

	printf("Enter the number of paramaters:\n");
	scanf("%d",&number_of_parameters);
	if(number_of_parameters > 5 || number_of_parameters < 0)
	{
		printf("Error: Too many parameters (min 0 / max 5).\n");
		exit(1);
	}
	printf("Enter the values of the parameters:\n");
	for(i=0;i<number_of_parameters;i++)
	{
		scanf(" %d",&param[i]);
	}

	if(number_of_parameters == 0)
	{
		valor_retorno = code();
	}
	else if(number_of_parameters == 1)
	{
		valor_retorno = code(param[0]);
	}
	else if(number_of_parameters == 2)
	{
		valor_retorno = code(param[0],param[1]);
	}
	else if(number_of_parameters == 3)
	{
		valor_retorno = code(param[0],param[1],param[2]);
	}
	else if(number_of_parameters == 4)
	{
		valor_retorno = code(param[0],param[1],param[2],param[3]);
	}
	else if(number_of_parameters == 5)
	{
		valor_retorno = code(param[0],param[1],param[2],param[3],param[4]);
	}
	
	printf("A funcao retornou %d\n",valor_retorno);
	
	fclose(input_file);
	
	return 0;
}
