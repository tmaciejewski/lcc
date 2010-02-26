/* File: prettyprinter.c
 * Desc: It's for debug only, prints LIPS-like our program tree ;-s)
 * Author: Tomasz Maciejewski (ponton[at]jabster.pl)
 * Licence: GNU General Public Licence 3.0
*/

#include "prettyprinter.h"
#include <stdio.h>

void print_vars(struct vars *v)
{
	if (v == NULL) return;
	if (v->decl->number)
		printf("var %s = %d\n", v->decl->id, *(v->decl->number));
	else
		printf("var %s\n", v->decl->id);
	print_vars(v->v);
}

void print_program(struct program *p)
{
	if (p == NULL) return;
	print_vars(p->v);
	print_instruction(p->i);
}

void print_expr(struct expr *e)
{
	if (e)
		switch (e->type)
		{
			case E_ID:
				printf("(Id %s)", e->data.id);
				break;

			case E_NUMBER:
				printf("(Number %d)", *(e->data.number));
				break;

			case E_MAX:
			case E_MIN:
			case E_PROD:
			case E_QUO:
			case E_DIFF:
			case E_SUM:
				printf("(Expr ");
				print_expr(e->data.exprs.e1);
				putchar(' ');
				print_expr(e->data.exprs.e2);
				putchar(')');
				break;

			default:
				printf("Unknown expr?\n");
		}

}

void print_cond(struct cond *c)
{
	if (c)
		switch (c->type)
		{
			case C_NOT:
				printf("(Not ");
				print_cond(c->data.conds.c1);
				putchar(')');
				break;

			case C_OR:
			case C_AND:
				printf("(And ");
				print_cond(c->data.conds.c1);
				putchar(' ');
				print_cond(c->data.conds.c2);
				putchar(')');
				break;

			case C_NEQ:
			case C_EQ:
				printf("(Eq ");
				print_expr(c->data.exprs.e1);
				putchar(' ');
				print_expr(c->data.exprs.e2);
				putchar(')');
				break;

			default:
				printf("Unknown cond?\n");
		}

}

void print_single(struct single *s)
{
	if (s)
		switch (s->type)
		{
			case I_GIMMEH:
				printf("(Gim %s)", s->data.id);
				break;
			case I_VISIBLE:
				printf("(Vis ");
				print_expr(s->data.e);
				putchar(')');
				break;
			case I_LOOP:
				printf("(Loop ");
				print_instruction(s->data.i);
				putchar(')');
				break;
			case I_IF:
				printf("(if then ");
				print_instruction(s->data.ifelse.t);
				if (s->data.ifelse.e)
				{
					printf(" else ");
					print_instruction(s->data.ifelse.e);
				}
				putchar(')');
				break;
			case I_ASSIGN:
				printf("(%s = ", s->data.assign.id);
				print_expr(s->data.assign.e);
				putchar(')');
				break;
			case I_BREAK:
				printf("break");
				break;
			default:
				break;
		}
}

void print_instruction(struct instruction *i)
{
	if (i)
	{
		if (i->i) putchar('(');
		print_single(i->s);
		if (i->i) { print_instruction(i->i); putchar(')'); }
	}
}


