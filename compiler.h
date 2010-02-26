/* File: compiler.h
 * Desc: Declarations of compiler functions
 * Author: Tomasz Maciejewski (ponton[at]jabster.pl)
 * Licence: GNU General Public Licence 3.0
*/

#ifndef  COMPILER_INC
#define  COMPILER_INC

#include "machine.h"
#include "grammar.h"

#include <stdio.h>

extern int *code;
extern int code_max_size;
extern int code_size;

void free_code();
int add_code(int);
void emit_code_to_file(FILE *);


struct symbol
{
	char *id;
	int *number;	
};

extern struct symbol *symbols;
extern int symbols_max_size;
extern int symbols_size;

void free_symbols();
int add_symbol(char *, int *);

void free_labels();

int translate_program(struct program *);
int translate_vars(struct vars *);
int translate_instruction(struct instruction *, int);
int translate_single(struct single *, int);

#endif   /* ----- #ifndef COMPILER_INC  ----- */

