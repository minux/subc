
	.data

	extrn	hello

	.code

	mov	ax,DGROUP
	mov	ds,ax
	mov	es,ax
	mov	ss,ax
	mov	dx,offset hello
	mov	ah,9
	int	21h
	mov	ax,4c00h
	int	21h

