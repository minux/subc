;
;	NMH's Simple C Compiler, 2013,2014
;	C runtime module for DOS
;

; XXX	UNFINISHED!

DOS	equ	21h
BPW	equ	2
CMDLEN	equ	128
PMTRS	equ	64
PSPCL	equ	81h
ENVPTR	equ	2ch
STKTOP	equ	0fffeh
STKLEN	equ	800h

	.model small

	.stack	STKLEN		; dummy stack

	.data

	extrn	Cerrno
	public	Cenviron

Cenviron	dw	0

	.code

	extrn	Cmain,	Cexit
	public	switch, C_exit
	public	C_creat, C_open, C_close, C_read, C_write
	public	C_lseek, C_unlink, C_rename, C_time
	public	C_sbrk, Craise

; Initial entry point is here

crun:
	mov	bx,8192		; set program memory to 128K ; XXX
	mov	ah,4ah
	int	DOS
	jnc	setupheap

error:
	mov	dx,offset memerror
	push	cs
	pop	ds
	mov	ah,9
	int	DOS
	mov	ax,4c7fh
	int	DOS

; At this point:
; CS = code, DS = ES = PSP, SS = end of static data
; DGROUP = beginning of static data

setupheap:
	mov	dx,ss
	mov	ax,ds
	sub	dx,ax		; dx = length of static data
	mov	cl,4
	shl	dx,cl
	mov	ax,ss
	shr	ax,cl
	mov	cs:arena,ax	; beginning of malloc() arena
	mov	cs:break,ax
	xor	ax,ax		
	sub	ax,dx
	sub	ax,STKLEN
	cmp	ax,7fffh	; assert arena size <= INT_MAX
	jbe	setmem
	mov	ax,7fffh
setmem:	mov	cs:maxmem,ax	; size of arena
	mov	ax,DGROUP
	mov	ds,ax
	mov	ss,ax
	mov	sp,STKTOP	; set DS = SS = DGROUP

; At this point:
; CS = code, DS = SS = DGROUP, ES = PSP

; copy the command line from the PSP

	sub	sp,CMDLEN	; reserve space for argc,argv
	sub	sp,PMTRS
	mov	di,sp
	add	di,BPW		; point to reserved area
	mov	si,PSPCL	; command line in PSP
	mov	bx,di		; bx = start of command line
	mov	cx,CMDLEN	; copy CMDLEN bytes

	call	swapsegs
	cld
	rep
	movsb
	call	swapsegs

; parse the command line and build argv

	mov	si,di		; save &argv[0]
	mov	word ptr [si],0	; clear argv[0]
	inc	si
	inc	si
	mov	cx,1		; preset argc

nextch:	mov	al,[bx]		; get char from cmd line
	inc	bx
	cmp	al,0dh		; end of line ?
	jz	argsdone

	cmp	al,20h		; skip white spaces
	jz	nextch
	cmp	al,9
	jz	nextch

	dec	bx
	mov	[si],bx		; save argv[n]
	inc	bx
	inc	si
	inc	si
	inc	cx		; increment argc

getend:	mov	al,[bx]		; search end of current arg
	cmp	al,20h
	jz	endarg
	cmp	al,9
	jz	endarg
	cmp	al,0dh		; CR: end of line reached, stop parsing
	jz	argsdone
	inc	bx
	jmp	short getend

endarg:	mov	byte ptr [bx],0
	inc	bx
	jmp	nextch

argsdone:
	mov	byte ptr [bx],0	; terminate last arg
	mov	word ptr [si],0	; terminate argv[] with NULL
	push	cx		; pass argc
	push	di		; and argv to main()
	mov	ax,2		; __argc
	push	ax

; copy environment

setupenv:
	mov	si,ENVPTR	; get environ ptr from PSP
	mov	ax,es:[si]
	mov	es,ax

	xor	bx,bx		; get length of env
	xor	dx,dx		; and number of entries
nxtenv:	mov	al,es:[bx]
	inc	bx
	or	al,al
	jnz	nxtenv
	inc	dx
	mov	al,es:[bx]
	inc	bx
	or	al,al
	jnz	nxtenv

	cmp	bx,cs:maxmem
	jbe	copyenv
	jmp	error		; not enough space

copyenv:
	sub	sp,bx		; space for environ[]
	sub	sp,dx
	sub	sp,dx
	sub	sp,2

	mov	di,sp		; copy environment
	xor	si,si
	mov	cx,bx

	call	swapsegs
	rep
	movsb
	push	es
	pop	ds

	mov	Cenviron,di

	mov	si,sp		; build environ[]
	inc	si
store:	dec	si
	mov	ax,si
	stosw
scan:	lodsb
	or	al,al
	jnz	scan
	lodsb
	or	al,al
	jnz	store
	xor	ax,ax
	stosw

; initialize run time library

	call	near ptr C_init

; call main() and exit

	call	near ptr Cmain
	add	sp,6
	push	ax
	mov	ax,1
	push	ax
;	call	near ptr Cexit	; XXX
	call	near ptr C_exit

; swap es,dx

swapsegs:
	mov	dx,es		; swap es,ds
	mov	ax,ds
	mov	ds,dx
	mov	es,ax
	ret

; void _exit(int rc);

C_exit:	mov	bx,sp
	mov	ax,[bx+4]
	mov	ah,4ch		; terminate program
	int	DOS

; evaluate 'switch' statement XXX

switch:	pop	si		; get offset of case table
docase:	mov	dx,cs:[si]	; fetch value from table
	add	si,BPW		; advance tbl ptr
	mov	bx,cs:[si]	; fetch address from table
	or	bx,bx		; end of table ?
	jz	dodflt		; yup, do default
	add	si,BPW
	cmp	ax,dx		; right case ?
	jnz	docase		; no, go on
	jmp	bx		; yes: jump to it
dodflt:	mov	bx,dx		; do default
	jmp	bx

; int _creat(char *name, int perms);

C_creat:mov	bx,sp
	mov	cx,0
	mov	dx,[bx+6]
	mov	ah,3ch
	int	DOS
	jnc	crok
	xor	ax,ax
	dec	ax
crok:	ret

; int _open(char *name, int mode);

C_open:	mov	bx,sp
	mov	ax,[bx+4]
	mov	ah,3dh
	mov	dx,[bx+6]
	int	DOS
	jnc	opok
	xor	ax,ax
	dec	ax
opok:	ret

; int _close(int fd);

C_close:
	mov	bx,sp
	mov	bx,[bx+4]
	mov	ah,3eh
	int	DOS
	jnc	clok
	xor	ax,ax
	dec	ax
	ret
clok:	xor	ax,ax
	ret

; int _read(int fd, void *buf, int len);

C_read:
	mov	bx,sp
	mov	cx,[bx+4]
	mov	dx,[bx+6]
	mov	bx,[bx+8]
	mov	ah,3fh
	int	DOS
	jnc	rdok
	xor	ax,ax
	dec	ax
rdok:	ret

; int _write(int fd, void *buf, int len);

C_write:
	mov	bx,sp
	mov	cx,[bx+4]
	mov	dx,[bx+6]
	mov	bx,[bx+8]
	mov	ah,40h
	int	DOS
	jnc	wrok
	xor	ax,ax
	dec	ax
wrok:	ret

C_sbrk:	mov	bx,sp
	mov	ax,[bx+4]
	add	ax,cs:break
	cmp	ax,cs:maxmem
	ja	sbfail
	mov	ax,cs:break
	mov	dx,ax
	add	ax,[bx+4]
	mov	cs:break,ax
	mov	ax,dx
	ret
sbfail:	xor	ax,ax
	dec	ax
	ret

	.data

abort	db	"crt0: program aborted.", 13, 10, '$'

	.code

; int raise(int sig);

Craise:	mov	dx,offset abort
	mov	ah,9
	int	DOS
	mov	ax,4c7fh
	int	DOS
	ret

; int _lseek(int fd, int where, int how);

C_lseek:
	mov	bx,sp
	mov	ax,[bx+6]
	cwd
	mov	cx,dx
	mov	dx,ax
	mov	ax,[bx+4]
	mov	bx,[bx+8]
	mov	ah,42h
	int	DOS
	jnc	lsok
	xor	ax,ax
	dec	ax
lsok:	ret

; int _unlink(char *name);

C_unlink:
	mov	bx,sp
	mov	dx,[bx+4]
	mov	ah,41h
	int	DOS
	jnc	ulok
	xor	ax,ax
	dec	ax
	ret
ulok:	xor	ax,ax
	ret

; int _rename(char *old, char *new);

C_rename:
	mov	bx,sp
	mov	dx,[bx+6]
	mov	di,[bx+4]
	mov	ah,56h
	jnc	rnok
	xor	ax,ax
	dec	ax
	ret
rnok:	xor	ax,ax
	ret

; int _time(void);

C_time:
	xor	ax,ax
	ret

; internal data (in code segment)

arena:		dw	0
break:		dw	0
maxmem:		dw	0

memerror:	db	"crt0: out of memory", 13, 10, '$'

