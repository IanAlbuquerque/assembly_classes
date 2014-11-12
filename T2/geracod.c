#include <stdio.h>
#include <stdlib.h>
#include "geracod.h"

#define TAM_MAIOR_INSTRUCAO 8
#define MAX_VARS 5

//typedef int (*funcp) ();

struct jumpReference
{
	int byte_to_write;
	int jump_line_dest;
};

typedef struct jumpReference JumpReference;

typedef unsigned char byte;

union value
{
	int i;
	byte b[4];
};

typedef union value Value;

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
	int i;
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

			printf("ATENCAO V%d NUNCA FOI USADO\n",var_num);
			for (i = 0; i < 5; ++i)
			{
				printf("[%d]",used_vs[i]);
			}
			printf("\n");

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

/* NOVO NEW NOUVEAU*/
int valorAdicionarEbp( int param_num ) 
{
	return 8 + 4*param_num; //para mim só faz sentido usar params 0,1,2,3,4...Tratar que o cara use os parametros
}				//2,4,5,7,9 me parece bizarro...mas vc é aquariano.
/* NOVO NEW NOUVEAU */

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

int computeFirstOperation(byte* codigo, int byte_atual, int* used_vs, int* amount_used_vs, char simbolo, Value valor_simbolo)
{
	if(simbolo == '$')
	{
		// movl $valor,%eax
		codigo[byte_atual] = 0xB8;
		codigo[byte_atual+1] = valor_simbolo.b[0];
		codigo[byte_atual+2] = valor_simbolo.b[1];
		codigo[byte_atual+3] = valor_simbolo.b[2];
		codigo[byte_atual+4] = valor_simbolo.b[3];
		printf("%d\n",valor_simbolo.i);
		printCodigo(codigo,byte_atual,5);
		byte_atual += 5;
	}
	else if(simbolo == 'v' || simbolo == 'p')
	{
		// movl valor(%ebp),%eax
		codigo[byte_atual] = 0x8B;
		codigo[byte_atual+1] = 0x45;
		if(simbolo == 'v') codigo[byte_atual+2] = ~valorSubtrairEbp(NULL,used_vs,amount_used_vs,valor_simbolo.i,codigo,&byte_atual)+1;
		else codigo[byte_atual+2] = valorAdicionarEbp(valor_simbolo.i);
		printCodigo(codigo,byte_atual,3);
		byte_atual += 3;
	}

	return byte_atual;
}

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

/* NOVO NEW NOUVEAU*/
int computeFirstOperationParam(FILE* f, byte* codigo, int byte_atual /*, int* used_params, int* amount_used_params */ ) 
{
	//byte valor_adicionar_ebp;
	int param_num;

	fscanf(f," %d", &param_num);
	printf("%d\n", param_num);


	// movl valor_adicionar_ebp(%ebp),%eax 8b 45 0c
	codigo[byte_atual] = 0x8B;
	codigo[byte_atual+1] = 0x45;
	codigo[byte_atual+2] = valorAdicionarEbp( param_num ); ;
	printCodigo(codigo,byte_atual,3);
	byte_atual += 3;

	return byte_atual;
}
/* NOVO NEW NOUVEAU*/

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

/* NOVO NEW NOUVEAU*/
int computeSecondOperationVar(FILE* f, byte* codigo, int byte_atual, int* used_vs, int* amount_used_vs, int var_num, char operacao)
{
	byte valor_subtrair_ebp_dest;
	byte valor_subtrair_ebp_direita;
	int var_num_dir;

	fscanf(f," %d", &var_num_dir);
	printf("%d\n", var_num_dir);

	valor_subtrair_ebp_dest = valorSubtrairEbp(f,used_vs,amount_used_vs,var_num,codigo,&byte_atual);
	valor_subtrair_ebp_direita = valorSubtrairEbp(f,used_vs,amount_used_vs,var_num_dir,codigo,&byte_atual);
	
	if(operacao == '+')
	{
		// addl -valor_subtrair_ebp_direita(%ebp),%eax
		codigo[byte_atual] = 0x03;
		codigo[byte_atual+1] = 0x45;
		codigo[byte_atual+2] = ~valor_subtrair_ebp_direita + 1;
		printCodigo(codigo,byte_atual,3);
		byte_atual += 3;
	}
	else if(operacao == '-')
	{
		// subl -valor_subtrair_ebp_direita(%ebp),%eax
		codigo[byte_atual] = 0x2B;
		codigo[byte_atual+1] = 0x45;
		codigo[byte_atual+2] = ~valor_subtrair_ebp_direita + 1;
		printCodigo(codigo,byte_atual,3);
		byte_atual += 3;
	}
	else if(operacao == '*')
	{
		// imull -valor_subtrair_ebp_direita(%ebp),%eax
		codigo[byte_atual] = 0x0F;
		codigo[byte_atual+1] = 0xAF;
		codigo[byte_atual+2] = 0x45;
		codigo[byte_atual+3] = ~valor_subtrair_ebp_direita + 1;
		printCodigo(codigo,byte_atual,4);
		byte_atual += 4;
	}
	else
	{
		printf("Error: invalid operator '%c'.\n",operacao);
		exit(1);
	}

	//movl %eax,-valor_subtrair_ebp(%ebp)
	codigo[byte_atual] = 0x89;
	codigo[byte_atual+1] = 0x45;
	codigo[byte_atual+2] = ~valor_subtrair_ebp_dest + 1;
	printCodigo(codigo,byte_atual,3);
	byte_atual += 3;


	return byte_atual;
}

/* NOVO NEW NOUVEAU*/
int computeSecondOperationParam(FILE* f, byte* codigo, int byte_atual, int* used_vs, int* amount_used_vs, int var_num, char operacao)
{
	byte valor_subtrair_ebp_dest;
	byte valor_adicionar_ebp_direita;
	int param_num_dir;

	fscanf(f," %d", &param_num_dir);
	printf("%d\n", param_num_dir);

	valor_subtrair_ebp_dest = valorSubtrairEbp(f,used_vs,amount_used_vs,var_num,codigo,&byte_atual);
	valor_adicionar_ebp_direita = valorAdicionarEbp( param_num_dir );
	
	if(operacao == '+')
	{
		// addl valor_adicionar_ebp_direita(%ebp),%eax
		codigo[byte_atual] = 0x03;
		codigo[byte_atual+1] = 0x45;
		codigo[byte_atual+2] = valor_adicionar_ebp_direita;
		printCodigo(codigo,byte_atual,3);
		byte_atual += 3;
	}
	else if(operacao == '-')
	{
		// subl valor_adicionar_ebp_direita(%ebp),%eax
		codigo[byte_atual] = 0x2B;
		codigo[byte_atual+1] = 0x45;
		codigo[byte_atual+2] = valor_adicionar_ebp_direita;
		printCodigo(codigo,byte_atual,3);
		byte_atual += 3;
	}
	else if(operacao == '*')
	{
		// imull valor_adicionar_ebp_direita(%ebp),%eax
		codigo[byte_atual] = 0x0F;
		codigo[byte_atual+1] = 0xAF;
		codigo[byte_atual+2] = 0x45;
		codigo[byte_atual+3] = valor_adicionar_ebp_direita;
		printCodigo(codigo,byte_atual,4);
		byte_atual += 4;
	}
	else
	{
		printf("Error: invalid operator '%c'.\n",operacao);
		exit(1);
	}

	//movl %eax,-valor_adicionar_ebp(%ebp)
	codigo[byte_atual] = 0x89;
	codigo[byte_atual+1] = 0x45;
	codigo[byte_atual+2] = ~valor_subtrair_ebp_dest + 1;
	printCodigo(codigo,byte_atual,3);
	byte_atual += 3;


	return byte_atual;
}
/* NOVO NEW NOUVEAU*/

int addCabecalho(FILE* f, byte* codigo, int byte_atual)
{
	//pushl %ebp
	codigo[byte_atual] = 0x55;
	printCodigo(codigo,byte_atual,1);
	byte_atual+=1;
	
	//movl %esp, %ebp
	codigo[byte_atual] = 0x89;
	codigo[byte_atual+1] = 0xE5;
	printCodigo(codigo,byte_atual,2);
	byte_atual+=2;
	
	// subl $20,%esp;
	codigo[byte_atual] = 0x83;
	codigo[byte_atual+1] = 0xEC;
	codigo[byte_atual+2] = 0x14;
	printCodigo(codigo,byte_atual,3);
	byte_atual+=3;

	return byte_atual;
}

int computeIfEq(byte* codigo, int byte_atual, int* used_vs, int* amount_used_vs, char char1, Value v1, char char2, Value v2, JumpReference ref_jump[], int linha, int linha_atual)
{
	if(char1 == '$')
	{
		// movl $valor,%eax
		codigo[byte_atual] = 0xB8;
		codigo[byte_atual+1] = v1.b[0];
		codigo[byte_atual+2] = v1.b[1];
		codigo[byte_atual+3] = v1.b[2];
		codigo[byte_atual+4] = v1.b[3];
		printf("%d\n",v1.i);
		printCodigo(codigo,byte_atual,5);
		byte_atual += 5;
	}
	else if(char1 == 'v' || char1 == 'p')
	{
		// movl valor(%ebp),%eax
		codigo[byte_atual] = 0x8B;
		codigo[byte_atual+1] = 0x45;
		if(char1 == 'v') codigo[byte_atual+2] = ~valorSubtrairEbp(NULL,used_vs,amount_used_vs,v1.i,codigo,&byte_atual)+1;
		else codigo[byte_atual+2] = valorAdicionarEbp(v1.i);
		printCodigo(codigo,byte_atual,3);
		byte_atual += 3;
	}

	if(char2 == '$')
	{
		// movl $valor,%eax
		codigo[byte_atual] = 0x3D;
		codigo[byte_atual+1] = v2.b[0];
		codigo[byte_atual+2] = v2.b[1];
		codigo[byte_atual+3] = v2.b[2];
		codigo[byte_atual+4] = v2.b[3];
		printf("%d\n",v2.i);
		printCodigo(codigo,byte_atual,5);
		byte_atual += 5;
	}
	else if(char2 == 'v' || char2 == 'p')
	{
		// movl valor(%ebp),%eax
		codigo[byte_atual] = 0x3B;
		codigo[byte_atual+1] = 0x45;
		if(char2 == 'v') codigo[byte_atual+2] = ~valorSubtrairEbp(NULL,used_vs,amount_used_vs,v2.i,codigo,&byte_atual)+1;
		else codigo[byte_atual+2] = valorAdicionarEbp(v2.i);
		printCodigo(codigo,byte_atual,3);
		byte_atual += 3;
	}

	codigo[byte_atual] = 0x0F;
	codigo[byte_atual+1] = 0x84;
	ref_jump[linha_atual].byte_to_write = byte_atual+2;
	ref_jump[linha_atual].jump_line_dest = linha;
	printCodigo(codigo,byte_atual,6);
	byte_atual+=6;

	return byte_atual;
}

funcp geracod (FILE *f)
{
	byte* codigo;
	int local_linha[51];
	JumpReference ref_jump[51];
	int linha_atual;

	int i;
	
	int var_num;
	
	int used_vs[5];
	int amount_used_vs = 0;
	
	char char_lido;
	Value valor_lido;
	char operacao;
	int byte_atual = 0;

	char char1, char2;
	Value v1, v2;
	int linha;

	Value line_diference;
	
	for(i=0;i<51;i++)
	{
		ref_jump[i].byte_to_write = -1;
		ref_jump[i].jump_line_dest = -1;
	}

	codigo = malloc(50*TAM_MAIOR_INSTRUCAO);

	if(codigo == NULL)
	{
		printf("Code Area Allocation Error\n");
		exit(1);
	}
	
	byte_atual = addCabecalho(f,codigo,byte_atual);
	
	linha_atual = 0;
	while(fscanf(f," %c",&char_lido)==1)
	{
		printf("%c",char_lido);
		linha_atual++;
		local_linha[linha_atual] = byte_atual;
		printf("Linha %d esta no byte %d\n",linha_atual,local_linha[linha_atual]);
		if(char_lido == 'i')
		{
			fscanf(f,"feq %c%d %c%d %d",&char1,&(v1.i),&char2,&(v2.i),&linha);
			printf("feq %c%d %c%d %d\n",char1,v1.i,char2,v2.i,linha);
			byte_atual = computeIfEq(codigo, byte_atual, used_vs, &amount_used_vs, char1, v1, char2, v2, ref_jump, linha, linha_atual);
		}
		else if(char_lido == 'r')
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
			fscanf(f," := %c%d",&char_lido,(int*) valor_lido.b);
			printf(" := %c%d",char_lido,valor_lido.i);
			/*
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
			else if(char_lido == 'p')
			{
				printf("!!!!!!!!chegou aqui");
				byte_atual = computeFirstOperationParam( f,codigo , byte_atual );
				
			}
			*/

			/* NOVO NEW NOUVEAU*/
			if(char_lido == '$' || char_lido == 'v' || char_lido == 'p')
			{
				byte_atual = computeFirstOperation(codigo, byte_atual, used_vs, &amount_used_vs, char_lido, valor_lido);
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
				byte_atual = computeSecondOperationVar(f,codigo,byte_atual,used_vs,&amount_used_vs,var_num,operacao);
			}
			else if(char_lido == 'p')
			{
				byte_atual = computeSecondOperationParam(f,codigo,byte_atual,used_vs,&amount_used_vs,var_num,operacao);
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

	for(i=0;i<51;i++)
	{
		if(ref_jump[i].byte_to_write != -1)
		{
			printf("Atualizando JUMPS\n");
			line_diference.i = local_linha[ref_jump[i].jump_line_dest] - (ref_jump[i].byte_to_write+4);
			printCodigo(codigo,ref_jump[i].byte_to_write-2,6);
			codigo[ref_jump[i].byte_to_write] = line_diference.b[0];
			codigo[ref_jump[i].byte_to_write+1] = line_diference.b[1];
			codigo[ref_jump[i].byte_to_write+2] = line_diference.b[2];
			codigo[ref_jump[i].byte_to_write+3] = line_diference.b[3];
			printCodigo(codigo,ref_jump[i].byte_to_write-2,6);
		}
	}

	return (funcp) codigo;
}

