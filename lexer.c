/* File: lexer.c
 * Desc: Here it recognizes tokens from input string
 * Author: Tomasz Maciejewski (ponton[at]jabster.pl)
 * Licence: GNU General Public Licence 3.0
*/

#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "lexer.h"

token *tokens;
int tokens_number;
int tokens_size;

static token get_next_token(FILE *in);
static int add_token(token t);
static token tokenize_string(char *);

void free_tokens()
{
	if (tokens != NULL)
	{
		int i;
		for (i = 0; i < tokens_number; ++i)
			if (tokens[i].data)
				free(tokens[i].data);
		
		free(tokens);
	}

	tokens_number = 0;
}

int get_tokens_from_file(FILE *in)
{
	token t;
	int was_error = 0;

	free_tokens();

	tokens = malloc(sizeof (token) * 1024);
	tokens_size = 1024;

	if (tokens == NULL)
	{
		perror("Error while initializing token array");
		return -1;
	}
	
	do
	{
		t = get_next_token(in);

		if (t.type == T_NULL)
		{
			was_error = 1;
		}
		
		if (add_token(t) < 0)
			return -1;
	} while (t.type != T_EOF);

	t.type = T_NULL;
	add_token(t);

	return was_error;
}

token get_next_token(FILE *in)
{
	token t = {0, NULL, 0, 0};
	int c, oldc;

	static unsigned line = 1, column = 0;

	do
	{
		c = fgetc(in);
		++column;
	} while ((c == ' ' || c == '\t') && !feof(in));

	t.line = line;
	t.column = column;
	oldc = c;

	if (feof(in))
	{
		t.type = T_EOF;
	}
	else if (isalpha(c))
	{
		/* keyword or id */
		char buf[255];
		int i;
		ungetc(c, in);
		--column;

		for (i = 0; i < 254; ++i)
		{
			char c = fgetc(in);
			if (!isdigit(c) && !isalpha(c))
			{
				ungetc(c, in);
				break;
			}

			buf[i] = c;
		}
		buf[i] = '\0';
		t = tokenize_string(buf);
		t.line = line;
		t.column = column;
		column += strlen(buf);
	}
	else if (isdigit(c))
	{
		/* number */
		int *i;
		i = malloc(sizeof(int));
		ungetc(c, in);
		--column;
		column += fscanf(in, "%d", i);
		t.type = T_NUMBER;
		t.data = i;
	}
	else if (c == '\n' || c == ',')
	{
		t.type = T_SEP;
		if (c == '\n')
		{
			column = 0;
			++line;
			while ((c = fgetc(in)) == '\n')
				++line;
			ungetc(c, in);
		}
		else
			++column;
	}
	else if (c == '?')
		t.type = T_QMARK;
	else if (c == '"')
	{
		/* string */
		char buf[1024];
		int i;
		
		t.type = T_STRING;

		for (i = 0; i < 1023; ++i)
		{
			char c = fgetc(in);
			++column;

			if (c == '\n')
				++line;

			if (c == '"')
				break;

			if (c == '\\')
			{
				c = fgetc(in);
			}

			buf[i] = c;
		}
		buf[i] = '\0';
		t.data = malloc(strlen(buf)+1);
		strcpy(t.data, buf);
	}

	if (t.type == T_NULL)
	{
		int *c;
		c = malloc(sizeof (int));
		*c = oldc;
		t.data = c;
		printf("%d:%d: error: unrecognized character `%c`\n", t.line, t.column, *((char *)t.data));

	}
	return t;
}

int add_token(token t)
{
	if (tokens_number == tokens_size)
	{
		tokens = realloc(tokens, 2*tokens_size);

		if (tokens == NULL)
		{
			perror("Couldn't realloc tokens array");
			return -1;
		}

		tokens_size *= 2;
	}

	tokens[tokens_number] = t;
	++tokens_number;

	return 0;
}

static token tokenize_string(char *s)
{
	#define CHECK(x) else if (strcmp(s, #x) == 0) t.type = T_##x ;

	token t = {T_NULL, NULL};
	if (strcmp(s, "HAI") == 0)
		t.type = T_HAI;
	CHECK(KTHXBYE)
	CHECK(I)
	CHECK(HAS)
	CHECK(ITZ)
	CHECK(LOL)
	CHECK(IZ)
	CHECK(SUM)
	CHECK(DIFF)
	CHECK(PRODUKT)
	CHECK(QUOSHUNT)
	CHECK(MOD)
	CHECK(BIGGR)
	CHECK(SMALLR)
	CHECK(OF)
	CHECK(AN)
	CHECK(VISIBLE)
	CHECK(GIMMEH)
	CHECK(IM)
	CHECK(IN)
	CHECK(YR)
	CHECK(LOOP)
	CHECK(OUTTA)
	CHECK(GTFO)
	CHECK(O)
	CHECK(RLY)
	CHECK(YA)
	CHECK(NO)
	CHECK(WAI)
	CHECK(OIC)
	CHECK(EITHER)
	CHECK(NOT)
	CHECK(BOTH)
	CHECK(SAEM)
	CHECK(DIFFRINT)
	else
	{
		char *data = malloc(strlen(s) + 1);
		strcpy(data, s);
		t.type = T_ID;
		t.data = data;
	}
	#undef CHECK
	return t;
}
