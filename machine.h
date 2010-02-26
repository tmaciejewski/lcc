/* File: machine.h 
 * Desc: Stack machine language definition
 * Author: Tomasz Maciejewski (ponton[at]jabster.pl)
 * Licence: GNU General Public Licence 3.0
*/

#ifndef  MACHINE_INC
#define  MACHINE_INC

#define ASM_NIL			0
#define ASM_PUSH		1
#define ASM_ADD			2
#define ASM_SUB			3
#define ASM_MUL			4
#define ASM_DIV			5
#define ASM_MOD			6
#define ASM_MAX			7
#define ASM_MIN			8
#define ASM_JMP			9
#define ASM_JZERO		10
#define ASM_JNZERO		11
#define ASM_LVALUE		12
#define ASM_RVALUE		13
#define ASM_ASSIGN		14
#define ASM_PRINT		15
#define ASM_PRINTSTRING 16
#define ASM_GET			17
#define ASM_NOT			18
#define ASM_EQ			19
#define ASM_HALT		20

struct asm_header
{
	int stack_size;
	int code_size;
};

#endif   /* ----- #ifndef MACHINE_INC  ----- */

