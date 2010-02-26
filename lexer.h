/* File: lexer.h
 * Desc: Declarations of lexer functions
 * Author: Tomasz Maciejewski (ponton[at]jabster.pl)
 * Licence: GNU General Public Licence 3.0
*/

#ifndef  LEXER_INC
#define  LEXER_INC

#include <stdio.h>

#include "token.h"

extern token *tokens;
extern int tokens_number;
extern int tokens_size;

int get_tokens_from_file(FILE *);
void free_tokens();

#endif   /* ----- #ifndef LEXER_INC  ----- */

