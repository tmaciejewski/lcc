/* File: prettyprinter.h
 * Desc: Prettyprinter functions
 * Author: Tomasz Maciejewski (ponton[at]jabster.pl)
 * Licence: GNU General Public Licence 3.0
*/

#ifndef  PRETTYPRINTER_INC
#define  PRETTYPRINTER_INC

#include "grammar.h"

void print_vars(struct vars *);
void print_program(struct program *);
void print_expr(struct expr *);
void print_cond(struct cond *);
void print_single(struct single *);
void print_instruction(struct instruction *);

#endif   /* ----- #ifndef PRETTYPRINTER_INC  ----- */

