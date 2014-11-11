#include <stdio.h>
#include <stdlib.h>
#include "geracod.h"

#define TAM_MAIOR_INSTRUCAO 8
#define MAX_VARS 5

//typedef int (*funcp) ();

typedef unsigned char byte;

int search(int* v, int n, int x)
{
	int i;
	for(i=0;i<n;i++)
	{
		if(x==v[i])
		{
			return i;
		}
	}
	return -1;
}

void printCodigo(byte* codigo, int byte_atual, int size)
{
	int i;
	printf("Codigo de Maquina Gerado= ");
	for(i=byte_atual;i<byte_atual+size;i++)
	{
		printf("%02x ",codigo[i]);
	}
	printf("\n");
}

funcp geracod (FILE *f)
{
	byte* codigo;
	
	//int i;
	
	int used_vs[5];
	int amount_used_vs = 0;
	
	char first_char;
	int byte_atual = 0;
	
	int var_num;
	int var_index;
	
	byte valor_subtrair_ebp;
	
	codigo = malloc(50*TAM_MAIOR_INSTRUCAO);
	if(codigo == NULL)
	{
		printf("Code Area Allocation Error\n");
		exit(1);
	}
	
	//pushl %ebp
	codigo[byte_atual] = 0x55;
	byte_atual+=1;
	
	//movl %esp, %ebp
	codigo[byte_atual] = 0x89;
	codigo[byte_atual+1] = 0xE5;
	byte_atual+=2;
	
	// subl $20,%esp;
	codigo[byte_atual] = 0x83;
	codigo[byte_atual+1] = 0xEC;
	codigo[byte_atual+2] = 0x14;
	byte_atual+=3;
	
	while(fscanf(f," %c",&first_char)==1)
	{
		printf("Carater Lido=%c\n",first_char);
		if(first_char == 'r')
		{
			// movl $valor,%eax
			codigo[byte_atual] = 0xB8;
			fscanf(f,"et $%d",(int*) &codigo[byte_atual+1]);
			printCodigo(codigo,byte_atual,5);
			byte_atual += 5;

			//movl %ebp, %esp
			codigo[byte_atual] = 0x89;
			codigo[byte_atual+1] = 0xEC;
			printCodigo(codigo,byte_atual,2);
			byte_atual+=2;
			
			//popl %ebp
			codigo[byte_atual] = 0x5D;
			printCodigo(codigo,byte_atual,1);
			byte_atual+=1;
			
			// ret
			codigo[byte_atual] = 0xC3;
			printCodigo(codigo,byte_atual,1);
			byte_atual += 1;
		}
		else if(first_char == 'v')
		{
			fscanf(f,"%d",&var_num);
			var_index = search(used_vs,amount_used_vs,var_num);
			if(var_index == -1)
			{
				if(amount_used_vs == MAX_VARS)
				{
					printf("More than 5 vars used.\n");
					exit(1);
				}
				else
				{
					used_vs[amount_used_vs] = var_num;
					var_index=amount_used_vs;
					amount_used_vs++;
				}
			}
			valor_subtrair_ebp = ((byte)MAX_VARS - (byte)var_index)*4; // bytes
			
			// movl $valor,-valor_subtrair_ebp(%ebp)
			codigo[byte_atual] = 0xC7;
			codigo[byte_atual+1] = 0x45; 
			codigo[byte_atual+2] = ~valor_subtrair_ebp + 1;
			fscanf(f," := $%d",(int*) &codigo[byte_atual+3]);
			printCodigo(codigo,byte_atual,7);
			byte_atual += 7;

		}
		else
		{
			printf("Unknown expression\n");
			exit(1);
		}
	}
	return (funcp) codigo;
}

