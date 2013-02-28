#
#	NMH's Simple C Compiler, 2011--2013
#	C runtime module for Linux/amd64
#

	.data
	.globl	Cenviron
Cenviron:
	.quad	0

	.text
	.globl	_start
_start:
	call	C_init
	leaq	8(%rsp),%rsi	# argv
	movq	0(%rsp),%rcx	# argc
	movq	%rcx,%rax	# environ = &argv[argc+1]
	incq	%rax
	shlq	$3,%rax
	addq	%rsi,%rax
	movq	%rax,Cenviron
	pushq	%rcx
	pushq	%rsi
	pushq	$2		# __argc
	call	Cmain
	addq	$12,%rsp
	pushq	%rax
	pushq	$1
x:
	call	Cexit
	xorq	%rbx,%rbx
	divq	%rbx
	jmp	x

# internal switch(expr) routine
# %rsi = switch table, %rax = expr

	.globl	switch
switch:
	pushq	%rsi
	movq	%rdx,%rsi
	movq	%rax,%rbx
	cld
	lodsq
	movq	%rax,%rcx
next:
	lodsq
	movq	%rax,%rdx
	lodsq
	cmpq	%rdx,%rbx
	jnz	no
	popq	%rsi
	jmp	*%rax
no:
	loop	next
	lodsq
	popq	%rsi
	jmp	*%rax

# int setjmp(jmp_buf env);

	.globl	Csetjmp
Csetjmp:
	movq	16(%rsp),%rdx
	movq	%rsp,(%rdx)
	addq	$8,(%rdx)
	movq	%rbp,8(%rdx)
	movq	(%rsp),%rax
	movq	%rax,16(%rdx)
	xorq	%rax,%rax
	ret

# void longjmp(jmp_buf env, int v);

	.globl	Clongjmp
Clongjmp:
	movq	16(%rsp),%rax
	movq	24(%rsp),%rdx
	movq	(%rdx),%rsp
	movq	8(%rdx),%rbp
	movq	16(%rdx),%rdx
	jmp	*%rdx

# int _exit(int rc);

	.globl	C_exit
C_exit:
	movq	16(%rsp),%rdi
	movq	$231,%rax
	syscall
	ret

# int _sbrk(int size);

	.data
curbrk:
	.quad	0

	.text
	.globl	C_sbrk
C_sbrk:
	cmpq	$0,curbrk
	jnz	sbrk
	xorq	%rdi,%rdi	# get break
	movq	$12,%rax	# brk
	syscall
	movq	%rax,curbrk
sbrk:
	cmpq	$0,16(%rsp)
	jnz	setbrk
	mov	curbrk,%rax	# size==0, return break
	ret
setbrk:
	movq	curbrk,%rdi	# set new break
	addq	16(%rsp),%rdi
	movq	$12,%rax	# brk
	syscall
	cmpq	%rax,curbrk	# brk(x)==curbrk -> error
	jnz	sbrkok
	movq	$-1,%rax
	ret
sbrkok:
	movq	curbrk,%rbx	# update curr. break
	movq	%rax,curbrk
	movq	%rbx,%rax
	ret

# int _write(int fd, char *buf, int len);

	.globl	C_write
C_write:
	movq	16(%rsp),%rdx
	movq	24(%rsp),%rsi
	movq	32(%rsp),%rdi
	movq	$1,%rax
	syscall
	ret

# int _read(int fd, char *buf, int len);

	.globl	C_read
C_read:
	movq	16(%rsp),%rdx
	movq	24(%rsp),%rsi
	movq	32(%rsp),%rdi
	movq	$0,%rax
	syscall
	ret

# int _lseek(int fd, int pos, int how);

	.globl	C_lseek
C_lseek:
	movq	16(%rsp),%rdx
	movq	24(%rsp),%rsi
	movq	32(%rsp),%rdi
	movq	$8,%rax
	syscall
	ret

# int _creat(char *path, int mode);

	.globl	C_creat
C_creat:
	movq	16(%rsp),%rsi
	movq	24(%rsp),%rdi
	movq	$85,%rax
	syscall
	ret

# int _open(char *path, int flags);

	.globl	C_open
C_open:
	movq	16(%rsp),%rsi
	movq	24(%rsp),%rdi
	movq	$2,%rax
	syscall
	ret

# int _close(int fd);

	.globl	C_close
C_close:
	movq	16(%rsp),%rdi
	movq	$3,%rax
	syscall
	ret

# int _unlink(char *path);

	.globl	C_unlink
C_unlink:
	movq	16(%rsp),%rdi
	movq	$87,%rax
	syscall
	ret

# int _rename(char *old, char *new);

	.globl	C_rename
C_rename:
	movq	16(%rsp),%rsi
	movq	24(%rsp),%rdi
	mov	$82,%rax
	syscall
	ret

# int _fork(void);

	.globl	C_fork
C_fork:
	movq	$57,%rax
	syscall
	ret

# int _wait(int *rc);

	.globl	C_wait
C_wait:
	movq	$-1,%rdi
	movq	16(%rsp),%rsi
	movq	$0, (%rsi)
	xorq	%rdx,%rdx
	xorq	%r10,%r10
	movq	$61,%rax	# wait4
	syscall
	ret

# int _execve(char *path, char *argv[], char *envp[]);

	.globl	C_execve
C_execve:
	movq	16(%rsp),%rdx
	movq	24(%rsp),%rsi
	movq	32(%rsp),%rdi
	movq	$59,%rax
	syscall
	ret

# int _time(void);

	.globl	C_time
C_time:
	leaq	-16(%rsp),%rdi
	xorq	%rsi,%rsi
	movq	$96,%rax	# gettimeofday
	syscall
	movq	-16(%rsp),%rax
	ret

# int raise(int sig);

	.globl	Craise
Craise:
	movq	$39,%rax	# getpid
	syscall
	movq	%rax,%rdi
	movq	16(%rsp),%rsi
	movq	$62,%rax
	syscall
	ret

# int signal(int sig, int (*fn)());

	.globl	Csignal
Csignal:
	# prepare a struct sigaction
	movq	16(%rsp),%rax
	movq	%rax,-32(%rsp)
	movq	$0x10000000,-24(%rsp)	# SA_RESTART
	xorq	%rax,%rax
	movq	%rax,-16(%rsp)
	movq	%rax,-8(%rsp)
	movq	24(%rsp),%rdi
	leaq	-32(%rsp),%rsi
	leaq	-64(%rsp),%rdx
	movq	$8,%r10
	movq	$13,%rax
	syscall
	cmpq	$0,%rax
	jz	1f
	movq	$2,%rax	# SIG_ERR
	ret
1:
	movq	-64(%rsp),%rax
	ret

