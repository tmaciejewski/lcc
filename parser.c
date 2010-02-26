/* File: parser.c
 * Desc: Here it scans tokens and recognizes grammar
 * Author: Tomasz Maciejewski (ponton[at]jabster.pl)
 * Licence: GNU General Public Licence 3.0
*/

#include <stdlib.h>

#include "lexer.h"
#include "parser.h"

static int current_token;

static token get_token();
static void unget_token();
static void parse_error(char *);

char *parse_id()
{
	token t;
	t = get_token();

	if (t.type != T_ID)
	{
		unget_token();
		return NULL;
	}

	return (char *) t.data;
}

int *parse_number()
{
	token t;
	t = get_token();

	if (t.type != T_NUMBER)
	{
		unget_token();
		return NULL;
	}

	return (int *) t.data;
}

struct cond* parse_cond()
{
	int error = 0, current = current_token;
	struct cond *c;
	token t;

	c = malloc(sizeof(struct cond));
	if (c == NULL)
	{
		perror("No memory");
		return NULL;
	}

	t = get_token();

	switch (t.type)
	{
		case T_NOT:
			c->type = C_NOT;
			c->data.conds.c1 = parse_cond();
			if (c->data.conds.c1 == NULL)
			{
				error = 1;
			}
			break;
		
		case T_EITHER:
		case T_BOTH:
			if (t.type == T_BOTH)
				c->type = C_AND;
			else
				c->type = C_OR;

			c->data.conds.c1 = parse_cond();
			if (c->data.conds.c1 == NULL)
			{
				error = 1;
			}
			else
			{
				t = get_token();
				if (t.type != T_AN)
				{
					error = 1;
					free(c->data.conds.c1);
					parse_error("expected `AN`");
				}
				else
				{
					c->data.conds.c2 = parse_cond();
					if (c->data.conds.c2 == NULL)
					{
						error = 1;
					}
				}
			}
			break;

		case T_DIFFRINT:
		case T_SAEM:
			if (t.type == T_DIFFRINT)
				c->type = C_NEQ;
			else
				c->type = C_EQ;

			c->data.exprs.e1 = parse_expr();
			if (c->data.exprs.e1 == NULL)
			{
				error = 1;
			}
			else
			{
				t = get_token();
				if (t.type != T_AN)
				{
					error = 1;
					free(c->data.exprs.e1);
					parse_error("expected `AN`");
				}
				else
				{
					c->data.exprs.e2 = parse_expr();
					if (c->data.exprs.e2 == NULL)
					{
						error = 1;
					}
				}
			}
			break;


		default:
			unget_token();
			parse_error("expected condition");
			error = 1;
	}

	if (error == 1)
	{
		free(c);
		current_token = current;
		return NULL;
	}

	return c;
}

void free_cond(struct cond *c)
{
	if (c == NULL) return;

	switch (c->type)
	{
		case C_AND:
		case C_OR:
			free_cond(c->data.conds.c2);
		case C_NOT:
			free_cond(c->data.conds.c1);
			break;

		case C_EQ:
		case C_NEQ:
			free_expr(c->data.exprs.e1);
			free_expr(c->data.exprs.e2);
			break;

		default:
			break;
	}
	
	free(c);
}

struct expr *parse_expr()
{
	int error = 0, current = current_token;
	token t;
	token_type type;
	struct expr *e;
	
	e = malloc(sizeof(struct expr));
	if (e == NULL) 
	{
		perror("No memory");
		return NULL;
	}

	if ((e->data.id  = parse_id()) != NULL)
	{
		e->type = E_ID;
		return e;
	}

	if ((e->data.number = parse_number()) != NULL)
	{
		e->type = E_NUMBER;
		return e;
	}

	t = get_token();

	switch (t.type)
	{
		case T_SUM:
		case T_DIFF:
		case T_PRODUKT:
		case T_QUOSHUNT:
		case T_MOD:
		case T_BIGGR:
		case T_SMALLR:
			type = t.type;
			t = get_token();
			if (t.type != T_OF)
			{
				error = 1;
				parse_error("expected `OF`");
			}
			else
			{
				e->data.exprs.e1 = parse_expr();
				if (e->data.exprs.e1 == NULL)
				{
					error = 1;
				}
				else
				{
					t = get_token();
					if (t.type != T_AN)
					{
						error = 1;
						parse_error("exptected `AN`");
					}
					else
					{
						e->data.exprs.e2 = parse_expr();
						if (e->data.exprs.e2 == NULL)
						{
							free_expr(e->data.exprs.e1);
							error = 1;
						}

					}
				}
			}

			switch (type)
			{
				case T_SUM:
					e->type = E_SUM;
					break;
				case T_DIFF:
					e->type = E_DIFF;
					break;
				case T_PRODUKT:
					e->type = E_PROD;
					break;
				case T_QUOSHUNT:
					e->type = E_QUO;
					break;
				case T_MOD:
					e->type = E_MOD;
					break;
				case T_BIGGR:
					e->type = E_MAX;
					break;
				case T_SMALLR:
					e->type = E_MIN;
					break;
				default:
					break;
			}

			break;

		default:
			unget_token();
			parse_error("expected expression");
			error = 1;
	}

	if (error == 1)
	{
		current_token = current;
		free(e);
		return NULL;
	}

	return e;
}

void free_expr(struct expr *e)
{
	if (e)
	{
		if (e->type != E_ID && e->type != E_NUMBER)
		{
			free_expr(e->data.exprs.e1);
			free_expr(e->data.exprs.e2);
		}

		free (e);
	}
}

struct single *parse_gtfo()
{
	struct single *s;

	s = malloc(sizeof(struct single));

	if (s == NULL)
	{
		perror("No memory");
		return NULL;
	}

	if (get_token().type == T_GTFO)
	{
		s->type = I_BREAK;
		return s;
	}
	
	unget_token();
	return NULL;
}

struct single *parse_halt()
{
	struct single *s;

	s = malloc(sizeof(struct single));

	if (s == NULL)
	{
		perror("No memory");
		return NULL;
	}

	if (get_token().type == T_KTHXBYE)
	{
		s->type = I_HALT;
		return s;
	}
	
	unget_token();
	return NULL;
}


struct single *parse_single()
{
	struct single *s;

	s = parse_io();

	if (s != NULL)
		return s;

	s = parse_loop();

	if (s != NULL)
		return s;

	s = parse_if();

	if (s != NULL)
		return s;

	s = parse_assign();

	if (s != NULL)
		return s;

	s = parse_gtfo();

	if (s != NULL)
		return s;

	s = parse_halt();

	if (s != NULL)
		return s;

	return NULL;
}

void free_single(struct single *s)
{
	free(s);
}

struct single *parse_loop()
{
	int current = current_token;
	token t1, t2, t3, t4;
	struct single *s;

	s = malloc(sizeof(struct single));
	if (s == NULL)
	{
		perror("No memory");
		return NULL;
	}

	t1 = get_token();
	t2 = get_token();
	t3 = get_token();
	t4 = get_token();

	if (t1.type != T_IM || t2.type != T_IN || t3.type != T_YR || t4.type != T_LOOP)
	{
		free(s);
		current_token = current;
		return NULL;
	}

	s->type = I_LOOP;

	t1 = get_token();
	if (t1.type != T_SEP)
	{
		free(s);
		parse_error("expected separator");
		return NULL;
	}

	s->data.i = parse_instruction();

	if (s->data.i == NULL)
	{
		free(s);
		parse_error("expected instruction");
		return NULL;
	}

	t1 = get_token();
	if (t1.type != T_SEP)
	{
		free(s);
		parse_error("expected separator");
		return NULL;
	}

	t1 = get_token();
	t2 = get_token();
	t3 = get_token();
	t4 = get_token();

	if (t1.type != T_IM || t2.type != T_OUTTA || t3.type != T_YR || t4.type != T_LOOP)
	{
		free(s);
		parse_error("expected `IM OUTTA YR LOOP`");
		return NULL;
	}

	return s;
}

struct single *parse_assign()
{
	struct single *s;

	if (get_token().type != T_LOL)
	{
		unget_token() ;
		return NULL;
	}

	s = malloc(sizeof(struct single));

	if (s == NULL)
	{
		perror("No memory");
		return NULL;
	}

	s->type = I_ASSIGN;

	s->data.assign.id = parse_id();
	if (s->data.assign.id == NULL)
	{
		parse_error("expected id");
		free(s);
		return NULL;
	}
	
	if (get_token().type != T_IZ)
	{
		parse_error("expected `IZ`");
		free(s);
		return NULL;
	}

	s->data.assign.e = parse_expr();
	if (s->data.assign.e == NULL)
	{
		parse_error("expected expression");
		free(s);
		return NULL;
	}

	return s;
}

struct single *parse_if()
{	
	int current = current_token;
	token t1, t2, t3, t4, t5;
	struct single *s;

	s = malloc(sizeof(struct single));
	if (s == NULL)
	{
		perror("No memory");
		return NULL;
	}

	t1 = get_token();
	t2 = get_token();
	
	if (t1.type != T_O || t2.type != T_RLY)
	{
		free(s);
		current_token = current;
		return NULL;
	}

	s->type = I_IF;

	s->data.ifelse.c = parse_cond();
	if (s->data.ifelse.c == NULL)
	{
		free(s);
		parse_error("expected condition");
		return NULL;
	}

	t1 = get_token();
	t2 = get_token();
	t3 = get_token();
	t4 = get_token();
	t5 = get_token();

	if (t1.type != T_QMARK || t2.type != T_SEP || t3.type != T_YA || t4.type != T_RLY || t5.type != T_SEP)
	{
		free(s);
		parse_error("wrong YA RLY syntax");
		return NULL;
	}

	s->data.ifelse.t = parse_instruction();
	if (s->data.ifelse.t == NULL)
	{
		free(s);
		parse_error("expected instruction");
		return NULL;
	}

	t1 = get_token();
	t2 = get_token();
	
	if (t1.type == T_SEP && t2.type == T_OIC)
	{
		s->data.ifelse.e = NULL;
		return s;
	}
	
	t3 = get_token();
	t4 = get_token();

	if (t1.type != T_SEP || t2.type != T_NO || t3.type != T_WAI || t4.type != T_SEP)
	{
		free(s);
		parse_error("wrong NO WAI syntax");
		return NULL;
	}

	s->data.ifelse.e = parse_instruction();
	if (s->data.ifelse.e == NULL)
	{
		free(s);
		parse_error("expected instruction");
		return NULL;
	}

	t1 = get_token();
	t2 = get_token();
	
	if (t1.type != T_SEP || t2.type != T_OIC)
	{
		free(s);
		parse_error("expected OIC");
		return NULL;
	}

	return s;
}

struct single *parse_io()
{
	token t;
	int current = current_token, error = 0;
	struct single *s;

	s = malloc(sizeof (struct single));

	t = get_token();

	if (t.type == T_VISIBLE)
	{
		if ((t = get_token()).type == T_STRING)
		{
			s->type = I_VISIBLE_STRING;
			s->data.string = t.data;
		}
		else
		{	
			unget_token();

			s->type = I_VISIBLE;
			s->data.e = parse_expr();

			if (s->data.e == NULL)
			{
				error = 1;
				parse_error("I can only visible strings and epressions");
			}
		}
	}
	else if (t.type == T_GIMMEH)
	{
		s->type = I_GIMMEH;
		s->data.id = parse_id();
		if (s->data.id == NULL)
		{
			error = 1;
			
		}
	}
	else
		error = 1;
	
	if (error == 1)
	{
		free(s);
		current_token = current;
		return NULL;
	}

	return s;
}

struct instruction *parse_instruction()
{
	struct instruction *i;

	i = malloc(sizeof(struct instruction));
	if (i == NULL)
	{
		perror("No memory");
		return NULL;
	}

	i->s = parse_single();

	if (i->s == NULL)
	{
		free(i);
		return NULL;
	}

	if (get_token().type != T_SEP)
	{
		unget_token();
		i->i = NULL;
		return i;
	}

	i->i = parse_instruction();

	if (i->i == NULL)
	{
		unget_token();
	}

	return i;
}

void free_instruction(struct instruction *i)
{
	if (i)
	{
		free_single(i->s);
		free_instruction(i->i);
	}
}

struct vardecl *parse_decl()
{
	int current = current_token;
	token t1, t2;
	struct vardecl *d;

	t1 = get_token();
	t2 = get_token();

	if (t1.type != T_I || t2.type != T_HAS)
	{
		current_token = current;
		return NULL;
	}

	d = malloc(sizeof(struct vardecl));

	if (d == NULL)
	{
		perror("No memory");
		return NULL;
	}

	d->id = parse_id();

	if (d->id == NULL)
	{
		free(d);
		parse_error("expected id");
		return NULL;
	}

	if (get_token().type != T_ITZ)
	{
		unget_token();
		d->number = NULL;
		return d;
	}

	d->number = parse_number();
	if (d->number == NULL)
	{
		free(d);
		parse_error("expected number");
		return NULL;
	}

	return d;
}

void free_decl(struct vardecl *d)
{
	if (d)	free(d);
}

struct vars *parse_vars()
{
	struct vars *v;

	v = malloc(sizeof(struct vars));
	if (v == NULL)
	{
		perror("No memory");
		return NULL;
	}

	v->decl = parse_decl();
	if (v->decl == NULL)
	{
		free(v);
		return NULL;
	}

	if (get_token().type != T_SEP)
	{
		unget_token();
		v->v = NULL;
		return v;
	}

	v->v = parse_vars();

	if (v->v == NULL)
	{
		unget_token();
	}

	return v;
}

void free_vars(struct vars *v)
{
	if (v == NULL) return;
	free_decl(v->decl);
	free_vars(v->v);
	free(v);
}

struct program *parse_program()
{
	struct program *p;

	if (get_token().type != T_HAI)
	{
		unget_token();
		return NULL;
	}

	if (get_token().type != T_SEP)
	{
		unget_token();
		parse_error("expected separator");
		return NULL;
	}

	p = malloc(sizeof(struct program));

	if (p == NULL)
	{
		perror("No memory");
		return NULL;
	}


	p->v = parse_vars();

	if (p->v != NULL)
		if (get_token().type != T_SEP)
		{
			parse_error("expected separator");
			free(p);
			return NULL;
		}

	p->i = parse_instruction();

	while (get_token().type == T_SEP);

	unget_token();

	if (tokens[current_token].type != T_EOF)
	{
		parse_error("expecting instruction");
		return NULL;
	}

	return p;
}

void free_program(struct program *p)
{
	if (p == NULL) return;
	free_vars(p->v);
	free_instruction(p->i);
	free(p);
}

/*********************/

token get_token()
{
	if (current_token < tokens_number-1)
		return tokens[current_token++];

	return tokens[current_token]; 
}

void unget_token()
{
	if (current_token > 0)
		--current_token;
}

void parse_error(char *s)
{
	if (s == NULL)
		return;

	printf("%d:%d: parse error: %s\n", tokens[current_token].line, tokens[current_token].column, s);
}
