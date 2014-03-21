#include <stdio.h>

#define K16	16384

void ufseek(FILE *fd, int pos, int how) {
	if (pos < 0) {
		fseek(fd, K16, how);
		how = SEEK_CUR;
		pos -= K16;
		fseek(fd, K16, how);
		pos -= K16;
	}
	fseek(fd, pos, how);
}
