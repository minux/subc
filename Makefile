SNAP=	20130518
REL=	20130829
ARC=	subc-$(SNAP).tgz
DIST=	subc-$(REL).tgz

csums:
	csum -u <_sums >_newsums ; mv -f _newsums _sums

sums:	clean
	find . -type f | grep -v _sums | csum >_sums

clean:
	cd src && make -f Makefile.unix clean
	cd bin86 && make clean
	rm -f ptest.c $(ARC) $(DIST)

arc:	clean
	tar cvfz $(ARC) *

dist:	clean
	(cd .. && tar cvfz $(DIST) subc) && mv ../$(DIST) .
