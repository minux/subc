/*
 *	NMH's Simple C Compiler, 2013,2014
 *	NetBSD/x86-64 environment
 */

#define OS		"NetBSD"
#define ASCMD		"as -o %s %s"
#define LDCMD		"ld -o %s %s/lib/crt0.o"
#define SYSLIBC		"/usr/lib/libc.a"
