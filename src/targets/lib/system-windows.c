/*
 *	NMH's Simple C Compiler, 2013
 *	system() for Windows/386
 */

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <limits.h>
#include <windows.h>
#include <stdio.h>
#include <errno.h>

static char system_buf[0x10000];

int system(char *cmd) {
	char	*p;
	int	ret;
	struct _STARTUPINFO		si;
	struct _PROCESS_INFORMATION	pi;

	memset(&si, 0, sizeof(struct _STARTUPINFO));
	memset(&pi, 0, sizeof(struct _PROCESS_INFORMATION));
	si.cb = sizeof(struct _STARTUPINFO);
	strcpy(system_buf, "cmd.exe /c ");
	p = system_buf + 11;
	while (*cmd) {
		*p = *cmd;
		p++;
		cmd++;
	}
	*p = '\0';
	if (	!CreateProcessA((char*)0, system_buf, (void*)0, (void*)0, 
		0, 0, (void*)0, (void*)0, &si, &pi))
	{
		return -1;
	}
	WaitForSingleObject(pi.hProcess, 0xFFFFFFFF);
	GetExitCodeProcess(pi.hProcess, &ret);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return ret;
}
