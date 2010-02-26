/* File: compiler.c
 * Desc: Here it translates program tree to asembler
 * Author: Tomasz Maciejewski (ponton[at]jabster.pl)
 * Licence: GNU General Public Licence 3.0
*/

#include "compiler.h"
#include "grammar.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int *code;
int code_max_size;
int code_size;

struct symbol *symbols;
int symbols_max_size;
int symbols_size;

int *labels;
int labels_max_size;
int labels_size;

int new_label()
{
	if (labels == NULL)
	{
		labels_max_size = 1024;
		labels = malloc(sizeof(int) * labels_max_size);
		if (labels == NULL)
		{
			perror("No memory for labels");
			exit(-1);
		}
	}

	if (labels_size == labels_max_size)
	{
		labels = realloc(labels, 2*labels_max_size);
		if (labels == NULL)
		{
			perror("No memory for code");
			exit(-1);
		}
		labels_max_size *= 2;
	}
 
 	labels[labels_size] = -1;
	return labels_size++;
}

void set_label_addr(int l)
{
	labels[l] = code_size;
}

void free_labels()
{
	if (labels)
		free(labels);
}

void free_code()
{
	if (code)
		free(code);
}

int add_code(int c)
{
	if (code == NULL)
	{
		code_max_size = 1024;
		code = malloc(code_max_size * sizeof (int));
		if (code == NULL)
		{
			perror("No memory for code");
			exit(-1);
		}
	}

	if (code_size == code_max_size)
	{
		code = realloc(code, 2*code_max_size);
		if (code == NULL)
		{
			perror("No memory for code");
			exit(-1);
		}
		code_max_size *= 2;
	}

	code[code_size] = c;
	++code_size;
	return 1;
}

void emit_code_to_file(FILE *f)
{
	int i;
	struct asm_header h;

	h.stack_size = symbols_size;
	h.code_size = code_size;
	
	if (f == NULL) return;

	fwrite(&h, sizeof(h), 1, f);
	
	for (i = 0; i < symbols_size; ++i)
	{
		int s = 0;
			if (symbols[i].number)
				s = (symbols[i].number == NULL ? 0 : *symbols[i].number);
			
			fwrite(&s, sizeof (int), 1, f);
	}
	
	for (i = 0; i < code_size; ++i)
	{
			fwrite(code+i, sizeof (int), 1, f);
			if (code[i] == ASM_JMP || code[i] == ASM_JZERO || code[i] == ASM_JNZERO)
			{
				++i;
				fwrite(&labels[code[i]], sizeof (int), 1, f);
			}
	}
}

void free_symbols()
{
	if (symbols)
		free(symbols);
}

int get_symbol(char *id)
{
	int i;

	for (i = 0; i < symbols_size; ++i)
		if (symbols[i].id)
			if (strcmp(id, symbols[i].id) == 0)
				return i;

	return -1;
}

int add_symbol(char *id, int *number)
{
	if (symbols == NULL)
	{
		symbols_max_size = 1024;
		symbols = malloc(symbols_max_size * sizeof (struct symbol));
		if (symbols == NULL)
		{
			perror("No memory for code");
			exit(-1);
		}
	}

	if (id != NULL && get_symbol(id) >= 0)
		return -1;

	if (symbols_size == symbols_max_size)
	{
		symbols = realloc(symbols, 2*symbols_max_size);
		if (symbols == NULL)
		{
			perror("No memory for code");
			exit(-1);
		}
		symbols_max_size *= 2;
	}

	symbols[symbols_size].id = id;
	symbols[symbols_size].number = number;
	++symbols_size;

	return 1;
}

int add_string(char *str)
{
	int i = symbols_size;
	do
	{
		add_symbol(NULL, (int *) str);
		++str;
	}
	while (*str != '\0');

	add_symbol(NULL, NULL);
	
	return i;
}

/*************************/

int translate_program(struct program *p)
{
	if (p == NULL) return -1;

	if (translate_vars(p->v) < 0)
		return -1;
	
	return translate_instruction(p->i, -1);
}

int translate_vars(struct vars *v)
{
	if (v == NULL) return 1;
	
	if (add_symbol(v->decl->id, v->decl->number) < 0)
	{
		printf("error in adding symbol `%s` (it might be defined twice)\n", v->decl->id);
		return -1;
	}

	translate_vars(v->v);

	return 1;
}

int translate_expr(struct expr *e)
{
	switch (e->type)
	{
		case E_NUMBER:
			add_code(ASM_PUSH);
			add_code(*(e->data.number));
			break;

		case E_ID:
			{
				int i;
				i = get_symbol(e->data.id);
				if (i < 0)
				{
					printf("error: undeclared symbol `%s`\n", e->data.id);
					return -1;
				}
				add_code(ASM_RVALUE);
				add_code(i);
			}
			break;

		default:
			{
				int a, b;
				a = translate_expr(e->data.exprs.e2);
				b = translate_expr(e->data.exprs.e1);

				if (a < 0 || b < 0)
				{
					return -1;
				}

				switch (e->type)
				{
					case E_SUM:
						add_code(ASM_ADD);
						break;

					case E_DIFF:
						add_code(ASM_SUB);
						break;

					case E_PROD:
						add_code(ASM_MUL);
						break;

					case E_QUO:
						add_code(ASM_DIV);
						break;

					case E_MOD:
						add_code(ASM_MOD);
						break;

					case E_MAX:
						add_code(ASM_MAX);
						break;

					case E_MIN:
						add_code(ASM_MIN);
						break;

					default:
						break;
				}
			}
			break;
	}
	return 1;
}

int translate_cond(struct cond *c)
{
	switch (c->type)
	{
		case C_NOT:
			if (translate_cond(c->data.conds.c1) < 0)
				return -1;
			add_code(ASM_NOT);
			break;

		case C_AND:
		case C_OR:
			{
				int a, b;

				a = translate_cond(c->data.conds.c2);
				b = translate_cond(c->data.conds.c1);

				if (a < 0 || b < 0)
					return -1;

				add_code( (c->type == C_AND ? ASM_MUL : ASM_ADD ) );
			}
			break;

		case C_EQ:
		case C_NEQ:
			{
				int a, b;

				a = translate_expr(c->data.exprs.e2);
				b = translate_expr(c->data.exprs.e1);

				if (a < 0 || b < 0)
					return -1;

				add_code(ASM_SUB);
				if (c->type == C_EQ)
					add_code(ASM_NOT);
			}
			break;
	
	}

	return 1;
}

int translate_string(char *str)
{
	int i;
	i = add_string(str);
	add_code(ASM_LVALUE);
	add_code(i);
	return 1;
}

int translate_instruction(struct instruction *i, int last)
{
	if (i == NULL) return -1;

	if (translate_single(i->s, last) < 0)
		return -1;

	if (i->i)
		return translate_instruction(i->i, last);

	return 1;
}

int translate_single(struct single *s, int end_of_loop)
{
	switch (s->type)
	{
		case I_VISIBLE:
			if (translate_expr(s->data.e) < 0) return -1;
			add_code(ASM_PRINT);
			break;

		case I_VISIBLE_STRING:
			translate_string(s->data.string);	
			add_code(ASM_PRINTSTRING);
			break;

		case I_GIMMEH:
			{
				int i;
				i = get_symbol(s->data.id);
				if (i < 0)
				{
					printf("error: undeclared `%s`\n", s->data.id);
					return -1;
				}
				add_code(ASM_LVALUE);
				add_code(i);
				add_code(ASM_GET);
				break;
			}

		case I_ASSIGN:
			{
				int i;
				
				if (translate_expr(s->data.assign.e) < 0) return -1;
				
				i = get_symbol(s->data.assign.id);
				if (i < 0)
				{
					printf("error: undeclared `%s`\n", s->data.assign.id);
					return -1;
				}
				add_code(ASM_LVALUE);
				add_code(i);
				add_code(ASM_ASSIGN);
			}
			break;

		case I_IF:
			{
				int f;
				f = new_label();

				if (translate_cond(s->data.ifelse.c) < 0)
					return -1;

				add_code(ASM_JZERO);
				add_code(f);

				if (translate_instruction(s->data.ifelse.t, end_of_loop) < 0)
					return -1;

				if (s->data.ifelse.e)
				{
					int after;
					after = new_label();
					add_code(ASM_JMP);
					add_code(after);
					set_label_addr(f);
					if (translate_instruction(s->data.ifelse.e, end_of_loop) < 0)
						return -1;
					set_label_addr(after);					
				}
				else
					set_label_addr(f);
			}
			break;

		case I_LOOP:
			{
				int p, k;
				p = new_label();
				k = new_label();
				set_label_addr(p);
				if (translate_instruction(s->data.i, k) < 0)
					return -1;
				add_code(ASM_JMP);
				add_code(p);
				set_label_addr(k);
			}
			break;

		case I_BREAK:
			if (end_of_loop < 0) return -1;
			add_code(ASM_JMP);
			add_code(end_of_loop);
			break;

		case I_HALT:
			add_code(ASM_HALT);
			break;

		default:
			break;
	}

	return 1;
}
