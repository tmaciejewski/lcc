/* File: lcc.c
 * Desc: Main file
 * Author: Tomasz Maciejewski (ponton[at]jabster.pl)
 * Licence: GNU General Public Licence 3.0
*/

#include "lexer.h"
#include "parser.h"
#include "compiler.h"
#include "prettyprinter.h"

#include <stdio.h>

int main(int argc, char **argv)
{
	struct program *p;
	FILE *out;
	FILE *in;

	if (argc != 3)
	{
		printf("Usage: %s file.in file.out\n", argv[0]);
		return 0;
	}

	if (argv[1][0] == '-' && argv[1][1] == '\0')
		in = stdin;
	else
	{
		in = fopen(argv[1], "r");
		if (in == NULL)
		{
			perror("Can't open input");
			return -1;
		}
	}

	out = fopen(argv[2], "w");
	if (out == NULL)
	{
		perror("Can't open output");
		return -1;
	}


	get_tokens_from_file(in);

/*
	printf("Tokens:");

	for (i = 0; i < tokens_number; ++i)
	{
		printf(" %d", tokens[i].type);

		if (tokens[i].type == T_NUMBER)
			printf(" (%d)", *((int *)tokens[i].data));
		else if (tokens[i].type == T_ID || tokens[i].type == T_STRING)
			printf(" (%s)", (char *) tokens[i].data);
	}
	printf("\n");	
*/
	p = parse_program();

	if (p != NULL)
	{
		if (translate_program(p) > 0)
			emit_code_to_file(out);
		else
			printf("There were some errors\n");
	}

	free_code();
	free_tokens();
	free_program(p);
	free_symbols();
	free_labels();

	return 0;
}
