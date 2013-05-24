/*
 *	SLD -- A Small Loader
 *	Nils M Holm, 1993,1995,2013
 *	In the public domain
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "defs.h"
#include "ar.h"
#include "as.h"
#include "sld.h"
#include "x_out.h"
#include "ld.h"

int	debug = 0;

FILE	*Objf, *Ctmp, *Dtmp, *Exef;
char	Outfile[81];
int	Errors;
char	Ohd[OHDSZ];
char	Xhd[XHDR_SZ];
int	Codep, Datap;
int	Stkinit;

char	*Libdir;

char	*Buffer;
char	*Symtab;	/* Shared extrn/public */
char	*Extsym;
char	*Reloc;
int	N_dosrel;

char	Arh[ARH_LEN];

void error(char *m, char *n) {
	if (n)
		printf("sld: %s: %s\n", m, n);
	else
		printf("sld: %s\n", m);
	Errors++;
}

void fatal(char *m) {
	printf("sld: fatal error: ");
	error(m, NULL);
	if (!debug) {
		remove(LDCODE);
		remove(LDDATA);
	}
	exit(1);
}

void readerr(void) {
	fatal("file read error");
}

void writeerr(void) {
	fatal("file write error");
}

void init(void) {
	int	i;
	char	*nomem;
	char	*notmp;

	nomem = "not enough memory";
	notmp = "could not create temp file: ld?00000";
	if ((Symtab = malloc(SYM_SZ)) == NULL)
		fatal(nomem);
	if ((Extsym = malloc(EXT_SZ)) == NULL)
		fatal(nomem);
	if ((Reloc = malloc(REL_SZ)) == NULL)
		fatal(nomem);
	if ((Buffer = malloc(BUF_SZ)) == NULL)
		fatal(nomem);
	memset(Symtab, 0, SYM_SZ);
	memset(Extsym, 0, EXT_SZ);
	memset(Reloc, 0, REL_SZ);
	memset(Buffer, 0, BUF_SZ);
	if ((Ctmp = fopen(LDCODE, "wb+")) == NULL)
		fatal(notmp);
	if ((Dtmp = fopen(LDDATA, "wb+")) == NULL)
		fatal(notmp);
	if ((Exef = fopen(Outfile, "wb+")) == NULL) {
		error("could not create output file", Outfile);
		fatal("aborting");
	}
	for (i = 0; i<SYM_SZ; i += SSIZE)
		Symtab[i] = 0;
	for (i=0; i < EXT_SZ-MKSZ; i += MKSZ)
		Extsym[i+MKADDR] = Extsym[i+MKADDR+1] = 0;
	N_dosrel = 0;
	Codep = Datap = 0;
	Stkinit = 1024;
}

int addsym(char *buf) {
	int	i, add, pub;

	pub = (buf[SCLASS]==PUBLIC);
	for (add=-1,i=0; i<SYM_SZ; i+=SSIZE) {
		if (!Symtab[i]) {
			if (add != -1) continue;
			memcpy(&Symtab[i], buf, SSIZE);
			Symtab[i+SLLIST] = Symtab[i+SLLIST+1] = 0xff;
			if (debug)
				printf("add sym:\t%s (%c,%c) = %04x\n",
					buf, buf[SSEGMT], buf[SCLASS],
					buf[SADDR] + (buf[SADDR+1] << 8));
			add = i;
		}
		else if (pub) {
			if (	!strcmp(&Symtab[i], buf) &&
				Symtab[i+SCLASS] == PUBLIC
			)
				error("duplicate symbol", buf);
		}
	}
	if (add != -1) return add;
	fatal("symbol table overflow");
	return -1;
}

void mkmark(int si, char *ebuf) {
	int	a, i;

	for (i=0; i<EXT_SZ; i+= MKSZ) {
		if (Extsym[i+MKADDR] == 0 && Extsym[i+MKADDR+1] == 0) {
			a = Symtab[si+SLLIST] + (Symtab[si+SLLIST+1] << 8);
			Extsym[i+MKPTR] = a;
			Extsym[i+MKPTR+1] = a>>8;
			Symtab[si+SLLIST] = i;
			Symtab[si+SLLIST+1] = (i >> 8);
			Extsym[i+MKFLAG] = ebuf[MKFLAG];
			Extsym[i+MKADDR] = ebuf[MKADDR];
			Extsym[i+MKADDR+1] = ebuf[MKADDR+1];
			if (debug)
				printf("mark:\t%s\tF=%02x\tA=%04x\tP=%04x\n",
					&Symtab[si], Extsym[i+MKFLAG],
					Extsym[i+MKADDR] +
						(Extsym[i+MKADDR+1]<<8),
					Extsym[i+MKPTR] +
						(Extsym[i+MKPTR]<<8));
			return;
		}
	}
	fatal("mark table overflow");
}

void load(char *name) {
	int	i, n, off, si, nrel, old, cmp, size, t, k, a;
	char	*paddr;
	char	buf[SSIZE];
	char	ee[MKSZ];
	char	drel[2];

	if (fread(Ohd, 1, OHDSZ, Objf) != OHDSZ) {
		error("bad object file (short file)", name);
		return;
	}
	if (Ohd[OMAGIC] != O_MAGIC) {
		error("bad object file (magic match failed)", name);
		return;
	}
	off = OHDSZ;
	while (off < Ohd[OEXTP] + (Ohd[OEXTP+1]<<8)) {
		if (fread(buf, 1, SSIZE, Objf) != SSIZE)
			readerr();
		paddr = &buf[SADDR];
		k = paddr[0] + (paddr[1]<<8);
		k += buf[SSEGMT] == SCODE? Codep: Datap;
		paddr[0] = k;
		paddr[1] = k>>8;
		addsym(buf);
		off += SSIZE;
	}
	while (off < Ohd[ODOSREL] + (Ohd[ODOSREL+1]<<8)) {
		if (fread(buf, 1, SSIZE, Objf) != SSIZE)
			readerr();
		si = addsym(buf);
		off += SSIZE;
		fread(ee, 1, MKSZ, Objf);
		off += MKSZ;
		while (ee[MKADDR] != 0xff && ee[MKADDR+1] != 0xff) {
			k = ee[MKADDR] + (ee[MKADDR+1]<<8);
			k += Codep;
			ee[MKADDR] = k;
			ee[MKADDR+1] = k>>8;
			mkmark(si, ee);
			fread(ee, 1, MKSZ, Objf);
			off += MKSZ;
		}
	}
	while (off < Ohd[ORELOC] + (Ohd[ORELOC+1]<<8)) {
		if (fread(drel, 1, 2, Objf) != 2) readerr();
		off += 2;
		n = Codep + drel[0] + (drel[1]<<8);
		if (debug) printf("DOS reloc:\t%04x\n", n);
		drel[0] = n;
		drel[1] = n>>8;
		if (fwrite(drel, 1, 2, Exef) != 2) writeerr();
		drel[0] = drel[1] = 0;
		if (fwrite(drel, 1, 2, Exef) != 2) writeerr();
		N_dosrel++;
	}

	n = Ohd[OCODE] + (Ohd[OCODE+1]<<8) - off;
	off += n;
	nrel = n/3;
	if (fread(Reloc, 1, n, Objf) != n) readerr();
	if (debug) printf("%d RELOC entries\n", nrel);
	n = Ohd[ODATA] + (Ohd[ODATA+1]<<8) - off;
	off += n;
	if (n >= BUF_SZ) fatal("text area too large");
	if (fread(Buffer, 1, n, Objf) != n) readerr();
	if (debug) {
		printf("%d TEXT bytes\n", n);
		printf("Relocating...\n");
	}
	for (nrel*=3, i=0; i<nrel; i+=3) {
		t = Reloc[i];
		paddr = &Buffer[a = Reloc[i+1] + (Reloc[i+2]<<8)];
		k = paddr[0] + (paddr[1]<<8);
		k += (t == SCODE? Codep: Datap);
		paddr[0] = k;
		paddr[1] = k>>8;
		if (debug)
			printf(" %04x%c%s",
				a, (char) t, !(i%48)&&i? "\n": "");
	}
	if (debug) putchar('\n');
	if (fwrite(Buffer, 1, n, Ctmp) != n) writeerr();
	old = Codep;
	cmp = (Codep += n);
	if (cmp < old) fatal("code segment overflow");
	for (i=size=Ohd[ODATASZ] + (Ohd[ODATASZ+1]<<8); i; ) {
		if (i >= BUF_SZ || i < 0)
			n = BUF_SZ;
		else
			n = i;
		if (fread(Buffer, 1, n, Objf) != n) readerr();
		if (fwrite(Buffer, 1, n, Dtmp) != n) writeerr();
		off += n;
		i -= n;
	}
	old = Datap;
	cmp = (Datap += size);
	if (cmp < old) fatal("data segment overflow");
	if (debug) printf("%d DATA bytes\n", size);
}

void resolve(int ptr, int addr, int class) {
	int	i, old;
	int	va, la;

	i = 0;
	while (ptr != 0xffff) {
		la = Extsym[ptr+MKADDR] + (Extsym[ptr+MKADDR+1]<<8);
		if (debug)
			printf(" %04x%s%s",
				la, (Extsym[ptr+MKFLAG] & MKREL)? "R": "A",
				!(i%15)&&i++? "\n": "");
		fseek(Ctmp, la, SEEK_SET);
		/*if (class == SCODE) {*/
		if (Extsym[ptr+MKFLAG] & MKREL) {
			va = addr-la-2;
			fputc(va & 0xff, Ctmp);
			fputc(va >> 8, Ctmp);
		}
		else {
			va = (fgetc(Ctmp) & 0xff);
			va += (fgetc(Ctmp) << 8);
			va += addr;
			fseek(Ctmp, la, SEEK_SET);
			fputc(va & 0xff, Ctmp);
			fputc(va >> 8, Ctmp);
		}
		old = ptr;
		ptr = Extsym[ptr+MKPTR] + (Extsym[ptr+MKPTR+1]<<8);
		Extsym[old+MKADDR] = 0;
	}
	fseek(Ctmp, 0, SEEK_END);
	clearerr(Ctmp);
	if (debug) putchar('\n');
}

void try_reslv(void) {
	int	i, j, ptr, addr;

	for (i=0; i<SYM_SZ; i+=SSIZE) {
		if (Symtab[i+SCLASS] == EXTRN) {
			for (j=0; j<SYM_SZ; j+=SSIZE) {
				if (Symtab[j+SCLASS] == PUBLIC) {
					if (!strcmp(&Symtab[i], &Symtab[j])) {
						ptr = (Symtab[i+SLLIST]&0xff)+
							(Symtab[i+SLLIST+1]<<8);
						addr = (Symtab[j+SADDR]&0xff)+
							(Symtab[j+SADDR+1]<<8);
						if (debug)
							printf(
					"resolving extern:\t%s (%04x) ...\n",
							&Symtab[i], addr);
						resolve(ptr, addr,
						Symtab[j+SSEGMT]);
						Symtab[i] = Symtab[i+SCLASS] = 0;
						break;
					}
				}
			} /* <-- break goes here */
		}
	}
}

void mkxhdr(int valid) {
	int	k;

	rewind(Exef);
	memcpy((char *)&Xhd[X_MAGIC], valid? XMAGIC: "__", 2);
	Xhd[X_NRELOC] = N_dosrel;
	Xhd[X_NRELOC+1] = N_dosrel>>8;
	k = ((XORELOC + (N_dosrel<<2)) >> 4)+1;
	k = (k + 31) / 32 * 32;
	Xhd[X_HDSIZE] = k;
	Xhd[X_HDSIZE+1] = k>>8;
	Xhd[X_MAXAL] = XMAXAL;
	Xhd[X_MAXAL+1] = XMAXAL>>8;
	Xhd[X_STKPTR] = Stkinit;
	Xhd[X_STKPTR+1] = Stkinit>>8;
	Xhd[X_CKSUM] = XCKSUM;
	Xhd[X_CKSUM+1] = XCKSUM>>8;
	Xhd[X_ENTRY] = XENTRY;
	Xhd[X_ENTRY+1] = XENTRY>>8;
	Xhd[X_CSEG] = XCSEG;
	Xhd[X_CSEG+1] = XCSEG>>8;
	Xhd[X_ORELOC] = XORELOC;
	Xhd[X_ORELOC+1] = XORELOC>>8;
	Xhd[X_OVLID] = XOVL;
	Xhd[X_OVLID+1] = XOVL>>8;
	if (fwrite(Xhd, 1, XHDR_SZ, Exef) != XHDR_SZ)
		writeerr();
}

void bind(void) {
	int	i, n, hdsz, seg2;
	int	t, o, k;
	char	*relp, *slp;

	for (i=0; i<SYM_SZ; i+=SSIZE) {
		if (Symtab[i+SCLASS] == EXTRN) {
			slp = &Symtab[i+SLLIST];
			if (*slp != 0xff || slp[0] != 0xff)
				error("unresolved external", &Symtab[i]);
		}
	}
	while ((Codep++)%16)
		fputc(0, Ctmp);
	for (hdsz=(N_dosrel<<2)+XORELOC; hdsz%512; hdsz++)
		fputc(0, Exef);
	rewind(Ctmp);
	rewind(Dtmp);
	n = fread(Buffer, 1, BUF_SZ, Ctmp);
	while (n) {
		fwrite(Buffer, 1, n, Exef);
		n = fread(Buffer, 1, BUF_SZ, Ctmp);
	}
	n = fread(Buffer, 1, BUF_SZ, Dtmp);
	while (n) {
		fwrite(Buffer, 1, n, Exef);
		n = fread(Buffer, 1, BUF_SZ, Dtmp);
	}
	fclose(Ctmp);
	fclose(Dtmp);
	seg2 = 0;
	o = t = Codep;
	t += Datap;
	if (o > t) seg2 = 1;
	t += hdsz;
	if (o > t) seg2 = 1;
	k = t/512+(seg2? 128: 0)+1;
	Xhd[X_NPAGES] = k;
	Xhd[X_NPAGES+1] = k>>8;
	k = t%512-1;
	Xhd[X_NBYTES] = k;
	Xhd[X_NBYTES+1] = k>>8;
	Datap = (Datap>>4) & 0xfff;
	Codep = (Codep>>4) & 0xfff;
	Xhd[X_MINAL] = Datap+1;
	Xhd[X_MINAL+1] = Datap+1>>8;
	k = Codep + Datap + 1;
	Xhd[X_SSEG] = k;
	Xhd[X_SSEG+1] = k>>8;
	fseek(Exef, XORELOC, SEEK_SET);
	if (fread(Buffer, 1, N_dosrel<<2, Exef) != (N_dosrel<<2)) readerr();
	for (i=0, relp=Buffer; i<N_dosrel; i++) {
		n = *relp++ + hdsz;
		relp++;
		fseek(Exef, n, SEEK_SET);
		fputc(Codep&0xff, Exef);
		fputc(Codep>>8, Exef);
	}
}

void wr_symtab(void) {
	int	i;

	fseek(Exef, 0, SEEK_END);
	i = S_MAGIC;
	fputc(S_MAGIC, Exef);
	fputc(0, Exef);
	for (i=0; i<SYM_SZ; i += SSIZE)
		if (Symtab[i])
			if (fwrite(&Symtab[i], 1, SSIZE, Exef) != SSIZE)
				writeerr();
}

void usage(void) {
	printf("usage: ld [-dsv] [-o outfile] [-L libdir] [-f listfile] "
		"[file ...]"
		" [-llib ...]\n");
	exit(1);
}

char *islib(char *name) {
	static char	lib[81];
	int		len;

	if ((len = strlen(name)) < 2) return NULL;
	if (!strcmp(&name[len-2], ".a")) return name;
	if (strncmp(name, "-l", 2)) return NULL;
	strcpy(lib, Libdir);
	strcat(lib, "/l");
	strcat(lib, name+2);
	strcat(lib, ".a");
	return lib;
}

void loadlib(char *name, int verbose) {
	FILE	*libf;
	int	hdlen, ptr, i, k;
	char	*ip;
	int	pos;
	char	sym[SSIZE];
	int	res_done;

	if ((libf = fopen(name, "rb")) == NULL) {
		error("cannot open library", name);
		return;
	}
	if (fread(Arh, 1, ARH_LEN, libf) != ARH_LEN) readerr();
	if (strncmp(&Arh[AR_MAGIC], A_MAGIC, 2)) {
		error("not a library", name);
		return;
	}
	if (strcmp(&Arh[AR_NAME], SYMDEF)) {
		error("library has no symbol index", name);
		return;
	}
	ip = &Arh[AR_SIZE];
	hdlen = (ip[0] + (ip[1]<<8) + 15) & 0xfff0;
	for (res_done = 1; res_done; ) {
		res_done = 0;
		ptr = ARH_LEN;
		for (; ptr < hdlen+ARH_LEN; ptr += SSIZE) {
			fseek(libf, 1*ptr, SEEK_SET);
			if (fread(sym, 1, SSIZE, libf) != SSIZE) readerr();
			for (i=0; i<SYM_SZ; i+=SSIZE) {
				if (Symtab[i+SCLASS] == EXTRN) {
					if (!strcmp(&Symtab[i], sym)) {
						ip = &sym[SADDR];
						k = ip[0] + (ip[1]<<8);
						pos = (k<<4) + hdlen;
						fseek(libf, pos, SEEK_SET);
						if (fread(Arh, 1, ARH_LEN,
						    libf) != ARH_LEN)
							readerr();
						if (verbose)
							printf(
							"loading %s from %s\n",
							&Arh[AR_NAME], name);
						if (strncmp(&Arh[AR_MAGIC],
						    A_MAGIC, 2))
							fatal(
							"bad archive member");
						Objf = libf;
						load(&Arh[AR_NAME]);
						try_reslv();
						clearerr(libf);
						res_done = 1;
					}
				}
			}
		}
	}
	fclose(libf);
}

int main(int argc, char **argv) {
	char	*p, *listfile, buf[81];
	FILE	*list;
	int	verbose, strip;
	int	i;

	strcpy(Outfile, "a_out.exe");
	Libdir = ".";
	strip = verbose = 0;
	listfile = NULL;
	for (i=1; i < argc && *argv[i] == '-'; i++) {
		p = argv[i];
		p++;
		if (*p == 'l') break;
		while (*p) switch(*p++) {
			case 'd':	debug = 1; break;
			case 'v':	verbose = 1; break;
			case 's':	strip = 1; break;
			case 'f':	if (*p == 0) {
						if ((p = argv[++i]) == NULL)
							usage();
					}
					listfile = p;
					p += strlen(p);
					break;
			case 'o':	if (*p == 0) {
						if ((p = argv[++i]) == NULL)
							usage();
					}
					strcpy(Outfile, p);
					p += strlen(p);
					break;
			case 'L':	if (*p == 0) {
						if ((p = argv[++i]) == NULL)
							usage();
					}
					Libdir = p;
					p += strlen(p);
					break;
			default:	usage();
		}
	}
	if (i >= argc && !listfile) usage();
	init();
	Errors = 0;
	mkxhdr(0);
	for (; i < argc; i++) {
		p = argv[i];
		if (islib(p)) break;
		if (verbose) printf("loading %s\n", p);
		if ((Objf = fopen(p, "rb")) == NULL) {
			error("no such file", p);
			continue;
		}
		load(p);
		fclose(Objf);
		try_reslv();
	}
	if (listfile) {
		if ((list = fopen(listfile, "r")) == NULL)
			error("cannot open listfile", listfile);
		else {
			fgets(buf, 81, list);
			while (!feof(list)) {
				buf[strlen(buf)-1] = 0;
				if ((p = islib(buf)) != NULL) {
					if (verbose)
						printf("loading library: %s\n",
						p);
					loadlib(p, verbose);
				}
				else {
					if (verbose)
						printf("loading %s\n", buf);
 					if ((Objf = fopen(buf,"rb")) == NULL) {
						error("no such module", buf);
					}
					else {
						load(buf);
						fclose(Objf);
						try_reslv();
					}
				}
				fgets(buf, 81, list);
			}
			fclose(list);
		}
	}
	for (; i < argc; i++) {
		if ((p = islib(argv[i])) == NULL)
			error("bad library", argv[i]);
		if (verbose) printf("loading library: %s\n", p);
		loadlib(p, verbose);
	}
	bind();
	if (!debug) {
		remove(LDCODE);
		remove(LDDATA);
	}
	mkxhdr(1);
	if (!strip) wr_symtab();
	fclose(Exef);
	if (Errors) {
		printf("%d error(s)\n", Errors);
		remove(Outfile);
		return 1;
	}
	return 0;
}
