// David Winfield, James Nguyen

// Systems Software Spring 2022

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"
#define MAX_NUMBER_TOKENS 1000
#define MAX_IDENT_LEN 11
#define MAX_NUMBER_LEN 5

lexeme *list;
int lex_index;

void printlexerror(int type);
void printtokens();

int isnumber(char string[])
{
    for (int i = 0; string[i]!= '\0'; i++)
    {
        if (isdigit(string[i]) == 0)
              return 0;
    }
    return 1;
}

int process(char *buffer, int buffIndex, int *inComment)
{
	// Put in the null terminator in the buffer
	buffer[buffIndex] = '\0';

	// Comment end detected
	if (strcmp(buffer, "*/") == 0)
	{
		// printf("hi i'm not in a comment\n");
		*inComment = 0;
	}
	// Skip commented out code
	else if (*inComment == 1)
	{
		buffIndex = 0;
	}
	// Comment start detected
	else if (strcmp(buffer, "/*") == 0)
	{
		// printf("hi i AM in a comment\n");
		*inComment = 1;
	}

	// Check if the buffer is a reserved word or an operator
	else if (strcmp(buffer, "const") == 0)
	{
		strcpy(list[lex_index].name, "const");
		list[lex_index].value = 2;
		list[lex_index].type = constsym;
		lex_index++;
	}
	else if (strcmp(buffer, "var") == 0)
	{
		strcpy(list[lex_index].name, "var");
		list[lex_index].value = 4;
		list[lex_index].type = varsym;
		lex_index++;
	}
	else if (strcmp(buffer, "procedure") == 0)
	{
		strcpy(list[lex_index].name, "procedure");
		list[lex_index].value = 6;
		list[lex_index].type = procsym;
		lex_index++;
	}
	else if (strcmp(buffer, "call") == 0)
	{
		strcpy(list[lex_index].name, "call");
		list[lex_index].value = 8;
		list[lex_index].type = callsym;
		lex_index++;
	}
	else if (strcmp(buffer, "if") == 0)
	{
		strcpy(list[lex_index].name, "if");
		list[lex_index].value = 10;
		list[lex_index].type = ifsym;
		lex_index++;
	}
	else if (strcmp(buffer, "then") == 0)
	{
		strcpy(list[lex_index].name, "then");
		list[lex_index].value = 12;
		list[lex_index].type = thensym;
		lex_index++;
	}
	else if (strcmp(buffer, "else") == 0)
	{
		strcpy(list[lex_index].name, "else");
		list[lex_index].value = 14;
		list[lex_index].type = elsesym;
		lex_index++;
	}
	else if (strcmp(buffer, "while") == 0)
	{
		strcpy(list[lex_index].name, "while");
		list[lex_index].value = 16;
		list[lex_index].type = whilesym;
		lex_index++;
	}
	else if (strcmp(buffer, "do") == 0)
	{
		strcpy(list[lex_index].name, "do");
		list[lex_index].value = 18;
		list[lex_index].type = dosym;
		lex_index++;
	}
	else if (strcmp(buffer, "begin") == 0)
	{
		strcpy(list[lex_index].name, "begin");
		list[lex_index].value = 20;
		list[lex_index].type = beginsym;
		lex_index++;
	}
	else if (strcmp(buffer, "end") == 0)
	{
		strcpy(list[lex_index].name, "end");
		list[lex_index].value = 22;
		list[lex_index].type = endsym;
		lex_index++;
	}
	else if (strcmp(buffer, "read") == 0)
	{
		strcpy(list[lex_index].name, "read");
		list[lex_index].value = 24;
		list[lex_index].type = readsym;
		lex_index++;
	}
	else if (strcmp(buffer, "write") == 0)
	{
		strcpy(list[lex_index].name, "write");
		list[lex_index].value = 26;
		list[lex_index].type = writesym;
		lex_index++;
	}
	else if (strcmp(buffer, "==") == 0)
	{
		list[lex_index].type = eqlsym;
		lex_index++;
	}
	else if (strcmp(buffer, "!=") == 0)
	{
		list[lex_index].type = neqsym;
		lex_index++;
	}
	else if (strcmp(buffer, "<") == 0)
	{
		list[lex_index].type = lsssym;
		lex_index++;
	}
	else if (strcmp(buffer, "<=") == 0)
	{
		list[lex_index].type = leqsym;
		lex_index++;
	}
	else if (strcmp(buffer, ">") == 0)
	{
		list[lex_index].type = gtrsym;
		lex_index++;
	}
	else if (strcmp(buffer, ">=") == 0)
	{
		list[lex_index].type = geqsym;
		lex_index++;
	}
	else if (strcmp(buffer, "*") == 0)
	{
		list[lex_index].type = multsym;
		lex_index++;
	}
	else if (strcmp(buffer, "/") == 0)
	{
		list[lex_index].type = divsym;
		lex_index++;
	}
	else if (strcmp(buffer, "+") == 0)
	{
		list[lex_index].type = plussym;
		lex_index++;
	}
	else if (strcmp(buffer, "-") == 0)
	{
		list[lex_index].type = minussym;
		lex_index++;
	}
	else if (strcmp(buffer, "(") == 0)
	{
		list[lex_index].type = lparensym;
		lex_index++;
	}
	else if (strcmp(buffer, ")") == 0)
	{
		list[lex_index].type = rparensym;
		lex_index++;
	}	
	else if (strcmp(buffer, ",") == 0)
	{
		list[lex_index].type = commasym;
		lex_index++;
	}
	else if (strcmp(buffer, ".") == 0)
	{
		list[lex_index].type = periodsym;
		lex_index++;
	}
	else if (strcmp(buffer, ";") == 0)
	{
		list[lex_index].type = semicolonsym;
		lex_index++;
	}
	else if (strcmp(buffer, ":=") == 0)
	{
		list[lex_index].type = assignsym;
		lex_index++;
	}
	else if (strcmp(buffer, "&&"))
	{
		// add to list
		// increment lex_index
	}
	else if (strcmp(buffer, "||"))
	{
		// add to list
		// increment lex_index
	}
	else if (strcmp(buffer, "!"))
	{
		// add to list
		// increment lex_index
	}
	// Check if identifider or number or error
	else
	{
		// printf("HI I'm in ELSE!!!\n");
		// Check to see if we have an identifier
		if (isalpha(buffer[0]))
		{
			list[lex_index].type = identsym;
			// printf("list[%d] : %d\n", lex_index, list[lex_index].type);
			strcpy(list[lex_index++].name, buffer);
		}
		// Check to see if the buffer is a number
		else if (isdigit(buffer[0]))
		{
			int bufferLength = strlen(buffer);
			
			// ERROR: Number length
			if (bufferLength > MAX_NUMBER_LEN)
			{
				printf("%s\n", buffer);
				printlexerror(2);
				return 1;
			}

			list[lex_index].type = numbersym;
			list[lex_index].value = atoi(buffer);
			lex_index++;
		}
		
	}

	return 0;
}

lexeme *lexanalyzer(char *input, int printFlag)
{
	list = malloc(sizeof(lexeme) * MAX_NUMBER_TOKENS);
	lex_index = 0;

	// Read in input character by character until end
	char charIn, bufferType, buffer[MAX_IDENT_LEN + 1];
	int buffIndex, length, inComment, hasError;
	buffIndex = inComment = hasError = 0;

	for (int i = 0, length = strlen(input); i < length; i++)
	{
		charIn = input[i];

		// If it's not an escape character, we can potentially do something
		if (iscntrl(charIn) == 0 || charIn == '\n')
		{
			// Potential comment detected
            if (charIn == '/')
            {
                // Start comment
                if (input[i + 1] == '*')
                {
                    inComment = 1;
                }
                // End comment
                else if (input[i - 1] == '*')
                {
                    inComment = 0;
                    buffIndex = 0;
                    continue;
                }

                // No comment found
            }

            if (inComment) continue;

			// If it's a space, we know we're at the end of an operator, a digit, or a word or there's a bunch of spaces
			if (isspace(charIn))
			{
				// If we actually have anything in our buffer
				if (buffIndex > 0)
				{
					hasError = process(buffer, buffIndex, &inComment);
					if (hasError) return NULL;
				}

				// Safe to restart the buffer
				buffIndex = 0;
			}
			// New character is not a space
			else
			{
				// If alphanumeric
				if (isalpha(charIn) || isdigit(charIn))
				{

					// ERROR: Identifier Length
					if (buffIndex >= MAX_IDENT_LEN) 
					{
						printlexerror(3);
						return NULL;
					}

					// ERROR: Number's too long
					if (buffIndex > 0 && isdigit(buffer[0]) && isnumber(buffer) && buffIndex >= MAX_NUMBER_LEN)
					{
						printlexerror(2);
						return NULL;
					}

					// ERROR: Invalid identifier
					if (isdigit(buffer[0]) && buffIndex > 0)
					{
						for (int i = 1; i < buffIndex; i++)
						{
							if (!isdigit(buffer[i]))
							{
								printlexerror(1);
								return NULL;
							}
						}
					}
					
					// Safe to store this character to buffer
					buffer[buffIndex] = charIn;
					buffIndex++;
				}
				// Special characters
				else
				{
					// Check if comment or actual symbol or error
					switch (charIn)
					{
						// Known symbols
						case '-':	// no append
							if (inComment == 0) list[lex_index++].type = minussym;
							break;
						case '(':	// no append
							if (inComment == 0) list[lex_index++].type = lparensym;
							break;
						case ')':	// no append
							if (inComment == 0) list[lex_index++].type = rparensym;
							break;
						case ';':	// no append
							hasError = process(buffer, buffIndex, &inComment);
							if (hasError) return NULL;
							if (inComment == 0) list[lex_index++].type = semicolonsym;
							buffIndex = 0;
							break;
						case ',':	// no append
							hasError = process(buffer, buffIndex, &inComment);
							if (hasError) return NULL;
							if (inComment == 0) list[lex_index++].type = commasym;
							buffIndex = 0;
							break;
						case '.':	// no append
							hasError = process(buffer, buffIndex, &inComment);
							if (hasError) return NULL;
							if (inComment == 0) list[lex_index++].type = periodsym;
							buffIndex = 0;
							break;
						case '=':
						case '!':
						case '<':
						case '>':
						case '*':
						case '/':
						case '+':
						case ':':
							if (inComment == 0)
							{
								buffer[buffIndex] = charIn;
								buffIndex++;
								break;
							}
							else
							{
								break;
							}
						// ERROR: Invalid symbol
						default:
							printlexerror(4);
							return NULL;
					}
				}
			}
		}
		else
		{
			buffer[buffIndex] = '\0';
			// buffIndex = 0;
		}	
	}

	if (inComment)
	{
		printlexerror(5);
		return NULL;
	}

	
	if (printFlag)
		printtokens();

	// these last two lines are really important for the rest of the package to run
	list[lex_index].type = -1;
	return list;
}

void printtokens()
{
	int i;
	printf("Lexeme Table:\n");
	printf("lexeme\t\ttoken type\n");
	for (i = 0; i < lex_index; i++)
	{
		switch (list[i].type)
		{
			case eqlsym:
				printf("%11s\t%d", "==", eqlsym);
				break;
			case neqsym:
				printf("%11s\t%d", "!=", neqsym);
				break;
			case lsssym:
				printf("%11s\t%d", "<", lsssym);
				break;
			case leqsym:
				printf("%11s\t%d", "<=", leqsym);
				break;
			case gtrsym:
				printf("%11s\t%d", ">", gtrsym);
				break;
			case geqsym:
				printf("%11s\t%d", ">=", geqsym);
				break;
			case multsym:
				printf("%11s\t%d", "*", multsym);
				break;
			case divsym:
				printf("%11s\t%d", "/", divsym);
				break;
			case plussym:
				printf("%11s\t%d", "+", plussym);
				break;
			case minussym:
				printf("%11s\t%d", "-", minussym);
				break;
			case lparensym:
				printf("%11s\t%d", "(", lparensym);
				break;
			case rparensym:
				printf("%11s\t%d", ")", rparensym);
				break;
			case commasym:
				printf("%11s\t%d", ",", commasym);
				break;
			case periodsym:
				printf("%11s\t%d", ".", periodsym);
				break;
			case semicolonsym:
				printf("%11s\t%d", ";", semicolonsym);
				break;
			case assignsym:
				printf("%11s\t%d", ":=", assignsym);
				break;
			case beginsym:
				printf("%11s\t%d", "begin", beginsym);
				break;
			case endsym:
				printf("%11s\t%d", "end", endsym);
				break;
			case ifsym:
				printf("%11s\t%d", "if", ifsym);
				break;
			case thensym:
				printf("%11s\t%d", "then", thensym);
				break;
			case elsesym:
				printf("%11s\t%d", "else", elsesym);
				break;
			case whilesym:
				printf("%11s\t%d", "while", whilesym);
				break;
			case dosym:
				printf("%11s\t%d", "do", dosym);
				break;
			case callsym:
				printf("%11s\t%d", "call", callsym);
				break;
			case writesym:
				printf("%11s\t%d", "write", writesym);
				break;
			case readsym:
				printf("%11s\t%d", "read", readsym);
				break;
			case constsym:
				printf("%11s\t%d", "const", constsym);
				break;
			case varsym:
				printf("%11s\t%d", "var", varsym);
				break;
			case procsym:
				printf("%11s\t%d", "procedure", procsym);
				break;
			case identsym:
				printf("%11s\t%d", list[i].name, identsym);
				break;
			case numbersym:
				printf("%11d\t%d", list[i].value, numbersym);
				break;
		}
		printf("\n");
	}
	printf("\n");
	printf("Token List:\n");
	for (i = 0; i < lex_index; i++)
	{
		if (list[i].type == numbersym)
			printf("%d %d ", numbersym, list[i].value);
		else if (list[i].type == identsym)
			printf("%d %s ", identsym, list[i].name);
		else
			printf("%d ", list[i].type);
	}
	printf("\n");
}

void printlexerror(int type)
{
	if (type == 1)
		printf("Lexical Analyzer Error: Invalid Identifier\n");
	else if (type == 2)
		printf("Lexical Analyzer Error: Number Length\n");
	else if (type == 3)
		printf("Lexical Analyzer Error: Identifier Length\n");
	else if (type == 4)
		printf("Lexical Analyzer Error: Invalid Symbol\n");
	else if (type == 5)
		printf("Lexical Analyzer Error: Never-ending comment\n");
	else
		printf("Implementation Error: Unrecognized Error Type\n");
	
	free(list);
	return;
}