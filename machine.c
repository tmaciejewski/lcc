/* File: machine.c
 * Desc: Simple stack machine language interpreter and disassembler
 * Author: Tomasz Maciejewski (ponton[at]jabster.pl)
 * Licence: GNU General Public Licence 3.0
*/

#include "machine.h"

#include <stdlib.h>
#include <stdio.h>

void print_code(int *code, int code_size)
{
	int i;
 	for (i = 0; i < code_size; ++i)
	{
		printf("%4d :: ", i);
		switch (code[i])
		{
			case ASM_LVALUE:
				printf("lvalue %d\n", code[++i]);
				break;
			case ASM_PUSH:
				printf("push %d\n", code[++i]);
				break;

			case ASM_ADD:
				printf("add\n");	
				break;

			case ASM_SUB:
				printf("sub\n");
				break;
			
			case ASM_MUL:
				printf("mul\n");
				break;
			
			case ASM_DIV:
				printf("div\n");
				break;
		
			case ASM_MOD:
				printf("mod\n");
				break;

			case ASM_MAX:
				printf("max\n");
				break;

			case ASM_MIN:
				printf("min\n");
				break;

			case ASM_JMP:
				printf("jmp %d\n", code[++i]);
				break;

			case ASM_JZERO:
				printf("jzero %d\n", code[++i]);
				break;

			case ASM_JNZERO:
				printf("jnzero %d\n", code[++i]);
				break;

			case ASM_RVALUE:
				printf("rvalue %d\n", code[++i]);
				break;

			case ASM_ASSIGN:
				printf("assign\n");
				break;

			case ASM_PRINT:
				printf("print\n");
				break;

			case ASM_PRINTSTRING:
				printf("printstring\n");
				break;

			case ASM_GET:
				printf("get\n");
				break;

			case ASM_NOT:
				printf("not\n");
				break;

			case ASM_EQ:
				printf("eq\n");
				break;

			case ASM_HALT:
				printf("halt\n");
				break;

			default:
				printf("Unknown instruction\n");
				break;
		}
		
	}
}

int main(int argc, char **argv)
{
	struct asm_header h;
	FILE *f;
	int *stack;
	int stack_size;
	int *code;
	int i;

	if (argc < 2)
	{
		printf("Usage: %s file\n", argv[0]);
		return 0;
	}

	f = fopen(argv[1], "r");
	if (f == NULL)
	{
		perror("Couldn't open file");
		return -1;
	}

	fread(&h, sizeof (struct asm_header), 1, f);

	stack = calloc(65535, sizeof(int));
	code = calloc(65535, sizeof(int));

	if (code == NULL || stack == NULL)
	{
		perror("No memory");
		return -1;
	}

	fread(stack, sizeof (int), h.stack_size, f);
	fread(code, sizeof (int), h.code_size, f);
	
	stack_size = h.stack_size;

	printf("Stack:\n\n");

	for (i = 0; i < stack_size; ++i)
	{
		printf("%4d :: %d\n", i, stack[i]);
	}

	printf("\n\nCode:\n\n");

	print_code(code, h.code_size);

	printf("\n-----------------------------------------\n\n");

	i = 0;
	while (i < h.code_size)
	{
		int a, b;
		switch (code[i])
		{
			case ASM_LVALUE:
			case ASM_PUSH:
				++stack_size;
				++i;
				stack[stack_size] = code[i];
				break;

			case ASM_ADD:
				a = stack[stack_size--];
				b = stack[stack_size];
				stack[stack_size] = a+b;
				break;

			case ASM_SUB:
				a = stack[stack_size--];
				b = stack[stack_size];
				stack[stack_size] = a-b;
				break;
			
			case ASM_MUL:
				a = stack[stack_size--];
				b = stack[stack_size];
				stack[stack_size] = a*b;
				break;
			
			case ASM_DIV:
				a = stack[stack_size--];
				b = stack[stack_size];
				stack[stack_size] = a / b;
				break;
		
			case ASM_MOD:
				a = stack[stack_size--];
				b = stack[stack_size];
				stack[stack_size] = a%b;
				break;

			case ASM_MAX:
				a = stack[stack_size--];
				b = stack[stack_size];
				stack[stack_size] = (a > b ? a : b);
				break;

			case ASM_MIN:
				a = stack[stack_size--];
				b = stack[stack_size];
				stack[stack_size] = (a < b ? a : b);
				break;

			case ASM_JMP:
				a = code[++i];
				i = a;
				continue;

			case ASM_JZERO:
				a = code[++i];
				b = stack[stack_size--];
				if (b == 0)
				{
					i = a;
					continue;
				}
				break;

			case ASM_JNZERO:
				a = code[++i];
				b = stack[stack_size--];
				if (b != 0)
				{
					i = a;
					continue;
				}
				break;

			case ASM_RVALUE:
				++stack_size;
				++i;
				stack[stack_size] = stack[code[i]];
				break;

			case ASM_ASSIGN:
				a = stack[stack_size--];
				b = stack[stack_size];
				stack[a] = b;
				break;

			case ASM_PRINT:
				printf("%d\n", stack[stack_size--]);
				break;

			case ASM_PRINTSTRING:
				{
					int a = stack[stack_size--];
					while (stack[a] != '\0')
						putchar(stack[a++]);
				}
				putchar('\n');
				break;

			case ASM_GET:
				scanf("%d", &(stack[stack[stack_size--]]));
				break;

			case ASM_NOT:
				stack[stack_size] = ( stack[stack_size] == 0 ? 1 : 0);
				break;

			case ASM_EQ:
				a = stack[stack_size--];
				b = stack[stack_size];

				stack[stack_size] = (a == b ? 1 : 0);
				break;

			case ASM_HALT:
				i = h.code_size+1;
				break;

			default:
				printf("Unknown instruction\n");
				break;
		}
		++i;
	}

	fclose(f);

	free(code);
	free(stack);

	return 0;
}
