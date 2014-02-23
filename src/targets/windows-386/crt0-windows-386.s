#
#	NMH's Simple C Compiler, 2011--2014
#	C runtime module for Windows/386 via MinGW
#

# Calling conventions: stack, return in %eax

	.text

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

	.text
	.globl	CGetEnvironmentA
CGetEnvironmentA:
	call	_GetEnvironmentStringsA@0
	ret

	.globl	CCreateProcessA
CCreateProcessA:
	pushl	8(%esp)
	pushl	16(%esp)
	pushl	24(%esp)
	pushl	32(%esp)
	pushl	40(%esp)
	pushl	48(%esp)
	pushl	56(%esp)
	pushl	64(%esp)
	pushl	72(%esp)
	pushl	80(%esp)
	call	_CreateProcessA@40	
	ret

	.globl	CGetStdHandle
CGetStdHandle:
	pushl	8(%esp)
	call	_GetStdHandle@4
	ret

	.globl	CGetCommandLineA
CGetCommandLineA:
	call	_GetCommandLineA@0
	ret

	.globl	CGetSystemTime
CGetSystemTime:
	pushl	8(%esp)
	call	_GetSystemTime@4	
	ret

	.globl	CSystemTimeToFile
CSystemTimeToFile:
	pushl	8(%esp)
	pushl	16(%esp)
	call	_SystemTimeToFileTime@8	
	ret

	.globl	CHeapReAlloc
CHeapReAlloc:
	pushl	8(%esp)
	pushl	16(%esp)
	pushl	24(%esp)
	pushl	32(%esp)
	call	_HeapReAlloc@16	
	ret

	.globl	CHeapAlloc
CHeapAlloc:
	pushl	8(%esp)
	pushl	16(%esp)
	pushl	24(%esp)
	call	_HeapAlloc@12	
	ret

	.globl	CGetProcessHeap
CGetProcessHeap:
	call	_GetProcessHeap@0	
	ret

	.globl	CMoveFileA
CMoveFileA:
	pushl	8(%esp)
	pushl	16(%esp)
	call	_MoveFileA@8	
	ret

	.globl	CExitProcess
CExitProcess:
	pushl	8(%esp)
	call	_ExitProcess@4	
	ret

	.globl	CWriteFile
CWriteFile:
	pushl	8(%esp)
	pushl	16(%esp)
	pushl	24(%esp)
	pushl	32(%esp)
	pushl	40(%esp)
	call	_WriteFile@20	
	ret

	.globl	CReadFile
CReadFile:
	pushl	8(%esp)
	pushl	16(%esp)
	pushl	24(%esp)
	pushl	32(%esp)
	pushl	40(%esp)
	call	_ReadFile@20	
	ret

	.globl	CSetFilePointer
CSetFilePointer:
	pushl	8(%esp)
	pushl	16(%esp)
	pushl	24(%esp)
	pushl	32(%esp)
	call	_SetFilePointer@16	
	ret

	.globl	CCreateFileA
CCreateFileA:
	pushl	8(%esp)
	pushl	16(%esp)
	pushl	24(%esp)
	pushl	32(%esp)
	pushl	40(%esp)
	pushl	48(%esp)
	pushl	56(%esp)
	call	_CreateFileA@28	
	ret

	.globl	CDeleteFileA
CDeleteFileA:
	pushl	8(%esp)
	call	_DeleteFileA@4	
	ret

	.globl	CCloseHandle
CCloseHandle:
	pushl	8(%esp)
	call	_CloseHandle@4	
	ret

	.globl	CGetExitCodeProce
CGetExitCodeProce:
	pushl	8(%esp)
	pushl	16(%esp)
	call	_GetExitCodeProcess@8	
	ret

	.globl	CWaitForSingleObj
CWaitForSingleObj:
	pushl	8(%esp)
	pushl	16(%esp)
	call	_WaitForSingleObject@8	
	ret

	.globl	_mainCRTStartup
_mainCRTStartup:
	.globl	_WinMainCRTStartup
_WinMainCRTStartup:
	call	CmainCRTStartup
	ret

