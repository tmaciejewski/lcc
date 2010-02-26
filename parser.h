/* File: parser.h
 * Desc: Parser function declaration
 * Author: Tomasz Maciejewski (ponton[at]jabster.pl)
 * Licence: GNU General Public Licence 3.0
*/

#ifndef  PARSER_INC
#define  PARSER_INC

#include "grammar.h"

struct expr* parse_expr();
void free_expr(struct expr *);

struct cond *parse_cond();
void free_cond(struct cond *);

struct single *parse_assign();
struct single *parse_if();
struct single *parse_loop();
struct single *parse_io();
struct single *parse_single();
void free_single(struct single*);

struct instruction *parse_instruction();
void free_instruction(struct instruction *);

struct vardecl *parse_decl();
void free_decl(struct vardecl *);

struct vars *parse_vars();
void free_vars(struct vars *);

struct program *parse_program();
void free_program(struct program *);

#endif   /* ----- #ifndef PARSER_INC  ----- */

