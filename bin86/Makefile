BINDIR	=	/u/bin

CC	=	scc
CFLAGS	=

forget-it:
	cat WARNING

all:		sar s86 sld snm xhd xstrip

xhd:	xhd.c x_out.h
	$(CC) $(CFLAGS) -o xhd xhd.c

snm:	snm.c x_out.h sld.h ar.h as.h
	$(CC) $(CFLAGS) -o snm snm.c

xstrip:	xstrip.c sld.h x_out.h
	$(CC) $(CFLAGS) -o xstrip xstrip.c

sld:	sld.c x_out.h sld.h as.h ld.h defs.h
	$(CC) $(CFLAGS) -o sld sld.c

s86:	s86.c as.h sld.h defs.h
	$(CC) $(CFLAGS) -o s86 s86.c

sar:	sar.c as.h sld.h ar.h
	$(CC) $(CFLAGS) -o sar sar.c

install:	all
	strip sar s86 sld snm xstrip xhd
	cp sar s86 sld snm xstrip xhd $(BINDIR)

clean:
	rm -f s86 sar sld snm xstrip xhd *.o *.tmp *.a debug core *.core \
		a_out.exe
