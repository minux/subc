/*
 *	SLD -- A Small Loader
 *	Nils M Holm, 1993,1995,2013
 *	In the public domain
 */


/*	Object file magic ID */
#define O_MAGIC	'O'

/*	Symbol table magic ID */
#define S_MAGIC	'S'

/*	Object file header	*/
#define OMAGIC	0
#define OPUBP	2
#define OEXTP	4
#define ODOSREL	6
#define ORELOC	8
#define OCODE	10
#define ODATA	12
#define ODATASZ	14

/*	size of Object File Header	*/
#define OHDSZ	16

