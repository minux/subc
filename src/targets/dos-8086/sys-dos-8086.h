/*
 *	NMH's Simple C Compiler, 2013,2014
 *	DOS/8086 environment
 */

#define OS		"DOS"
#define ASCMD		"s86 -o %s %s"
#define LDCMD		"sld -o %s %s/lib/crt0.o"
#define SYSLIBC		""
