/*
 *	SAR -- A Small Archiver
 *	Nils M Holm, 1993,1995,2013
 *	In the public domain
 */

#define BLKSIZE	16	/* Blocksize, Max archive size == BLKSIZE*65536 */

#define ARH_LEN	32	/* Must be a multiple of BLKSIZE ! */

/*	AR header structure	*/
#define AR_MAGIC	0
#define AR_NAME		2
#define AR_SIZE		14

/*	AR file magic	*/
#define A_MAGIC		"AR"

/*	The name of the extended dictionary entry	*/
#define SYMDEF		"__SYMDEF.__"
