BINDIR	=	/u/bin

CC	=	scc
CFLAGS	=

all:		sar s86 sld snm xhd xstrip

xhd:	xhd.c exe.h
	$(CC) $(CFLAGS) -o xhd xhd.c

snm:	snm.c exe.h obj.h arc.h sym.h
	$(CC) $(CFLAGS) -o snm snm.c

xstrip:	xstrip.c obj.h exe.h
	$(CC) $(CFLAGS) -o xstrip xstrip.c

sld:	sld.c exe.h obj.h sym.h defs.h
	$(CC) $(CFLAGS) -o sld sld.c

s86:	s86.c sym.h obj.h defs.h
	$(CC) $(CFLAGS) -o s86 s86.c

sar:	sar.c sym.h obj.h arc.h
	$(CC) $(CFLAGS) -o sar sar.c

install:	all
	strip sar s86 sld snm xstrip xhd
	cp sar s86 sld snm xstrip xhd $(BINDIR)

clean:
	rm -f s86 sar sld snm xstrip xhd *.o *.tmp *.a debug core *.core \
		a_out.exe
