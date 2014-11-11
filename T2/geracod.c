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
	printf(">>>>>>> ");
	for(i=byte_atual;i<byte_atual+size;i++)
	{
		printf("%02x ",codigo[i]);
	}
	printf("\n");
}

int computeReturnConst(FILE* f, byte* codigo,int byte_atual)
{
	// movl $valor,%eax
	codigo[byte_atual] = 0xB8;
	fscanf(f," %d",(int*) &codigo[byte_atual+1]);
	printf("[%d]\n",codigo[byte_atual+1]);
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

	return byte_atual;
}

int valorSubtrairEbp(FILE* f, int* used_vs, int* amount_used_vs, int var_num, byte* codigo, int* byte_atual)
{
	int var_index;
	var_index = search(used_vs,*amount_used_vs,var_num);
	if(var_index == -1)
	{
		if(*amount_used_vs == MAX_VARS)
		{
			printf("Error: More than 5 vars used.\n");
			exit(1);
		}
		else
		{
			used_vs[*amount_used_vs] = var_num;
			var_index=*amount_used_vs;
			(*amount_used_vs)++;

			// Inicializa Variáveis Não Atribuídas Antes como Zero
			// movl $0,-valor_subtrair_ebp(%ebp)
			codigo[*byte_atual] = 0xC7;
			codigo[*byte_atual+1] = 0x45; 
			codigo[*byte_atual+2] = ~(((byte)MAX_VARS - (byte)var_index)*4) + 1;
			codigo[*byte_atual+3] = 0; 
			codigo[*byte_atual+4] = 0; 
			codigo[*byte_atual+5] = 0; 
			codigo[*byte_atual+6] = 0; 
			printCodigo(codigo,*byte_atual,7);
			*byte_atual += 7;
		}
	}
	return ((byte)MAX_VARS - (byte)var_index)*4;
}

int computeReturnVar(FILE* f, byte* codigo, int byte_atual, int* used_vs, int* amount_used_vs, int var_num)
{
	byte valor_subtrair_ebp;
	valor_subtrair_ebp = valorSubtrairEbp(f,used_vs,amount_used_vs,var_num,codigo,&byte_atual);

	// movl -valor_subtrair_ebp(%ebp),%eax
	codigo[byte_atual] = 0x8B;
	codigo[byte_atual+1] = 0x45;
	codigo[byte_atual+2] = ~valor_subtrair_ebp + 1;
	printCodigo(codigo,byte_atual,3);
	byte_atual += 3;

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

	return byte_atual;
}

/*
int computeAttConst(FILE* f, byte* codigo, int byte_atual, int* used_vs, int* amount_used_vs, int var_num)
{
	byte valor_subtrair_ebp;
	valor_subtrair_ebp = valorSubtrairEbp(f,used_vs,amount_used_vs,var_num,codigo,&byte_atual);
	
	// movl $valor,-valor_subtrair_ebp(%ebp)
	codigo[byte_atual] = 0xC7;
	codigo[byte_atual+1] = 0x45; 
	codigo[byte_atual+2] = ~valor_subtrair_ebp + 1;
	fscanf(f," %d",(int*) &codigo[byte_atual+3]);
	printf("[%d]\n",codigo[byte_atual+3]);
	printCodigo(codigo,byte_atual,7);
	byte_atual += 7;

	return byte_atual;
}
*/

int computeFirstOperationConst(FILE* f, byte* codigo, int byte_atual)
{
	// movl $valor,%eax
	codigo[byte_atual] = 0xB8;
	fscanf(f," %d",(int*) &codigo[byte_atual+1]);
	printf("[%d]\n",codigo[byte_atual+1]);
	printCodigo(codigo,byte_atual,5);
	byte_atual += 5;

	return byte_atual;
}
int computeFirstOperationVar(FILE* f, byte* codigo, int byte_atual, int* used_vs, int* amount_used_vs)
{
	byte valor_subtrair_ebp;
	int var_num;

	fscanf(f," %d", &var_num);
	printf("%d", var_num);

	valor_subtrair_ebp = valorSubtrairEbp(f,used_vs,amount_used_vs,var_num,codigo,&byte_atual);

	// movl -valor_subtrair_ebp(%eax),%eax
	codigo[byte_atual] = 0x8B;
	codigo[byte_atual+1] = 0x45;
	codigo[byte_atual+2] = ~valor_subtrair_ebp+1;
	printCodigo(codigo,byte_atual,3);
	byte_atual += 3;

	return byte_atual;
}

int computeSecondOperationConst(FILE* f, byte* codigo, int byte_atual, int* used_vs, int* amount_used_vs, int var_num, char operacao)
{
	byte valor_subtrair_ebp;
	valor_subtrair_ebp = valorSubtrairEbp(f,used_vs,amount_used_vs,var_num,codigo,&byte_atual);
	
	if(operacao == '+')
	{
		// addl $valor,%eax
		codigo[byte_atual] = 0x05;
		fscanf(f," %d",(int*) &codigo[byte_atual+1]);
		printf("[%d]\n",codigo[byte_atual+1]);
		printCodigo(codigo,byte_atual,5);
		byte_atual += 5;
	}
	else if(operacao == '-')
	{
		// subl $valor,%eax
		codigo[byte_atual] = 0x2D;
		fscanf(f," %d",(int*) &codigo[byte_atual+1]);
		printf("[%d]\n",codigo[byte_atual+1]);
		printCodigo(codigo,byte_atual,5);
		byte_atual += 5;
	}
	else if(operacao == '*')
	{
		// imull $valor,%eax
		codigo[byte_atual] = 0x69;
		codigo[byte_atual+1] = 0xC0;
		fscanf(f," %d",(int*) &codigo[byte_atual+2]);
		printf("[%d]\n",codigo[byte_atual+2]);
		printCodigo(codigo,byte_atual,6);
		byte_atual += 6;
	}
	else
	{
		printf("Error: invalid operator '%c'.\n",operacao);
		exit(1);
	}

	//movl %eax,-valor_subtrair_ebp(%ebp)
	codigo[byte_atual] = 0x89;
	codigo[byte_atual+1] = 0x45;
	codigo[byte_atual+2] = ~valor_subtrair_ebp + 1;
	printCodigo(codigo,byte_atual,3);
	byte_atual += 3;


	return byte_atual;
}

int addCabecalho(FILE* f, byte* codigo, int byte_atual)
{
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

	return byte_atual;
}

funcp geracod (FILE *f)
{
	byte* codigo;
	
	int var_num;
	
	int used_vs[5];
	int amount_used_vs = 0;
	
	char char_lido;
	char operacao;
	int byte_atual = 0;
	
	codigo = malloc(50*TAM_MAIOR_INSTRUCAO);
	if(codigo == NULL)
	{
		printf("Code Area Allocation Error\n");
		exit(1);
	}
	
	byte_atual = addCabecalho(f,codigo,byte_atual);
	
	while(fscanf(f," %c",&char_lido)==1)
	{
		printf("%c",char_lido);
		if(char_lido == 'r')
		{
			fscanf(f,"et %c",&char_lido);
			printf("et %c",char_lido);
			if(char_lido == '$')
			{
				byte_atual = computeReturnConst(f,codigo,byte_atual);
			}
			else if(char_lido == 'v')
			{
				fscanf(f," %d",&var_num);
				printf("%d\n",var_num);
				byte_atual = computeReturnVar(f,codigo,byte_atual,used_vs,&amount_used_vs,var_num);
			}
			else
			{
				printf("Error: invalid symbol after 'ret' found.\n");
				exit(1);
			}
		}
		else if(char_lido == 'v')
		{
			fscanf(f," %d",&var_num);
			printf("%d",var_num);
			fscanf(f," := %c",&char_lido);
			printf(" := %c",char_lido);
			if(char_lido == '$')
			{
				// Atribuição de Constantes a Variáveis única
				// byte_atual = computeAttConst(f,codigo,byte_atual,used_vs,&amount_used_vs,var_num);
				byte_atual = computeFirstOperationConst(f,codigo,byte_atual);
			}
			else if(char_lido == 'v')
			{
				byte_atual = computeFirstOperationVar(f,codigo,byte_atual,used_vs,&amount_used_vs);
			}
			else
			{
				printf("Error: invalid symbol after ':=' found.\n");
				exit(1);
			}

			fscanf(f," %c",&operacao);
			printf(" %c",operacao);

			fscanf(f," %c",&char_lido);
			printf(" %c",char_lido);
			if(char_lido == '$')
			{
				byte_atual = computeSecondOperationConst(f,codigo,byte_atual,used_vs,&amount_used_vs,var_num,operacao);
			}
			else if(char_lido == 'v')
			{
				printf("encontrou v");
			}
			else
			{
				printf("Error: invalid symbol after '%c' found.\n",operacao);
				exit(1);
			}
		}
		else
		{
			printf("Error: Unknown expression\n");
			exit(1);
		}
	}
	return (funcp) codigo;
}

