#include <stdio.h>
#include <stdlib.h>
#include "geracod.h"

#define LARGEST_OPCODE_SIZE 8
#define MAXIMUM_NUMBER_OF_VARIABLES 5
#define MAXIMUM_NUMBER_OF_LINES_OF_CODE 50

struct jumpReference
{
	// Byte to update with the correct relative jump address
	int byte_to_write;
	// Destination line for the jump
	int jump_line_dest;
};

typedef struct jumpReference JumpReference;

typedef unsigned char byte;


// Union used for accessing of the integer's bytes, regardless of a little-endian memory.
union value
{
	int i;
	byte b[4];
};

typedef union value Value;

// Searches for a value in an array with size 'size'
// Returns the index where the value is, or -1 if the value is not present in the array.
static int search(int* array, int size, int value)
{
	int i;
	for(i=0; i<size; i++)
		if(value == array[i])
			return i;
	return -1;
}

//================================================================================================================================
// All the functions below save in the array 'code' the corresponding opcode of the assembly operation
// They will save the corresponding opcode in the byte which number was passed by 'current_byte'
// Some operations may use a value as a parameter
// All of the functions below return the number of the next byte to be written in the array, immediately after the opcode processed.
//================================================================================================================================

static int _movl_value_eax(byte* code, int current_byte, Value value)
{
	// movl $value, %eax
	code[current_byte] = 0xB8;
	code[current_byte+1] = value.b[0];
	code[current_byte+2] = value.b[1];
	code[current_byte+3] = value.b[2];
	code[current_byte+4] = value.b[3];
	return current_byte + 5;
}

static int _movl_valueebp_eax(byte* code, int current_byte, byte value)
{
	// movl value(%ebp),%eax
	code[current_byte] = 0x8B;
	code[current_byte+1] = 0x45;
	code[current_byte+2] = value;
	return current_byte + 3;
}

static int _movl_ebp_esp(byte* code, int current_byte)
{
	//movl %ebp, %esp
	code[current_byte] = 0x89;
	code[current_byte+1] = 0xEC;
	return current_byte + 2;
}

static int _popl_ebp(byte* code, int current_byte)
{
	//popl %ebp
	code[current_byte] = 0x5D;
	return current_byte + 1;
}

static int _ret(byte* code, int current_byte)
{
	// ret
	code[current_byte] = 0xC3;
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
	return current_byte + 6;
}
static int _addl_valueebp_eax(byte* code, int current_byte, byte value)
{
	// addl value(ebp),%eax
	code[current_byte] = 0x03;
	code[current_byte+1] = 0x45;
	code[current_byte+2] = value;
	return current_byte + 3;
}

static int _subl_valueebp_eax(byte* code, int current_byte, byte value)
{
	// subl value(ebp),%eax
	code[current_byte] = 0x2B;
	code[current_byte+1] = 0x45;
	code[current_byte+2] = value;
	return current_byte + 3;
}

static int _imull_valueebp_eax(byte* code, int current_byte, byte value)
{
	// imull value(ebp),%eax
	code[current_byte] = 0x0F;
	code[current_byte+1] = 0xAF;
	code[current_byte+2] = 0x45;
	code[current_byte+3] = value;
	return current_byte + 4;
}

static int _movl_eax_valueebp(byte* code, int current_byte, byte value)
{
	// movl %eax, value(%ebp)
	code[current_byte] = 0x89;
	code[current_byte+1] = 0x45;
	code[current_byte+2] = value;
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
	return current_byte + 1;
}
static int _movl_esp_ebp(byte* code, int current_byte)
{
	//movl %esp, %ebp
	code[current_byte] = 0x89;
	code[current_byte+1] = 0xE5;
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
	return current_byte + 5;	
}

static int _cmpl_valueebp_eax(byte* code, int current_byte, byte value)
{
	// cmpl value(%ebp),%eax
	code[current_byte] = 0x3B;
	code[current_byte+1] = 0x45;
	code[current_byte+2] = value;
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
	return current_byte + 6;
}

//================================================================================================================================

static byte ebpOffsetValue(int indexes_of_used_variables[], int* num_of_used_variables, char variable_type, int variable_index)
{
	int index_in_the_array;
	// If it is a variable
	if(variable_type == 'v')
	{
		// Search in the array if the variable has already been defined
		index_in_the_array = search(indexes_of_used_variables, *num_of_used_variables, variable_index);
		// If not,
		if(index_in_the_array == -1)
		{
			// Checks if more than the MAXIMUM_NUMBER_OF_VARIABLES was used
			if(*num_of_used_variables == MAXIMUM_NUMBER_OF_VARIABLES)
			{
				printf("Error: More than %d variables used.\n",MAXIMUM_NUMBER_OF_VARIABLES);
				exit(1);
			}
			// If it is possible to add one more variable,
			// Add this variable index in the array
			else
			{
				indexes_of_used_variables[*num_of_used_variables] = variable_index;
				index_in_the_array = *num_of_used_variables;
				(*num_of_used_variables)++;
			}
		}
		// Returns the corresponding offset for the %ebp
		// It is used ~OFFSET+1 because the %ebp offsets for variables need to be negative.
		return ~(((byte) MAXIMUM_NUMBER_OF_VARIABLES - (byte) index_in_the_array) * 4) + 1;
	}
	// If it is a parameter
	else
	{
		return  8 + 4 * (byte) variable_index;
	}
}

static int generateOpcodeReturn(byte* code, int current_byte, int indexes_of_used_variables[], int* num_of_used_variables, char operand_type, Value operand_value)
{
	// Adds the opcode that moves the operand to %eax
	if(operand_type == '$')
		current_byte = _movl_value_eax(code, current_byte, operand_value);
	else if(operand_type == 'v' || operand_type == 'p')
		current_byte = _movl_valueebp_eax(code, current_byte, ebpOffsetValue(indexes_of_used_variables, num_of_used_variables, operand_type, operand_value.i));

	// Adds the return opcode itself
	current_byte = _movl_ebp_esp(code, current_byte);
	current_byte = _popl_ebp(code, current_byte);
	current_byte = _ret(code, current_byte);

	return current_byte;
}

static int generateOpcodeAtt(byte* code, int current_byte, int indexes_of_used_variables[], int* num_of_used_variables,
									char operation_type, 
									char destination_operand_type, Value destination_operand_value, 
									char first_operand_type, Value first_operand_value,
									char second_operand_type, Value second_operand_value)
{
	// Adds the opcode that computes the first operand. (Moves the operand to %eax)
	if(first_operand_type == '$')
		current_byte = _movl_value_eax(code, current_byte, first_operand_value);
	else if(first_operand_type == 'v' || first_operand_type == 'p')
		current_byte = _movl_valueebp_eax(code, current_byte, ebpOffsetValue(indexes_of_used_variables, num_of_used_variables, first_operand_type, first_operand_value.i));

	// Adds the opcode that computes the second operand by doing the desired operation itself in %eax
	if(second_operand_type == '$')
		current_byte =  _operate_value_eax(code, current_byte, second_operand_value, operation_type);
	else if(second_operand_type == 'v' || second_operand_type == 'p')
		current_byte = _operate_valueebp_eax(code, current_byte, ebpOffsetValue(indexes_of_used_variables, num_of_used_variables, second_operand_type, second_operand_value.i), operation_type);

	// Adds the opcode that moves the result (located in %eax) to the its destination
	current_byte = _movl_eax_valueebp(code, current_byte, ebpOffsetValue(indexes_of_used_variables,num_of_used_variables,destination_operand_type,destination_operand_value.i));

	return current_byte;
}

static int generateOpcodeHeader(byte* code, int current_byte)
{
	Value _TWENTY;
	_TWENTY.i = 20;

	// Adds the opcode corresponding to the assembly function header (that organizes the memory stack)
	current_byte = _pushl_ebp(code, current_byte);
	current_byte = _movl_esp_ebp(code, current_byte);

	// Adds the opcode that creates space for the variables
	current_byte = _subl_value_esp(code, current_byte, _TWENTY);

	return current_byte;
}

static int generateOpcodeIfEqual(byte* code, int current_byte, int indexes_of_used_variables[], int* num_of_used_variables, 
						char first_operand_type, Value first_operand_value, 
						char second_operand_type, Value second_operand_value, 
						JumpReference array_of_jump_references[], int jump_destination_line, int current_line_of_code)
{
	// Arbitrary value for the relative jump address (could be any value)
	// This value will be overwritten later in the jump address update
	Value _ZERO;
	_ZERO.i = 0;

	// Adds the opcode that computes the first operand
	if(first_operand_type == '$')
		current_byte = _movl_value_eax(code, current_byte, first_operand_value);
	else if(first_operand_type == 'v' || first_operand_type == 'p')
		current_byte = _movl_valueebp_eax(code, current_byte, ebpOffsetValue(indexes_of_used_variables,num_of_used_variables,first_operand_type,first_operand_value.i));

	// Adds the opcode that compares the second operand with the first
	if(second_operand_type == '$')
		current_byte = _cmpl_value_eax(code, current_byte, second_operand_value);
	else if(second_operand_type == 'v' || second_operand_type == 'p')
		current_byte = _cmpl_valueebp_eax(code, current_byte, ebpOffsetValue(indexes_of_used_variables,num_of_used_variables,second_operand_type,second_operand_value.i));

	// Stores a jump reference for the desired jump line.
	// This will be used later for updating the JMP FAR pending addresses
	array_of_jump_references[current_line_of_code].byte_to_write = current_byte + 2;
	array_of_jump_references[current_line_of_code].jump_line_dest = jump_destination_line;

	// Adds the opcode corresponding to the JMP FAR operation to the desired line
	// The relative jump address will only be known later, so a arbitrary default value is used here (_ZERO)
	current_byte = _je_value(code, current_byte, _ZERO);

	return current_byte;
}

funcp geracod (FILE *f)
{
	// The opcode array
	byte* code;

	// The array that stores which 'minima' variables were used
	int indexes_of_used_variables[MAXIMUM_NUMBER_OF_VARIABLES];
	int num_of_used_variables = 0;

	// The array that stores the pending jump references updates
	// This will be used to locate where a jump reference should be updated in the code
	JumpReference array_of_jump_references[MAXIMUM_NUMBER_OF_LINES_OF_CODE + 1];

	// The array that stores in which location the corresponding opcode resides for each line
	// If the 'minima' line X has its upcode in the postion Y, we should have correspondent_opcode_location[X] == Y
	int correspondent_opcode_location[MAXIMUM_NUMBER_OF_LINES_OF_CODE + 1];
	
	// -----------
	// Variables used for input reading
	// -----------
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
	// -----------

	// Variable used for storing the JUMP relative address
	Value opcode_location_difference;

	// The current byte being written in the code array and the current line of code in 'minima'
	int current_byte = 0;
	int current_line_of_code = 0;

	// Iterator
	int i;
	
	// Clears the array_of_jump_references
	for(i=1; i<=MAXIMUM_NUMBER_OF_LINES_OF_CODE; i++)
	{
		array_of_jump_references[i].byte_to_write = -1;
		array_of_jump_references[i].jump_line_dest = -1;
	}

	// Dynamically allocates the space for the opcode array
	code = malloc(MAXIMUM_NUMBER_OF_LINES_OF_CODE * LARGEST_OPCODE_SIZE);

	if(code == NULL)
	{
		printf("Error: Code Area Allocation Error.\n");
		exit(1);
	}
	
	// Saves the header in the array
	current_byte = generateOpcodeHeader(code,current_byte);
	
	// Keeps reading the first character in the line
	while(fscanf(f," %c",&first_char_in_line)==1)
	{
		// Updates the current_line_of_code and the correspondent_opcode_location, because a new line is being read
		current_line_of_code++;
		correspondent_opcode_location[current_line_of_code] = current_byte;

		// Checks what is the value of the first character read
		// If it is 'i', there is a command 'ifeq' ahead
		if(first_char_in_line == 'i')
		{
			// Reads the rest of the command
			fscanf(f,"feq %c%d %c%d %d",&first_operand_type,&(first_operand_value.i),&second_operand_type,&(second_operand_value.i),&jump_destination_line);

			// Validates it
			if(first_operand_type != '$' && first_operand_type != 'v' && first_operand_type != 'p')
			{
				printf("Error (line %d): invalid symbol after ':=' found.\n",current_line_of_code);
				exit(1);
			}
			if(second_operand_type != '$' && second_operand_type != 'v' && second_operand_type != 'p')
			{
				printf("Error (line %d): invalid symbol after '%c' found.\n", operation_type,current_line_of_code);
				exit(1);
			}

			// Adds the corresponding opcode to the code array
			current_byte = generateOpcodeIfEqual(code, current_byte, indexes_of_used_variables, &num_of_used_variables, first_operand_type, first_operand_value, second_operand_type, second_operand_value, array_of_jump_references, jump_destination_line, current_line_of_code);
		}
		// If it is 'r', there is a command 'ret' ahead
		else if(first_char_in_line == 'r')
		{
			// Reads the rest of the command
			fscanf(f,"et %c%d",&operand_type,&operand_value.i);

			// Validates it
			if(operand_type != '$' && operand_type != 'v' && operand_type != 'p')
			{
				printf("Error (line %d): invalid symbol after 'ret' found.\n",current_line_of_code);
				exit(1);
			}

			// Adds the corresponding opcode to the code array
			current_byte = generateOpcodeReturn(code,current_byte,indexes_of_used_variables,&num_of_used_variables,operand_type,operand_value);
		}
		// If it is 'v' or 'p', there is an attribution ahead
		else if(first_char_in_line == 'v' || first_char_in_line == 'p')
		{
			// Reads the rest of the command
			destination_operand_type = first_char_in_line;
			fscanf(f," %d := %c%d %c %c%d",&destination_operand_value.i,
											&first_operand_type,(int*) first_operand_value.b,
											&operation_type,
											&second_operand_type,&(second_operand_value.i));

			// Validates it
			if(first_operand_type != '$' && first_operand_type != 'v' && first_operand_type != 'p')
			{
				printf("Error (line %d): invalid symbol after ':=' found.\n",current_line_of_code);
				exit(1);
			}
			if(second_operand_type != '$' && second_operand_type != 'v' && second_operand_type != 'p')
			{
				printf("Error (line %d): invalid symbol after '%c' found.\n", operation_type,current_line_of_code);
				exit(1);
			}
			if(operation_type != '+' && operation_type != '-' && operation_type != '*')
			{
				printf("Error (line %d): Invalid operator '%c'.\n",operation_type,current_line_of_code);
				exit(1);
			}
			
			// Adds the corresponding opcode to the code array
			current_byte = generateOpcodeAtt(code, current_byte, indexes_of_used_variables, &num_of_used_variables,
													operation_type,
													destination_operand_type,destination_operand_value,
													first_operand_type, first_operand_value,
													second_operand_type, second_operand_value);

		}
		// If the first character is not a known one, it is a invalid 'minima' command
		else
		{
			printf("Error (line %d): Unknown line of code found.\n",current_line_of_code);
			exit(1);
		}
	}

	// This part updates the pending memory relative jump references with its correct value
	// Iterates through the JumpReference's array
	for(i=1;i<=MAXIMUM_NUMBER_OF_LINES_OF_CODE;i++)
	{
		// For each valid JumpReference
		if(array_of_jump_references[i].byte_to_write != -1)
		{
			// Calculates the memory relative jump
			opcode_location_difference.i = correspondent_opcode_location[array_of_jump_references[i].jump_line_dest] - (array_of_jump_references[i].byte_to_write+4);
			
			// Updates the code array
			code[array_of_jump_references[i].byte_to_write] = opcode_location_difference.b[0];
			code[array_of_jump_references[i].byte_to_write+1] = opcode_location_difference.b[1];
			code[array_of_jump_references[i].byte_to_write+2] = opcode_location_difference.b[2];
			code[array_of_jump_references[i].byte_to_write+3] = opcode_location_difference.b[3];
		}
	}

	// Returns the reference for the code (dynamically allocated)
	return (funcp) code;
}
