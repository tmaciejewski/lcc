/* File: grammar.h
 * Desc: Grammar types and structures
 * Author: Tomasz Maciejewski (ponton[at]jabster.pl)
 * Licence: GNU General Public Licence 3.0
*/

#ifndef  GRAMMAR_INC
#define  GRAMMAR_INC

typedef enum 
{
	E_NUMBER,
	E_ID,
	E_SUM,
	E_DIFF,
	E_PROD,
	E_QUO,
	E_MOD,
	E_MAX,
	E_MIN
} expr_type;

struct expr
{
	expr_type type;
	union
	{
		struct
		{
			struct expr *e1;
			struct expr *e2;
		} exprs;

		char *id;
		int *number;
	} data;
};

typedef enum
{
	C_AND,
	C_OR,
	C_NOT,
	C_EQ,
	C_NEQ
} cond_type;

struct cond
{
	cond_type type;
	union 
	{
		struct
		{
			struct expr *e1;
			struct expr *e2;
		} exprs;

		struct
		{
			struct cond *c1;
			struct cond *c2;
		} conds;
	} data;
};


typedef enum
{
	I_VISIBLE,
	I_VISIBLE_STRING,
	I_GIMMEH,
	I_IF,
	I_LOOP,
	I_ASSIGN,
	I_BREAK,
	I_HALT
} single_type;

struct single
{
	single_type type;
	union
	{
		char *id;
		char *string;
		struct expr *e;
		struct instruction *i;
		struct
		{
			char *id;
			struct expr *e;
		} assign;
		struct
		{
			struct cond *c;
			struct instruction *t;
			struct instruction *e;
		} ifelse;
	} data;
};

struct instruction
{
	struct single *s;
	struct instruction *i;
};

struct vardecl
{
	char *id;
	int *number;
};

struct vars
{
	struct vardecl *decl;
	struct vars *v;
};

struct program
{
	struct vars *v;
	struct instruction *i;
};

#endif   /* ----- #ifndef GRAMMAR_INC  ----- */
