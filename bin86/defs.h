/*
 *	SLD -- A Small Loader
 *	Nils M Holm, 1993,1995,2013
 *	In the public domain
 */

/*	#define debug	/* produces a detailed debug file */

#define TMPDATA	"as00000d.tmp"

#define MAXLN	128

#define SYMSZ	10000

#define MACSZ	5000

#define MARKSZ	6000		/* 2 */

#define CODESZ	20000
#define DATASZ	65000

#define NDOSREL	200
#define NRELOC	8000

#ifndef __scc40__
#define index	strchr
#endif

