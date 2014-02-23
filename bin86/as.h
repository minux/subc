/*
 *	S86 -- A Small Assembler for the 8086
 *	Nils M Holm, 1993,1995,2013
 *	In the public domain
 */

/*	segments: Unknown, Data, Code	*/
#define SNONE	'X'
#define SDATA	'D'
#define SCODE	'C'

/*	Storage classes: Public, External, Local, ?	*/
#define PUBLIC	'P'
#define EXTRN	'E'
#define LOCAL	'L'
#define UNDEFD	'U'

/*	The Mark structure: Flags, Address, Ptr to next Mark, Size	*/
#define MKFLAG	0
#define MKADDR	2
#define MKPTR	4
#define MKSZ	6

/*	Mark Flags: Code address, Data address, short mark, long mark,
	relative address	*/
#define MKSHORT	1
#define MKREL	2

/*	Symbol entry structure	*/
#define SNAME	0
#define SSEGMT	15
#define SCLASS	16
#define SADDR	17
#define SLLIST	19
#define SSIZE	21
