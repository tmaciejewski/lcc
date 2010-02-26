/* File: token.h
 * Desc: Definition of token and token_type
 * Author: Tomasz Maciejewski (ponton[at]jabster.pl)
 * Licence: GNU General Public Licence 3.0
*/

#ifndef  TOKENS_INC
#define  TOKENS_INC

typedef enum {
	T_NULL, /**/
	T_EOF,  /**/
	T_SEP,  /**/
	T_HAI,
	T_KTHXBYE,
	T_I,
	T_HAS,
	T_ITZ,
	T_NUMBER, /**/
	T_LOL,
	T_IZ,
	T_ID, /**/
	T_SUM,
	T_DIFF,
	T_PRODUKT,
	T_QUOSHUNT,
	T_MOD,
	T_BIGGR,
	T_SMALLR,
	T_OF,
	T_AN,
	T_VISIBLE,
	T_GIMMEH,
	T_STRING, /**/
	T_IM,
	T_IN,
	T_YR,
	T_LOOP,
	T_OUTTA,
	T_GTFO,
	T_O,
	T_RLY,
	T_QMARK, /**/
	T_YA,
	T_NO,
	T_WAI,
	T_OIC,
	T_EITHER,
	T_NOT,
	T_BOTH,
	T_SAEM,
	T_DIFFRINT
} token_type;

/*************************************/

typedef struct 
{
	token_type type;
	void *data;
	unsigned line, column;
} token;

#endif   /* ----- #ifndef TOKENS_INC  ----- */
