// David Winfield - Spring 2022

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

#define MAX_CODE_LENGTH 150
#define MAX_SYMBOL_COUNT 50
#define MAX_REG_HEIGHT 10

// project-defined global variables
instruction *code;
int cIndex;
symbol *table;
int level;
int tIndex;

// my global variables
lexeme token;
lexeme *list;
int lIndex;
int error = 0;
int stacksize = -1;

// base functions
void emit(int opname, int level, int mvalue);
void addToSymbolTable(int k, char n[], int v, int l, int a, int m);
void mark();
int multipledeclarationcheck(char name[]);
int findsymbol(char name[], int kind);
void printparseerror(int err_code);
void printsymboltable();
void printassemblycode();

// my functions
void program();
void block();
void constDeclaration();
int varDeclaration();
void procDeclaration();
void statement();
void condition();
void expression();
void term();
void factor();
void get_next_token();

instruction *parse(lexeme *input, int printTable, int printCode)
{
	code = malloc(sizeof(instruction) * MAX_CODE_LENGTH);
	table = malloc(sizeof(symbol) * MAX_SYMBOL_COUNT);
	tIndex = 0; // table index
	cIndex = 0; // code index
	lIndex = 0; // list index

	list = input;

	// Begin parsing
	program();

	if (error == 1)
		return NULL;
	
	if (printTable)
		printsymboltable();
	if (printCode)
		printassemblycode();
	
	code[cIndex].opcode = -1;
	return code;
}

void get_next_token()
{
	token = list[lIndex];
	lIndex++;
}

void program()
{
	get_next_token(); 

	// emit JMP
	emit(7, 0, 0);
	
	addToSymbolTable(3, "main", 0, 0, 0, 0);
	
	level = -1;
	
	block();

	if (error == 1)
		return;

	// program must be closed by a period
	if (token.type != periodsym)
	{
		printparseerror(1);
		error = 1;
		return;
	}

	// emit HALT
	emit(9, 0, 3);

	// correct addresses
	for (int i = 0; i < cIndex; i++)
	{
		if (code[i].opcode == 5)
			code[i].m = table[code[i].m].addr;
	}

	// correct main's address
	code[0].m = table[0].addr;

	return;
}

void block()
{
	// store original procedure index
	int procIdx = tIndex - 1;
	level++;
	
	if (token.type == constsym)
		constDeclaration();

	if (error == 1)
		return;

	int x = 0;
	
	if (token.type == varsym)
		x = varDeclaration();

	if (error == 1)
		return;
		
	while (token.type == procsym)
	{
		procDeclaration();
		if (error == 1)
			return;
	}

	// correct addresses
	table[procIdx].addr = cIndex;

	// emit INC statement
	emit(6, 0, (x + 3));

	// call statement()
	statement();

	if (error == 1)
		return;

	// call mark
	mark();

	// decrement level
	level--;
}

void constDeclaration()
{
	do
	{
		// order must follow constsym -> identsym -> eqlsym -> numbersym
		get_next_token(); // identsym

		// constant declarations should follow specified pattern
		if (token.type != identsym)
		{
			printparseerror(2);
			error = 1;
			return;
		}

		char name[12];
		strcpy(name, token.name);

		// check if new symbol is valid
		if (multipledeclarationcheck(token.name) != -1)
		{
			printparseerror(18);
			error = 1;
			return;
		}
		
		get_next_token(); // assignsym

		// constant declarations should follow specified pattern
		if (token.type != assignsym)
		{
			printparseerror(2);
			error = 1;
			return;
		}

		get_next_token(); // numbersym

		// constant declarations should follow specified pattern
		if (token.type != numbersym)
		{
			printparseerror(2);
			error = 1;
			return;
		}
		
		// correct syntax, free to add token to symbol table
		addToSymbolTable(1, name, token.value, level, 0, 0);

		get_next_token(); // comma or something else
	} 
	while (token.type == commasym);

	// identifiers must be separated by commas
	if (token.type == identsym)
	{
		printparseerror(13);
		error = 1;
		return;
	}
	
	// symbol declarations should close with a semicolon
	if (token.type != semicolonsym)
	{
		printparseerror(14);
		error = 1;
		return;
	}

	get_next_token();
}

int varDeclaration()
{
	int counter = 0;
	// structure after picking up initial var declaration:
	// identifier -> {, -> identifier} -> semicolon
	do
	{
		// process a variable
		get_next_token();

		// variable declarations should follow specified pattern
		if (token.type != identsym)
		{
			printparseerror(3);
			error = 1;
			return counter;
		}

		// identifier already declared in symbol table
		if (multipledeclarationcheck(token.name) != -1)
		{
			printparseerror(18);
			error = 1;
			return counter;
		}

		addToSymbolTable(2, token.name, 0, level, counter + 3, 0);

		counter++;
		
		get_next_token();
	} 
	while (token.type == commasym);

	// identifiers must be separated by commas
	if (token.type == identsym)
	{
		printparseerror(13);
		error = 1;
		return counter;
	}
	
	if (token.type != semicolonsym)
	{
		printparseerror(14);
		error = 1;
		return counter;
	}
	
	get_next_token();

	return counter;
}

void procDeclaration()
{
	// procedure structure:
	// identifier -> semicolon -> block -> semicolon

	get_next_token(); // identifier

	if (token.type != identsym)
	{
		printparseerror(4);
		error = 1;
		return;
	}

	// invalid declaration
	if (multipledeclarationcheck(token.name) != -1)
	{
		printparseerror(18);
		error = 1;
		return;
	}

	// add to symbol table
	addToSymbolTable(3, token.name, 0, level, 0, 0);

	get_next_token(); // semicolon

	if (token.type != semicolonsym)
	{
		printparseerror(4);
		error = 1;
		return;
	}
	
	get_next_token();

	block();

	if (error == 1)
		return;

	if (token.type != semicolonsym)
	{
		printparseerror(14);
		error = 1;
		return;
	}

	// emit RET
	emit(2, 0, 0);
	
	get_next_token();
}

void statement()
{
	int symIdx = -1;
	int jpcIdx = -1;
	int jmpIdx = -1;
	int loopIdx = -1;

	// assignment statement
	if (token.type == identsym)
	{
		// search symbol table for token
		if (findsymbol(token.name, 2) != -1) // variable found!
			symIdx = findsymbol(token.name, 2);
		else if (findsymbol(token.name, 1) != -1) // identifier is a constant
		{
			printparseerror(6);
			error = 1;
			return;
		}
		else if (findsymbol(token.name, 3) != -1) // identifier is a procedure
		{
			printparseerror(6);
			error = 1;
			return;
		}
		else
		{
			printparseerror(19); // undeclared identifier
			error = 1;
			return;
		}

		get_next_token(); // equalsymbol

		// variables must be assigned using :=
		if (token.type != assignsym)
		{
			printparseerror(5);
			error = 1;
			return;
		}

		// out of courtesy, get next token before next instruction
		get_next_token();

		// execute expression
		expression();

		if (error == 1)
			return;

		// safe to do STO (4) emit statement
		emit(4, level - table[symIdx].level, table[symIdx].addr);

		stacksize--;
	}

	// begin statement
	else if (token.type == beginsym)
	{
		do 
		{
			get_next_token();
			statement();
			if (error == 1)
				return;
		}
		while (token.type == semicolonsym);

		// if token is another statement, we're missing a semicolon
		if (token.type == identsym || token.type == callsym || token.type == beginsym || token.type == ifsym || token.type == whilesym || token.type == readsym || token.type == writesym)
		{
			printparseerror(15);
			error = 1;
			return;
		}
		else if (token.type != endsym)
		{
			printparseerror(16);
			error = 1;
			return;
		}
		get_next_token();
	}

	// if statement
	else if (token.type == ifsym)
	{
		get_next_token(); // get next token

		condition(); // implied that this gets next token

		if (error == 1)
			return;

		if (token.type != thensym)
		{
			printparseerror(8);
			error = 1;
			return;
		}

		get_next_token();
		
		jpcIdx = cIndex;

		// emit JPC
		emit(8, 0, 0);

		stacksize--;

		statement(); // implied that this gets next token

		if (error == 1)
			return;

		if (token.type == elsesym)
		{
			get_next_token();

			jmpIdx = cIndex;

			// emit JMP
			emit(7, 0, 0);

			code[jpcIdx].m = cIndex;

			statement(); // implied that this gets next token

			if (error == 1)
				return;

			code[jmpIdx].m = cIndex;
		}
		else
			code[jpcIdx].m = cIndex;
	}

	// while statement
	else if (token.type == whilesym)
	{
		get_next_token();

		loopIdx = cIndex;

		condition(); // implied that this gets next token

		if (error == 1)
			return;

		// check for do symbol
		if (token.type != dosym)
		{
			printparseerror(9);
			error = 1;
			return;
		}

		get_next_token();

		jpcIdx = cIndex;

		// emit JMPC
		emit(8, 0, 0);

		stacksize--;

		statement();

		if (error == 1)
			return;

		// emit JMP
		emit(7, 0, loopIdx);

		
		code[jpcIdx].m = cIndex;
	}

	// read staement
	else if (token.type == readsym)
	{
		get_next_token(); // identifier

		if (token.type != identsym)
		{
			printparseerror(6);
			error = 1;
			return;
		}

		// search symbol table for token
		if (findsymbol(token.name, 2) != -1) // variable found!
			symIdx = findsymbol(token.name, 2);
		else if (findsymbol(token.name, 1) != -1) // identifier is a constant
		{
			printparseerror(6);
			error = 1;
			return;
		}
		else if (findsymbol(token.name, 3) != -1) // identifier is a procedure
		{
			printparseerror(6);
			error = 1;
			return;
		}
		else
		{
			printparseerror(19); // undeclared identifier
			error = 1;
			return;
		}

		// emit READ
		emit(9, 0, 2);

		stacksize++;
		if (stacksize >= MAX_REG_HEIGHT)
		{
			printparseerror(20);
			error = 1;
			return;
		}

		// emit STO
		emit(4, level - table[symIdx].level, table[symIdx].addr);

		stacksize--;

		// out of courtesy, get next token before next instruction
		get_next_token();
	}

	// write statement
	else if (token.type == writesym)
	{
		get_next_token();

		expression();

		if (error == 1)
			return;

		// emit WRT
		emit(9, 0, 1);

		stacksize--;
	}

	// call statement()
	else if (token.type == callsym)
	{
		get_next_token(); // identifier

		if (token.type != identsym)
		{
			printparseerror(7);
			error = 1;
			return;
		}
		
		// search table for usable procedure
		if (findsymbol(token.name, 3) != -1) // procedure found!
			symIdx = findsymbol(token.name, 3);
		else if (findsymbol(token.name, 1) != -1) // constant
		{
			printparseerror(7);
			error = 1;
			return;
		}
		else if (findsymbol(token.name, 2) != -1) // variable
		{
			printparseerror(7);
			error = 1;
			return;
		}
		else
		{
			printparseerror(19); // no match
			error = 1;
			return;
		}

		// emit CAL
		emit(5, level - table[symIdx].level, symIdx);

		// out of courtesy, get next token for next procedure
		get_next_token();
	}
}

void condition()
{
	// call expression()
	expression();

	if (error == 1)
		return;

	// process current token
	if (token.type == eqlsym)
	{
		get_next_token();
		expression();
		if (error == 1)
			return;
		emit(2, 0, 6); // emit EQL
	}
	else if (token.type == neqsym)
	{
		get_next_token();
		expression();
		if (error == 1)
			return;
		emit(2, 0, 7); // emit NEQ
	}
	else if (token.type == lsssym)
	{
		get_next_token();
		expression();
		if (error == 1)
			return;
		emit(2, 0, 8); // emit LSS
	}
	else if (token.type == leqsym)
	{
		get_next_token();
		expression();
		if (error == 1)
			return;
		emit(2, 0, 9); // emit LEQ
	}
	else if (token.type == gtrsym)
	{
		get_next_token();
		expression();
		if (error == 1)
			return;
		emit(2, 0, 10); // emit GTR
	}
	else if (token.type == geqsym)
	{
		get_next_token();
		expression();
		if (error == 1)
			return;
		emit(2, 0, 11); // emit GEQ
	}
	else
	{
		printparseerror(10);
		error = 1;
		return;
	}

	stacksize--;
}

void expression()
{
	if (token.type == minussym)
	{
		get_next_token();

		term(); // implied that this gets the next token

		if (error == 1)
			return;

		// emit NEG
		emit(2, 0, 1);

		while (token.type == plussym || token.type == minussym)
		{
			if (token.type == plussym)
			{
				get_next_token();
				term(); // implied that this gets the next token
				if (error == 1)
					return;
				// emit ADD
				emit(2, 0, 2);

				stacksize--;
			}
			else
			{
				get_next_token();
				term(); // implied that this gets the next token
				if (error == 1)
					return;
				// emit SUB
				emit(2, 0, 3);
				stacksize--;
			}
		}
	}
	// else statement
	else
	{
		if (token.type == plussym)
			get_next_token();

		term(); // implied that this gets the next token

		if (error == 1)
			return;

		while (token.type == plussym || token.type == minussym)
		{
			if (token.type == plussym)
			{
				get_next_token();
				term(); // implied that this gets the next token
				if (error == 1)
					return;
				// emit ADD
				emit(2, 0, 2);
				stacksize--;
			}
			else
			{
				get_next_token();
				term(); // implied that this gets the next token
				if (error == 1)
					return;
				// emit SUB
				emit(2, 0, 3);
				stacksize--;
			}
		}
	}

	// bad arithmetic
	if (token.type == plussym || token.type == minussym || token.type == multsym || token.type == divsym || token.type == identsym || token.type == numbersym || token.type == lparensym)
	{
		printparseerror(17);
		error = 1;
		return;
	}
}

void term()
{
	// call factor()
	factor(); // implied that this gets the next token

	if (error == 1)
		return;

	// while loop
	while (token.type == multsym || token.type == divsym)
	{
		if (token.type == multsym)
		{
			get_next_token();
			factor(); // implied that this gets the next token
			if (error == 1)
				return;
			// emit MUL
			emit(2, 0, 4);
			stacksize--;
		}
		else
		{
			get_next_token();
			factor(); // implied that this gets the next token
			if (error == 1)
				return;
			// emit DIV
			emit(2, 0, 5);
			stacksize--;
		}
	}
}

void factor()
{
	int constIdx = -1;
	int varIdx = -1;

	if (token.type == identsym)
	{
		// search for valid constant or var
		constIdx = findsymbol(token.name, 1);
		varIdx = findsymbol(token.name, 2);

		if (findsymbol(token.name, 3) != -1 && constIdx == -1 && varIdx == -1) // proc found
		{
			printparseerror(11);
			error = 1;
			return;
		}
		else if (constIdx == -1 && varIdx == -1) // no identifier found at all in table
		{
			printparseerror(19);
			error = 1;
			return;
		}

		stacksize++;

		if (stacksize >= MAX_REG_HEIGHT)
		{
			printparseerror(20);
			error = 1;
			return;
		}
		if (constIdx != -1 && varIdx != -1) // both var and const found - take higher level 
		{
			

			if (table[constIdx].level > table[varIdx].level) // const is higher, take that
				emit(1, 0, table[constIdx].val);
			else // var is higher, take that
				emit(3, level - table[varIdx].level, table[varIdx].addr);
		}
		else if (constIdx != -1) // only constant is option
			emit(1, 0, table[constIdx].val);
		else if (varIdx != -1) // only variable is option
			emit(3, level - table[varIdx].level, table[varIdx].addr);

		get_next_token();
	}
	else if (token.type == numbersym)
	{
		stacksize++;

		if (stacksize >= MAX_REG_HEIGHT)
		{
			printparseerror(20);
			error = 1;
			return;
		}
		// emit LIT
		emit(1, 0, token.value);

		get_next_token();
	}
	else if (token.type == lparensym)
	{
		get_next_token();
		// run expression
		expression();
		if (error == 1)
			return;

		if (token.type != rparensym)
		{
			printparseerror(12);
			error = 1;
			return;
		}

		get_next_token();
	}
	else
	{
		printparseerror(11);
		error = 1;
		return;
	}
}

void logic()
{
	// insert function details here
}

// adds a line of code to the program
void emit(int opname, int level, int mvalue)
{
	code[cIndex].opcode = opname;
	code[cIndex].l = level;
	code[cIndex].m = mvalue;
	cIndex++;
}

// add a symbol to the symbol table
void addToSymbolTable(int k, char n[], int v, int l, int a, int m)
{
	table[tIndex].kind = k;
	strcpy(table[tIndex].name, n);
	table[tIndex].val = v;
	table[tIndex].level = l;
	table[tIndex].addr = a;
	table[tIndex].mark = m;
	tIndex++;
}

// mark the symbols belonging to the current procedure, should be called at the end of block
void mark()
{
	int i;
	for (i = tIndex - 1; i >= 0; i--)
	{
		if (table[i].mark == 1)
			continue;
		if (table[i].level < level)
			return;
		table[i].mark = 1;
	}
}

// checks if a new symbol has a valid name, by checking if there's an existing symbol
// with the same name in the procedure
int multipledeclarationcheck(char name[])
{
	int i;
	for (i = 0; i < tIndex; i++)
		if (table[i].mark == 0 && table[i].level == level && strcmp(name, table[i].name) == 0)
			return i;
	return -1;
}

// returns the index of a symbol with a given name and kind in the symbol table
// returns -1 if not found
// prioritizes lower lex levels
int findsymbol(char name[], int kind)
{
	int i;
	int max_idx = -1;
	int max_lvl = -1;
	for (i = 0; i < tIndex; i++)
	{
		if (table[i].mark == 0 && table[i].kind == kind && strcmp(name, table[i].name) == 0)
		{
			if (max_idx == -1 || table[i].level > max_lvl)
			{
				max_idx = i;
				max_lvl = table[i].level;
			}
		}
	}
	return max_idx;
}

void printparseerror(int err_code)
{
	switch (err_code)
	{
		case 1:
			printf("Parser Error: Program must be closed by a period\n");
			break;
		case 2:
			printf("Parser Error: Constant declarations should follow the pattern 'ident := number {, ident := number}'\n");
			break;
		case 3:
			printf("Parser Error: Variable declarations should follow the pattern 'ident {, ident}'\n");
			break;
		case 4:
			printf("Parser Error: Procedure declarations should follow the pattern 'ident ;'\n");
			break;
		case 5:
			printf("Parser Error: Variables must be assigned using :=\n");
			break;
		case 6:
			printf("Parser Error: Only variables may be assigned to or read\n");
			break;
		case 7:
			printf("Parser Error: call must be followed by a procedure identifier\n");
			break;
		case 8:
			printf("Parser Error: if must be followed by then\n");
			break;
		case 9:
			printf("Parser Error: while must be followed by do\n");
			break;
		case 10:
			printf("Parser Error: Relational operator missing from condition\n");
			break;
		case 11:
			printf("Parser Error: Arithmetic expressions may only contain arithmetic operators, numbers, parentheses, constants, and variables\n");
			break;
		case 12:
			printf("Parser Error: ( must be followed by )\n");
			break;
		case 13:
			printf("Parser Error: Multiple symbols in variable and constant declarations must be separated by commas\n");
			break;
		case 14:
			printf("Parser Error: Symbol declarations should close with a semicolon\n");
			break;
		case 15:
			printf("Parser Error: Statements within begin-end must be separated by a semicolon\n");
			break;
		case 16:
			printf("Parser Error: begin must be followed by end\n");
			break;
		case 17:
			printf("Parser Error: Bad arithmetic\n");
			break;
		case 18:
			printf("Parser Error: Confliciting symbol declarations\n");
			break;
		case 19:
			printf("Parser Error: Undeclared identifier\n");
			break;
		case 20:
			printf("Parser Error: Register Overflow Error\n");
			break;
		default:
			printf("Implementation Error: unrecognized error code\n");
			break;
	}
	
	free(code);
	free(table);
}

void printsymboltable()
{
	int i;
	printf("Symbol Table:\n");
	printf("Kind | Name        | Value | Level | Address | Mark\n");
	printf("---------------------------------------------------\n");
	for (i = 0; i < tIndex; i++)
		printf("%4d | %11s | %5d | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].val, table[i].level, table[i].addr, table[i].mark); 
	
	free(table);
	table = NULL;
}

void printassemblycode()
{
	int i;
	printf("Line\tOP Code\tOP Name\tL\tM\n");
	for (i = 0; i < cIndex; i++)
	{
		printf("%d\t", i);
		printf("%d\t", code[i].opcode);
		switch (code[i].opcode)
		{
			case 1:
				printf("LIT\t");
				break;
			case 2:
				switch (code[i].m)
				{
					case 0:
						printf("RET\t");
						break;
					case 1:
						printf("NEG\t");
						break;
					case 2:
						printf("ADD\t");
						break;
					case 3:
						printf("SUB\t");
						break;
					case 4:
						printf("MUL\t");
						break;
					case 5:
						printf("DIV\t");
						break;
					case 6:
						printf("EQL\t");
						break;
					case 7:
						printf("NEQ\t");
						break;
					case 8:
						printf("LSS\t");
						break;
					case 9:
						printf("LEQ\t");
						break;
					case 10:
						printf("GTR\t");
						break;
					case 11:
						printf("GEQ\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			case 3:
				printf("LOD\t");
				break;
			case 4:
				printf("STO\t");
				break;
			case 5:
				printf("CAL\t");
				break;
			case 6:
				printf("INC\t");
				break;
			case 7:
				printf("JMP\t");
				break;
			case 8:
				printf("JPC\t");
				break;
			case 9:
				switch (code[i].m)
				{
					case 1:
						printf("WRT\t");
						break;
					case 2:
						printf("RED\t");
						break;
					case 3:
						printf("HAL\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			default:
				printf("err\t");
				break;
		}
		printf("%d\t%d\n", code[i].l, code[i].m);
	}
	if (table != NULL)
		free(table);
}