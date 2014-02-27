SNAP=	20140202
REL=	20140227
ARC=	subc-$(SNAP).tgz
DIST=	subc-$(REL).tgz

default:
	@echo "Use ./configure instead."

csums:
	csum -u <_sums >_newsums ; mv -f _newsums _sums

sums:	clean
	find . -type f | grep -v _sums | csum >_sums

version:
	vi src/defs.h Makefile README

clean:
	cd src && make clean
	cd bin86 && make clean
	rm -f tests/ptest.c $(ARC) $(DIST)
	if [ -f src/Makefile.ORIG ]; then \
		mv -f src/Makefile.ORIG src/Makefile; \
	fi

arc:	clean
	tar cvfz $(ARC) *

dist:	clean
	(cd .. && tar cvfz $(DIST) subc) && mv ../$(DIST) .
