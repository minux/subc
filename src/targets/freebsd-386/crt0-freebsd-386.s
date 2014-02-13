#
#	NMH's Simple C Compiler, 2011--2014
#	C runtime module for FreeBSD/386
#

# FreeBSD voodoo stuff, mostly copied from the x86-64 port, good luck!

	.section .note.ABI-tag,"a",@note
	.align	4
abitag: .long	8, 4, 1
	.string	"FreeBSD"
	.long	802000
	.data
	.p2align 2
	.globl	__progname
	.globl	environ
environ:
	.long	0
__progname:
	.long   0

# End of voodoo stuff

	.data
	.globl	Cenviron
Cenviron:
	.long	0

	.text
	.globl	_start
_start:	call	C_init
	leal	4(%esp),%esi	# argv
	movl	0(%esp),%ecx	# argc
	movl	%ecx,%eax	# environ = &argv[argc+1]
	incl	%eax
	shll	$2,%eax
	addl	%esi,%eax
	movl	%eax,Cenviron
	pushl	%ecx
	pushl	%esi
	pushl	$2		# __argc
	call	Cmain
	addl	$12,%esp
	pushl	%eax
	pushl	$1
x:	call	Cexit
	xorl	%ebx,%ebx
	divl	%ebx
	jmp	x

# internal switch(expr) routine
# %esi = switch table, %eax = expr

	.globl	switch
switch:	pushl	%esi
	movl	%edx,%esi
	movl	%eax,%ebx
	cld
	lodsl
	movl	%eax,%ecx
next:	lodsl
	movl	%eax,%edx
	lodsl
	cmpl	%edx,%ebx
	jnz	no
	popl	%esi
	jmp	*%eax
no:	loop	next
	lodsl
	popl	%esi
	jmp	*%eax

# int setjmp(jmp_buf env);

	.globl	Csetjmp
Csetjmp:
	movl	8(%esp),%edx
	movl	%esp,(%edx)
	addl	$4,(%edx)
	movl	%ebp,4(%edx)
	movl	(%esp),%eax
	movl	%eax,8(%edx)
	xorl	%eax,%eax
	ret

# void longjmp(jmp_buf env, int v);

	.globl	Clongjmp
Clongjmp:
	movl	8(%esp),%eax
	movl	12(%esp),%edx
	movl	(%edx),%esp
	movl	4(%edx),%ebp
	movl	8(%edx),%edx
	jmp	*%edx

# void _exit(int rc);

	.globl	C_exit
C_exit:	pushl	8(%esp)
	pushl	$0
	movl	$1,%eax		# SYS_exit
	int	$0x80
	addl	$8,%esp
	ret

# int _sbrk(int size);

	.data
	.extern	end
curbrk:	.long	end

	.text
	.globl	C_sbrk
C_sbrk:	movl	curbrk,%eax
	addl	8(%esp),%eax
	pushl	%eax
	pushl	%eax
	movl	$17,%eax	# SYS_break
	int	$0x80
	jnc	brkok
	addl	$8,%esp
	movl	$-1,%eax
	ret
brkok:	addl	$8,%esp
	movl	curbrk,%eax
	movl	8(%esp),%ebx
	addl	%ebx,curbrk
	ret

# int _write(int fd, void *buf, int len);

	.globl	C_write
C_write:
	pushl	8(%esp)
	pushl	16(%esp)
	pushl	24(%esp)
	pushl	$0
	movl	$4,%eax		# SYS_write
	int	$0x80
	jnc	wrtok
	negl	%eax
wrtok:	addl	$16,%esp
	ret

# int _read(int fd, void *buf, int len);

	.globl	C_read
C_read:	pushl	8(%esp)
	pushl	16(%esp)
	pushl	24(%esp)
	pushl	$0
	movl	$3,%eax		# SYS_read
	int	$0x80
	jnc	reaok
	negl	%eax
reaok:	addl	$16,%esp
	ret

# int _lseek(int fd, int pos, int how);

	.globl	C_lseek
C_lseek:
	pushl	8(%esp)
	movl	16(%esp),%eax
	cdq
	pushl	%edx		# off_t, high word
	pushl	%eax		# off_t, low word
	pushl	28(%esp)
	pushl	$0
	movl	$478,%eax	# SYS_lseek
	int	$0x80
	jnc	lskok
	negl	%eax
lskok:	addl	$20,%esp
	ret

# int _creat(char *path, int mode);

	.globl	C_creat
C_creat:
	pushl	8(%esp)
	pushl	$0x601		# O_CREAT | O_TRUNC | O_WRONLY
	pushl	20(%esp)
	pushl	$0
	movl	$5,%eax		# SYS_open
	int	$0x80
	jnc	crtok
	negl	%eax
crtok:	addl	$16,%esp
	ret

# int _open(char *path, int flags);

	.globl	C_open
C_open:	pushl	8(%esp)
	pushl	16(%esp)
	pushl	$0
	movl	$5,%eax		# SYS_open
	int	$0x80
	jnc	opnok
	negl	%eax
opnok:	addl	$12,%esp
	ret

# int _close(int fd);

	.globl	C_close
C_close:
	pushl	8(%esp)
	pushl	$0
	movl	$6,%eax		# SYS_close
	int	$0x80
	jnc	clsok
	negl	%eax
clsok:	addl	$8,%esp
	ret

# int _unlink(char *path);

	.globl	C_unlink
C_unlink:
	pushl	8(%esp)
	pushl	$0
	movl	$10,%eax	# SYS_unlink
	int	$0x80
	jnc	unlok
	negl	%eax
unlok:	addl	$8,%esp
	ret

# int _rename(char *old, char *new);

	.globl	C_rename
C_rename:
	pushl	8(%esp)
	pushl	16(%esp)
	pushl	$0
	movl	$128,%eax	# SYS_rename
	int	$0x80
	jnc	renok
	negl	%eax
renok:	addl	$12,%esp
	ret

# int _fork(void);

	.globl	C_fork
C_fork:	pushl	$0
	movl	$2,%eax		# SYS_fork
	int	$0x80
	jnc	frkok
	negl	%eax
frkok:	addl	$4,%esp
	ret

# int _wait(int *rc);

	.globl	C_wait
C_wait:	pushl	$0		# rusage
	pushl	$0		# options
	pushl	16(%esp)
	pushl	$-1		# wpid
	pushl	$0
	movl	$7,%eax		# SYS_wait4
	int	$0x80
	jnc	watok
	negl	%eax
watok:	addl	$20,%esp
	ret

# int _execve(char *path, char *argv[], char *envp[]);

	.globl	C_execve
C_execve:
	pushl	8(%esp)
	pushl	16(%esp)
	pushl	24(%esp)
	pushl	$0
	movl	$59,%eax	# SYS_execve
	int	$0x80
	jnc	excok
	negl	%eax
excok:	addl	$12,%esp
	ret

# int _time(void);

	.globl	C_time
C_time:	subl	$16,%esp
	pushl	%esp		# struct timespec
	pushl	$0		# CLOCK_REALTIME
	pushl	$0
	movl	$232,%eax	# SYS_clock_gettime
	int	$0x80
	jnc	timok
	negl	%eax
	addl	$28,%esp
	ret
timok:	movl	12(%esp),%eax
	addl	$28,%esp
	ret

# int raise(int sig);

	.globl	Craise
Craise:
	pushl	$0
	movl	$20,%eax	# SYS_getpid
	int	$0x80
	addl	$4,%esp
	pushl	8(%esp)
	pushl	%eax
	pushl	%eax
	movl	$37,%eax	# SYS_kill
	int	$0x80
	jnc	rasok
	negl	%eax
rasok:	addl	$12,%esp
	ret

# int signal(int sig, int (*fn)());

	.globl	Csignal
Csignal:
	subl	$24,%esp	# struct sigaction oact
	subl	$24,%esp	# struct sigaction act
	movl	56(%esp),%eax	# fn /act
	movl	%eax,(%esp)	# act.sa_handler / sa_action
	movl	$0,%eax
	movl	%eax,4(%esp)	# act.sa_flags
	movl	%eax,8(%esp)	# act.sa_mask
	movl	%eax,12(%esp)
	movl	%eax,16(%esp)
	movl	%eax,20(%esp)
	movl	60(%esp),%ebx
	movl	%esp,%esi
	movl	%esi,%edi	# oact
	addl	$24,%edi
	pushl	%edi		# oact
	pushl	%esi		# act
	pushl	%ebx		# sig
	pushl	$0
	movl	$416,%eax	# SYS_sigaction
	int	$0x80
	jnc	sacok
	addl	$64,%esp
	mov	$2,%eax		# SIG_ERR
	ret
sacok:	movl	40(%esp),%eax	# oact.sa_handler / sa_action
	addl	$64,%esp
	ret

