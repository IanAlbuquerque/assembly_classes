#include <stdio.h>
#include <stdlib.h>
#include "geracod.h"

#define LARGEST_OPCODE_SIZE 8
#define MAXIMUM_NUMBER_OF_VARIABLES 5
#define MAXIMUM_NUMBER_OF_LINES_OF_CODE 50

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

static int search(int* array, int size, int value)
{
	int i;
	for(i=0; i<size; i++)
		if(value == array[i])
			return i;
	return -1;
}

static void printCodigo(byte* code, int current_byte, int size)
{
	int i;
	for(i=current_byte;i<current_byte+size;i++)
	{
		printf("%02x ",code[i]);
	}
	printf("\n");
}

//================================================================================================================================

static int _movl_value_eax(byte* code, int current_byte, Value value)
{
	// movl $value, %eax
	code[current_byte] = 0xB8;
	code[current_byte+1] = value.b[0];
	code[current_byte+2] = value.b[1];
	code[current_byte+3] = value.b[2];
	code[current_byte+4] = value.b[3];
	printCodigo(code,current_byte,5);
	return current_byte + 5;
}

static int _movl_valueebp_eax(byte* code, int current_byte, byte value)
{
	// movl value(%ebp),%eax
	code[current_byte] = 0x8B;
	code[current_byte+1] = 0x45;
	code[current_byte+2] = value;
	printCodigo(code,current_byte,3);
	return current_byte + 3;
}

static int _movl_ebp_esp(byte* code, int current_byte)
{
	//movl %ebp, %esp
	code[current_byte] = 0x89;
	code[current_byte+1] = 0xEC;
	printCodigo(code,current_byte,2);
	return current_byte + 2;
}

static int _popl_ebp(byte* code, int current_byte)
{
	//popl %ebp
	code[current_byte] = 0x5D;
	printCodigo(code,current_byte,1);
	return current_byte + 1;
}

static int _ret(byte* code, int current_byte)
{
	// ret
	code[current_byte] = 0xC3;
	printCodigo(code,current_byte,1);
	return current_byte + 1;
}

static int _addl_value_eax(byte* code, int current_byte, Value value)
{
	// addl $value,%eax
	code[current_byte] = 0x05;
	code[current_byte+1] = value.b[0];
	code[current_byte+2] = value.b[1];
	code[current_byte+3] = value.b[2];
	code[current_byte+4] = value.b[3];
	printCodigo(code,current_byte,5);
	return current_byte + 5;
}

static int _subl_value_eax(byte* code, int current_byte, Value value)
{
	// subl $value,%eax
	code[current_byte] = 0x2D;
	code[current_byte+1] = value.b[0];
	code[current_byte+2] = value.b[1];
	code[current_byte+3] = value.b[2];
	code[current_byte+4] = value.b[3];
	printCodigo(code,current_byte,5);
	return current_byte + 5;
}

static int _imull_value_eax(byte* code, int current_byte, Value value)
{
	// imull $value,%eax
	code[current_byte] = 0x69;
	code[current_byte+1] = 0xC0;
	code[current_byte+2] = value.b[0];
	code[current_byte+3] = value.b[1];
	code[current_byte+4] = value.b[2];
	code[current_byte+5] = value.b[3];
	printCodigo(code,current_byte,6);
	return current_byte + 6;
}
static int _addl_valueebp_eax(byte* code, int current_byte, byte value)
{
	// addl value(ebp),%eax
	code[current_byte] = 0x03;
	code[current_byte+1] = 0x45;
	code[current_byte+2] = value;
	printCodigo(code,current_byte,3);
	return current_byte + 3;
}

static int _subl_valueebp_eax(byte* code, int current_byte, byte value)
{
	// subl value(ebp),%eax
	code[current_byte] = 0x2B;
	code[current_byte+1] = 0x45;
	code[current_byte+2] = value;
	printCodigo(code,current_byte,3);
	return current_byte + 3;
}

static int _imull_valueebp_eax(byte* code, int current_byte, byte value)
{
	// imull value(ebp),%eax
	code[current_byte] = 0x0F;
	code[current_byte+1] = 0xAF;
	code[current_byte+2] = 0x45;
	code[current_byte+3] = value;
	printCodigo(code,current_byte,4);
	return current_byte + 4;
}

static int _movl_eax_valueebp(byte* code, int current_byte, byte value)
{
	// movl %eax, value(%ebp)
	code[current_byte] = 0x89;
	code[current_byte+1] = 0x45;
	code[current_byte+2] = value;
	printCodigo(code,current_byte,3);
	return current_byte + 3;
}

static int _operate_value_eax(byte* code, int current_byte, Value value, char operation_type)
{
	if(operation_type == '+')
		return _addl_value_eax(code, current_byte, value);
	else if(operation_type == '-')
		return _subl_value_eax(code, current_byte, value);
	else if(operation_type == '*')
		return _imull_value_eax(code, current_byte, value);
	else
		return 0;
}

static int _operate_valueebp_eax(byte* code, int current_byte, byte value, char operation_type)
{
	if(operation_type == '+')
		return _addl_valueebp_eax(code, current_byte, value);
	else if(operation_type == '-')
		return _subl_valueebp_eax(code, current_byte, value);
	else if(operation_type == '*')
		return _imull_valueebp_eax(code, current_byte, value);
	else
		return 0;
}

static int _pushl_ebp(byte* code, int current_byte)
{
	//pushl %ebp
	code[current_byte] = 0x55;

	printCodigo(code,current_byte,1);

	return current_byte + 1;
}
static int _movl_esp_ebp(byte* code, int current_byte)
{
	//movl %esp, %ebp
	code[current_byte] = 0x89;
	code[current_byte+1] = 0xE5;

	printCodigo(code,current_byte,2);

	return current_byte + 2;
}

static int _subl_value_esp(byte* code, int current_byte, Value value)
{
	// subl $value,%esp;
	code[current_byte] = 0x81;
	code[current_byte+1] = 0xEC;
	code[current_byte+2] = value.b[0];
	code[current_byte+3] = value.b[1];
	code[current_byte+4] = value.b[2];
	code[current_byte+5] = value.b[3];

	printCodigo(code,current_byte,6);

	return current_byte + 6;
}

static int _cmpl_value_eax(byte* code, int current_byte, Value value)
{
	// cmpl $value,%eax
	code[current_byte] = 0x3D;
	code[current_byte+1] = value.b[0];
	code[current_byte+2] = value.b[1];
	code[current_byte+3] = value.b[2];
	code[current_byte+4] = value.b[3];

	printCodigo(code,current_byte,5);

	return current_byte + 5;	
}

static int _cmpl_valueebp_eax(byte* code, int current_byte, byte value)
{
	// cmpl value(%ebp),%eax
	code[current_byte] = 0x3B;
	code[current_byte+1] = 0x45;
	code[current_byte+2] = value;

	printCodigo(code,current_byte,3);

	return current_byte + 3;	
}

static int _je_value(byte* code, int current_byte, Value value)
{
	// je value
	code[current_byte] = 0x0F;
	code[current_byte+1] = 0x84;
	code[current_byte+2] = value.b[0];
	code[current_byte+3] = value.b[1];
	code[current_byte+4] = value.b[2];
	code[current_byte+5] = value.b[3];

	printCodigo(code,current_byte,6);

	return current_byte + 6;
}

//================================================================================================================================

static byte ebpOffsetValue(int indexes_of_used_variables[], int* num_of_used_variables, char variable_type, int variable_index)
{
	int index_in_the_array;
	if(variable_type == 'v')
	{
		index_in_the_array = search(indexes_of_used_variables, *num_of_used_variables, variable_index);
		if(index_in_the_array == -1)
		{
			if(*num_of_used_variables == MAXIMUM_NUMBER_OF_VARIABLES)
			{
				printf("Error: More than %d variables used.\n",MAXIMUM_NUMBER_OF_VARIABLES);
				exit(1);
			}
			else
			{
				indexes_of_used_variables[*num_of_used_variables] = variable_index;
				index_in_the_array = *num_of_used_variables;
				(*num_of_used_variables)++;
			}
		}
		return ~(((byte) MAXIMUM_NUMBER_OF_VARIABLES - (byte) index_in_the_array) * 4) + 1;
	}
	else
	{
		return  8 + 4 * (byte) variable_index;
	}
}

static int computeReturn(byte* code, int current_byte, int indexes_of_used_variables[], int* num_of_used_variables, char operand_type, Value operand_value)
{
	if(operand_type == '$')
		current_byte = _movl_value_eax(code, current_byte, operand_value);
	else if(operand_type == 'v' || operand_type == 'p')
		current_byte = _movl_valueebp_eax(code, current_byte, ebpOffsetValue(indexes_of_used_variables, num_of_used_variables, operand_type, operand_value.i));

	current_byte = _movl_ebp_esp(code, current_byte);
	current_byte = _popl_ebp(code, current_byte);
	current_byte = _ret(code, current_byte);

	return current_byte;
}

static int computeOperation(byte* code, int current_byte, int indexes_of_used_variables[], int* num_of_used_variables,
									char operation_type, 
									char destination_operand_type, Value destination_operand_value, 
									char first_operand_type, Value first_operand_value,
									char second_operand_type, Value second_operand_value)
{
	if(first_operand_type == '$')
		current_byte = _movl_value_eax(code, current_byte, first_operand_value);
	else if(first_operand_type == 'v' || first_operand_type == 'p')
		current_byte = _movl_valueebp_eax(code, current_byte, ebpOffsetValue(indexes_of_used_variables, num_of_used_variables, first_operand_type, first_operand_value.i));

	if(second_operand_type == '$')
		current_byte =  _operate_value_eax(code, current_byte, second_operand_value, operation_type);
	else if(second_operand_type == 'v' || second_operand_type == 'p')
		current_byte = _operate_valueebp_eax(code, current_byte, ebpOffsetValue(indexes_of_used_variables, num_of_used_variables, second_operand_type, second_operand_value.i), operation_type);

	current_byte = _movl_eax_valueebp(code, current_byte, ebpOffsetValue(indexes_of_used_variables,num_of_used_variables,destination_operand_type,destination_operand_value.i));

	return current_byte;
}

static int computeHeader(byte* code, int current_byte)
{
	Value _TWENTY;
	_TWENTY.i = 20;

	current_byte = _pushl_ebp(code, current_byte);
	current_byte = _movl_esp_ebp(code, current_byte);
	current_byte = _subl_value_esp(code, current_byte,_TWENTY);

	return current_byte;
}

static int computeIfEq(byte* code, int current_byte, int indexes_of_used_variables[], int* num_of_used_variables, 
						char first_operand_type, Value first_operand_value, 
						char second_operand_type, Value second_operand_value, 
						JumpReference array_of_jump_references[], int jump_destination_line, int current_line_of_code)
{
	Value _ZERO;
	_ZERO.i = 0;

	if(first_operand_type == '$')
		current_byte = _movl_value_eax(code, current_byte, first_operand_value);
	else if(first_operand_type == 'v' || first_operand_type == 'p')
		current_byte = _movl_valueebp_eax(code, current_byte, ebpOffsetValue(indexes_of_used_variables,num_of_used_variables,first_operand_type,first_operand_value.i));

	if(second_operand_type == '$')
		current_byte = _cmpl_value_eax(code, current_byte, second_operand_value);
	else if(second_operand_type == 'v' || second_operand_type == 'p')
		current_byte = _cmpl_valueebp_eax(code, current_byte, ebpOffsetValue(indexes_of_used_variables,num_of_used_variables,second_operand_type,second_operand_value.i));

	array_of_jump_references[current_line_of_code].byte_to_write = current_byte + 2;
	array_of_jump_references[current_line_of_code].jump_line_dest = jump_destination_line;
	current_byte = _je_value(code, current_byte, _ZERO);

	return current_byte;
}

funcp geracod (FILE *f)
{
	byte* code;

	int indexes_of_used_variables[MAXIMUM_NUMBER_OF_VARIABLES];
	int num_of_used_variables = 0;

	JumpReference array_of_jump_references[MAXIMUM_NUMBER_OF_LINES_OF_CODE + 1];
	int correspondent_opcode_location[MAXIMUM_NUMBER_OF_LINES_OF_CODE + 1];
	
	char operation_type;

	char operand_type;
	char destination_operand_type;
	char first_operand_type;
	char second_operand_type;

	Value operand_value;
	Value destination_operand_value;
	Value first_operand_value;
	Value second_operand_value;

	int jump_destination_line;
	
	char first_char_in_line;

	Value opcode_location_difference;

	int current_byte = 0;
	int current_line_of_code = 0;

	int i;
	
	for(i=1; i<=MAXIMUM_NUMBER_OF_LINES_OF_CODE; i++)
	{
		array_of_jump_references[i].byte_to_write = -1;
		array_of_jump_references[i].jump_line_dest = -1;
	}

	code = malloc(MAXIMUM_NUMBER_OF_LINES_OF_CODE * LARGEST_OPCODE_SIZE);

	if(code == NULL)
	{
		printf("Error: Code Area Allocation Error.\n");
		exit(1);
	}
	
	current_byte = computeHeader(code,current_byte);
	
	while(fscanf(f," %c",&first_char_in_line)==1)
	{
		current_line_of_code++;
		correspondent_opcode_location[current_line_of_code] = current_byte;
		printf("\n");
		printf("#%02d (0x%08x) >>> ",current_line_of_code,correspondent_opcode_location[current_line_of_code]);
		printf("%c",first_char_in_line);
		if(first_char_in_line == 'i')
		{
			fscanf(f,"feq %c%d %c%d %d",&first_operand_type,&(first_operand_value.i),&second_operand_type,&(second_operand_value.i),&jump_destination_line);
			printf("feq %c%d %c%d %d\n\n",first_operand_type,first_operand_value.i,second_operand_type,second_operand_value.i,jump_destination_line);
			current_byte = computeIfEq(code, current_byte, indexes_of_used_variables, &num_of_used_variables, first_operand_type, first_operand_value, second_operand_type, second_operand_value, array_of_jump_references, jump_destination_line, current_line_of_code);
		}
		else if(first_char_in_line == 'r')
		{
			fscanf(f,"et %c%d",&operand_type,&operand_value.i);
			printf("et %c%d\n\n",operand_type,operand_value.i);
			if(operand_type == '$' || operand_type == 'v' || operand_type == 'p')
			{
				current_byte = computeReturn(code,current_byte,indexes_of_used_variables,&num_of_used_variables,operand_type,operand_value);
			}
			else
			{
				printf("Error: invalid symbol after 'ret' found.\n");
				exit(1);
			}
		}
		else if(first_char_in_line == 'v' || first_char_in_line == 'p')
		{
			destination_operand_type = first_char_in_line;
			fscanf(f," %d",&destination_operand_value.i);
			printf("%d",destination_operand_value.i);
			fscanf(f," := %c%d",&first_operand_type,(int*) first_operand_value.b);
			printf(" := %c%d",first_operand_type,first_operand_value.i);


			fscanf(f," %c",&operation_type);
			printf(" %c",operation_type);

			fscanf(f," %c%d",&second_operand_type,&(second_operand_value.i));
			printf(" %c%d\n\n",second_operand_type,second_operand_value.i);

			if(first_operand_type != '$' && first_operand_type != 'v' && first_operand_type != 'p')
			{
				printf("Error: invalid symbol after ':=' found.\n");
				exit(1);
			}
			if(second_operand_type != '$' && second_operand_type != 'v' && second_operand_type != 'p')
			{
				printf("Error: invalid symbol after '%c' found.\n", operation_type);
				exit(1);
			}
			if(operation_type != '+' && operation_type != '-' && operation_type != '*')
			{
				printf("Error: Invalid operator '%c'.\n",operation_type);
				exit(1);
			}
			
			current_byte = computeOperation(code, current_byte, indexes_of_used_variables, &num_of_used_variables,
													operation_type,
													destination_operand_type,destination_operand_value,
													first_operand_type, first_operand_value,
													second_operand_type, second_operand_value);

		}
		else
		{
			printf("Error: Unknown expression\n");
			exit(1);
		}
	}

	printf("-------------------------------------------\n");
	printf("Atualizando Referencias dos JUMPS:\n");
	printf("-------------------------------------------\n");

	for(i=1;i<=MAXIMUM_NUMBER_OF_LINES_OF_CODE;i++)
	{
		if(array_of_jump_references[i].byte_to_write != -1)
		{
			printf("jump_destination_line %d (JUMP para jump_destination_line %d)\n",i,array_of_jump_references[i].jump_line_dest);
			opcode_location_difference.i = correspondent_opcode_location[array_of_jump_references[i].jump_line_dest] - (array_of_jump_references[i].byte_to_write+4);
			printf("Antes:\t");
			printCodigo(code,array_of_jump_references[i].byte_to_write-2,6);
			code[array_of_jump_references[i].byte_to_write] = opcode_location_difference.b[0];
			code[array_of_jump_references[i].byte_to_write+1] = opcode_location_difference.b[1];
			code[array_of_jump_references[i].byte_to_write+2] = opcode_location_difference.b[2];
			code[array_of_jump_references[i].byte_to_write+3] = opcode_location_difference.b[3];
			printf("Depois:\t");
			printCodigo(code,array_of_jump_references[i].byte_to_write-2,6);
			printf("\n");
		}
	}

	return (funcp) code;
}

