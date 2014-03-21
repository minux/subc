BINDIR	=	/u/bin

CC	=	scc
CFLAGS	=

all:		sar s86 sld snm xhd xstrip

xhd:	xhd.c exe.h
	$(CC) $(CFLAGS) -o xhd xhd.c

snm:	snm.c exe.h obj.h arc.h sym.h ufseek.o
	$(CC) $(CFLAGS) -o snm snm.c ufseek.o

xstrip:	xstrip.c obj.h exe.h ufseek.o
	$(CC) $(CFLAGS) -o xstrip xstrip.c ufseek.o

sld:	sld.c exe.h obj.h sym.h defs.h ufseek.o
	$(CC) $(CFLAGS) -o sld sld.c ufseek.o

s86:	s86.c sym.h obj.h defs.h ufseek.o
	$(CC) $(CFLAGS) -o s86 s86.c ufseek.o

sar:	sar.c sym.h obj.h arc.h ufseek.o
	$(CC) $(CFLAGS) -o sar sar.c ufseek.o

ufseek.o:	ufseek.c
	$(CC) $(CFLAGS) -c ufseek.c

install:	all
	strip sar s86 sld snm xstrip xhd
	cp sar s86 sld snm xstrip xhd $(BINDIR)

clean:
	rm -f s86 sar sld snm xstrip xhd *.o *.tmp *.a debug core *.core \
		a_out.exe
