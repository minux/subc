/*
 *	NMH's Simple C Compiler, 2011--2014
 *	Definitions
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "cg.h"
#include "sys.h"

#define VERSION		"2014-04-25"

#ifndef SCCDIR
 #define SCCDIR		"."
#endif

#ifndef AOUTNAME
 #define AOUTNAME	"a.out"
#endif

#define SCCLIBC		"%s/lib/libscc.a"

#define PREFIX		'C'
#define LPREFIX		'L'

#define INTSIZE		BPW
#define PTRSIZE		INTSIZE
#define CHARSIZE	1

#define TEXTLEN		512
#define NAMELEN		16

#define MAXFILES	32

#define MAXIFDEF	16
#define MAXNMAC		32
#define MAXCASE		256
#define MAXBREAK	16
#define MAXLOCINIT	32
#define MAXFNARGS	32

/* assert(NSYMBOLS < PSTRUCT) */
#define NSYMBOLS	1024
#define POOLSIZE	16384

/* types */
enum {
	TVARIABLE = 1,
	TARRAY,
	TFUNCTION,
	TCONSTANT,
	TMACRO,
	TSTRUCT
};

/* primitive types */
enum {
	PCHAR = 1,
	PINT,
	CHARPTR,
	INTPTR,
	CHARPP,
	INTPP,
	PVOID,
	VOIDPTR,
	VOIDPP,
	FUNPTR,
	PSTRUCT = 0x2000,
	PUNION  = 0x4000,
	STCPTR  = 0x6000,
	STCPP   = 0x8000,
	UNIPTR  = 0xA000,
	UNIPP   = 0xC000,
	STCMASK = 0xE000
};

/* storage classes */
enum {
	CPUBLIC = 1,
	CEXTERN,
	CSTATIC,
	CLSTATC,
	CAUTO,
	CSPROTO,
	CMEMBER,
	CSTCDEF
};

/* lvalue structure */
enum {
	LVSYM,
	LVPRIM,
	LV
};

/* debug options */
enum {
	D_LSYM = 1,
	D_GSYM = 2,
	D_STAT = 4
};

/* addressing modes */
enum {
	empty,
	addr_auto,
	addr_static,
	addr_globl,
	addr_label,
	literal,
	auto_byte,
	auto_word,
	static_byte,
	static_word,
	globl_byte,
	globl_word
};

/* compare instructions */
enum {
	cnone,
	equal,
	not_equal,
	less,
	greater,
	less_equal,
	greater_equal,
	below,
	above,
	below_equal,
	above_equal
};

/* boolean instructions */
enum {
	bnone,
	lognot,
	normalize
};

/* tokens */
enum {
	SLASH, STAR, MOD, PLUS, MINUS, LSHIFT, RSHIFT,
	GREATER, GTEQ, LESS, LTEQ, EQUAL, NOTEQ, AMPER,
	CARET, PIPE, LOGAND, LOGOR,

	ARROW, ASAND, ASXOR, ASLSHIFT, ASMINUS, ASMOD, ASOR, ASPLUS,
	ASRSHIFT, ASDIV, ASMUL, ASSIGN, AUTO, BREAK, CASE, CHAR, COLON,
	COMMA, CONTINUE, DECR, DEFAULT, DO, DOT, ELLIPSIS, ELSE, ENUM,
	EXTERN, FOR, IDENT, IF, INCR, INT, INTLIT, LBRACE, LBRACK,
	LPAREN, NOT, QMARK, RBRACE, RBRACK, REGISTER, RETURN, RPAREN,
	SEMI, SIZEOF, STATIC, STRLIT, STRUCT, SWITCH, TILDE, UNION,
	VOID, VOLATILE, WHILE, XEOF, XMARK,

	P_DEFINE, P_ELSE, P_ELSENOT, P_ENDIF, P_ERROR, P_IFDEF,
	P_IFNDEF, P_INCLUDE, P_LINE, P_PRAGMA, P_UNDEF
};
