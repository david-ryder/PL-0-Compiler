#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compiler.h"
#define MAX_REG_LENGTH 10
#define MAX_DATA_LENGTH 50
#define MAX_PROGRAM_LENGTH 150

void print_execution(int line, char *opname, instruction IR, int PC, int BP, int SP, int RP, int *data_stack, int *register_stack)
{
	int i;
	// print out instruction and registers
	printf("%2d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t\t", line, opname, IR.l, IR.m, PC, BP, SP, RP);
	
	// print register stack
	for (i = MAX_REG_LENGTH - 1; i >= RP; i--)
		printf("%d ", register_stack[i]);
	printf("\n");
	
	// print data stack
	printf("\tdata stack : ");
	for (i = 0; i <= SP; i++)
		printf("%d ", data_stack[i]);
	printf("\n");
}

int base(int L, int BP, int *data_stack)
{
	int ctr = L;
	int rtn = BP;
	while (ctr > 0)
	{
		rtn = data_stack[rtn];
		ctr--;
	}
	return rtn;
}

/*
Important pages:
32 - instruction set arch.
33- 35 - op code/func values
*/

/*
instruction struct for reference:
typedef struct instruction {
	int opcode;
	int l;
	int m;
} instruction;
*/


void execute_program(instruction *code, int printFlag)
{
	// Declare registers and stacks
	int BP, SP, PC, RP, SL, DL, RA, haltFlag, line;
	int dataStack[MAX_DATA_LENGTH], registerStack[MAX_REG_LENGTH];
	char opname[4];
	instruction IR;

	// Initialize variables with data or 0s
	BP = PC = 0, haltFlag = line = 1, SP = BP - 1, RP = MAX_REG_LENGTH;

	for (int i = 0; i < MAX_DATA_LENGTH; i++) dataStack[i] = 0;

	for (int i = 0; i < MAX_REG_LENGTH; i++) registerStack[i] = 0;

	// Keep looping until we see a HAL instruction
	while (haltFlag)
	{
		// Fetch
		IR = code[PC];
		PC++;

		// Execute
		switch (IR.opcode)
		{
			// LIT
			// Push constant value IR.M onto next empty spot on register stack; RP--
			case 1:
			{
				registerStack[RP] = IR.m;
				RP--;
				strcpy(opname, "LIT");
				break;
			}
			
			// OPR
			// Operations to be performed on the registers at the bottom of the stack
			case 2:		
			{
				switch (IR.m)
				{
					// Result stored in higher of two registers if required
					// RET
					// Return from subprocedure; SP moves to top of last AR, BP and PC restored w/ DL and RA from AR
					case 0:		
					{
						SP = BP - 1;
						BP = DL;
						PC = RA;
						strcpy(opname, "RET");
						break;
					}
					// NEG
					// Negate register at the bottom of the reg. stack
					case 1:		
					{
						registerStack[RP] = registerStack[RP] * -1;
						strcpy(opname, "NEG");
						break;
					}
					// ADD
					// Add 2 registers at the bottom of the reg. stack
					case 2:	
					{
						registerStack[RP - 1] = registerStack[RP - 1] + registerStack[RP];
						RP++;
						strcpy(opname, "ADD");
						break;
					}	
					// SUB
					// Subtract lower pos. from higher pos; RP must increment
					case 3:	
					{
						registerStack[RP - 1] = registerStack[RP - 1] - registerStack[RP];
						RP++;
						break;
					}	
					// MUL
					// Multiply the two registers at the bottom of the register stack; same RP logic as above
					case 4:	
					{
						registerStack[RP - 1] = registerStack[RP - 1] * registerStack[RP];
						RP++;
						break;
					}	
					// DIV
					// Divide; same res & RP logic as abov
					case 5:		
					{
						registerStack[RP - 1] = registerStack[RP - 1] / registerStack[RP];
						RP++;
						break;
					}
					// EQL
					// 1 if bot. registers are ==, 0 otherwise; same RP logic as above
					case 6:		
					{
						if (registerStack[RP - 1] == registerStack[RP])
							registerStack[RP - 1] = 1;
						else
							registerStack[RP - 1] = 0;

						RP++;
						break;
					}
					// NEQ
					// 1 if bot. registers != each other, 0 otherwise; same RP logic as above
					case 7:		
					{
						if (registerStack[RP - 1] != registerStack[RP])
							registerStack[RP - 1] = 1;
						else
							registerStack[RP - 1] = 0;

						RP++;
						break;
					}
					// LSS
					// 1 if higher < lower, 0 otherwise; same RP logic as above
					case 8:		
					{
						if (registerStack[RP - 1] < registerStack[RP])
							registerStack[RP - 1] = 1;
						else
							registerStack[RP - 1] = 0;

						RP++;
						break;
					}
					// LEQ
					// 1 if higher <= lower, 0 otherwise,  same RP logic as above
					case 9:		
					{
						if (registerStack[RP - 1] <= registerStack[RP])
							registerStack[RP - 1] = 1;
						else
							registerStack[RP - 1] = 0;

						RP++;
						break;
					}
					// GTR
					case 10:	// 1 if higher > lower, 0 otherwise, same RP logic as above
					{
						if (registerStack[RP - 1] > registerStack[RP])
							registerStack[RP - 1] = 1;
						else
							registerStack[RP - 1] = 0;
						RP++;
						break;
					}
					// GEQ
					case 11:	// 1 if higher >= lower, 0 otherwise, same RP logic as above
					{
						if (registerStack[RP - 1] >= registerStack[RP])
							registerStack[RP - 1] = 1;
						else
							registerStack[RP - 1] = 0;

						RP++;
						break;
					}
					// AND
					case 12:
					{
						/*Take the logical AND of the two values at the bottom of the register
						stack. If they are both 1, set the higher position to 1. Otherwise, set
						it to 0. RP increases either way*/
						if (registerStack[RP - 1] == 1 && registerStack[RP - 2] == 1)
							registerStack[RP - 1] == 1;
						else
							registerStack[RP - 1] == 0;
						
						RP++;
						break;
					}
					// ORR
					case 13:
					{
						/*Take the logical OR of the two values at the bottom of the register stack.
						If either is equal to 1, set the higher position to 1. Otherwise, set it 
						to 0. RP increases either way*/
						if (registerStack[RP - 1] == 1 || registerStack[RP - 2] == 1)
							registerStack[RP - 1] == 1;
						else
							registerStack[RP - 1] == 0;
						
						RP++;
						break;
					}
					// NOT
					case 14:
					{
						/*Set the value at the bottom of the register stack to its logical reverse
						1 -> 0, and 0 -> 1*/
						if (registerStack[RP - 1] == 1)
							registerStack[RP - 1] == 0;
						else if (registerStack[RP - 1] == 0)
							registerStack[RP - 1] == 1;
					}
				}
			}
			// LOD
			case 3:		// data_stack[base(L) + M]; Load value or bottom of the reg stack from data stack location at offset M from L levels down
			{
				registerStack[RP] = dataStack[base(IR.l, BP, dataStack) + IR.m];
				// Emailed Elle about moving RP
				// RP--;
				break;
			}
			// STO
			case 4: 	// data_stack[base(L) + M]; Store value from bottom of register stack to the data stack location at offset M from L lexicographical levels down
			{
				dataStack[base(IR.l, BP, dataStack) + IR.m] = registerStack[RP + 1];
				// Emailed Elle about moving RP
				// RP++;
				break;
			}
			// CAL
			case 5:		// Call procedure at code index M; generate a new AR; 1st - SL = base(L), 2nd - DL = BP, 3rd - RA = PC; then BP = SL of new AR, PC = M
			{
				// Store to return to old AR
				SL = base(IR.l, BP, dataStack);
				DL = BP;
				RA = PC;
				
				// Move onto the next stack frame
				BP = SP + 1;
				PC = IR.m;
				break;
			}
			// INC
			case 6:		// Allocate M memory words (increment SP by M)
			{
				SP += IR.m;
				break;
			}
			// JMP
			case 7:		// Jump to instruction M.
			{
				PC = IR.m;
				break;
			}
			// JPC
			case 8:		// Jump to instruction M if the bottom of the register stack is 0. RP will increment.
			{
				if (registerStack[RP] == 0)
				{
					PC = IR.m;
					RP++;
				}

				break;
			}
			case 9:
				switch (IR.m)
				{
					// WRT
					// Print the bottom of the reg. stack; RP will incremement
					case 1:
					{
						RP++;
						printf("%d", registerStack[RP]);
						break;
					}
					// RED
					// Read input from user and store of bottom; RP will decrement
					case 2:
					{
						RP--;
						printf("Please Enter an Integer: ");
						scanf("%d", &registerStack[RP]);
						break;
					}
					// HALT
					case 3:
					{
						haltFlag = 0;
						break;
					}
				}
				break;		// Print the bottom register stack element. RP will decrement.
		
			default:
				break;
		}
		print_execution(line, "", IR, PC, BP, SP, RP, dataStack, registerStack);
		line++;
	}

	if (printFlag)
	{
		printf("\t\t\t\tPC\tBP\tSP\tRP\n");
		printf("Initial values:\t\t\t%d\t%d\t%d\t%d\n", PC, BP, SP, RP);
	}
	
}