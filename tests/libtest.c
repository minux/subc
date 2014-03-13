/*
 * Ad-hoc libc test program.
 * Nils M Holm, 1995, 2014
 * In the public domain
 *
 * The executable program *must* be called "libtest" or
 * the stdout test will fail.
 *
 * The program will delete the file stdio.tmp silently!
 */

/* Todo:
 * abort atexit clearerr ctime ctype difftime fdopen ferror
 * fflush fgetpos fprintf fputs fread freopen fscanf fsetpos fwrite
 * getchar getenv memchr perror putchar puts rand realloc remove rename
 * scanf setbuf setvbuf sprintf sscanf strcspn strdup strerror strpbrk
 * strspn strtok strtol system time tmpfile tmpnam varargs vformat
 * vfprintf vprintf vscan vsprintf
 */

#define TMPFILE	"stdio.tmp"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <errno.h>
#include <limits.h>

int	Errors = 0;

void fail(char *name) {
	fputs(name, stdout);
	puts(" failed!");
	Errors++;
}

void test_memfn(void) {
	char	*c1, *c2;
	char	v1[128];
	int	i;

	c1 = "test01";
	c2 = "test03";
	if (memcmp(c1, c2, 5)) fail("memcmp-1");
	if (!memcmp(c1, c2, 6)) fail("memcmp-2");
	if (memcmp(c1, c2, 6) != -2) fail("memcmp-3");
	if (memcmp(c2, c1, 6) != 2) fail("memcmp-4");

	c1 = "abcdefghijklmnopqrstuvwxyz0123456789";
	memcpy(v1, c1, 36);
	if (memcmp(c1, v1, 36)) fail("memcpy-1");
	memcpy(v1+18, v1, 36);
	if (memcmp(c1, v1+18, 18)) fail("memcpy-2");
	memcpy(v1, v1+18, 36);
	if (memcmp(c1, v1, 18)) fail("memcpy-3");

	memcpy(v1, c1, 36);
	memmove(v1+1, v1, 35);
	if (memcmp(v1+1, c1, 35) || *v1 != 'a') fail("memmove-1");
	memcpy(v1, c1, 36);
	memmove(v1, v1+1, 35);
	if (memcmp(v1, c1+1, 35) || v1[35] != '9') fail("memmove-2");

	for (i=0; i<128; i++) v1[i] = -1;
	memset(v1, 0, 64);
	if (v1[63]) fail("memset-1");
	if (!v1[64]) fail("memset-2");
	for (i=0; i<128; i++) v1[i] = -1;
	memset(v1, 123, 64);
	if (v1[63] != 123) fail("memset-3");
	if (v1[64] == 123) fail("memset-4");
}

void test_indx(void) {
	char	*c1, *p1;

	c1 = "...............X1..............X2";
	p1 = strchr(c1, 'X');
	if (!p1 || *p1 != 'X' || p1[1] != '1') fail("strchr-1");
	if (strchr(c1, 'Z')) fail("strchr-2");

	p1 = strrchr(c1, 'X');
	if (!p1 || *p1 != 'X' || p1[1] != '2') fail("strrchr-1");
	if (strrchr(c1, 'Z')) fail("strchr-2");
}

void test_dmem(void) {
	char	*segp[64], *a;
	int	i, j;

	for (i=0; i<32; i++) {
		if ((segp[i] = malloc(1024)) == NULL)
			break;
		for (j=0; j<1024; j++)
			segp[i][j] = i;
	}
	if (i < 32) fail("malloc-1");
	for (i=0; i<32; i++) {
		for (j=0; j<1024; j++)
			if (segp[i][j] != i)
				break;
		if (j != 1024)
			break;
	}
	if (i < 32) fail("malloc-2");

	for (j=0; j<i; j++)
		free(segp[j]);

	if ((a = calloc(i, 1024)) == NULL)
		fail("calloc-1");
	for (i=0; i<1024; i++)
		if (a[i]) break;
	if (i < 1024)
		fail("calloc-2");
	free(a);
}

int qcmp(int *a, int *b) {
	return (*a<*b)? -1: (*a>*b)? 1: 0;
}

void test_sort(void) {
	int	array[128];
	int	i, j;

	for (i=0; i<128; i++)
		array[i] = 128-i;
	qsort(array, 128, sizeof(int), qcmp);
	for (j=i=0; i<128; i++) {
		if (array[i] < j) {
			fail("qsort-1");
			break;
		}
		j = array[i];
	}
}

int	test_array[] = { 2, 3, 5, 7, 11, 13, 17, 19, 23, 27 };

void test_search(void) {
	int	key;
	int	*p;

	key = 13;
	if ((p = bsearch(&key, test_array, 10, sizeof(int), qcmp)) == NULL)
		fail("bsearch-1");
	if (*p != 13) fail("bsearch-2");
	key = 15;
	if (bsearch(&key, test_array, 10, sizeof(int), qcmp) != NULL)
		fail("bsearch-3");
}

void test_mem(void) {
	test_memfn();
	test_indx();
	test_dmem();
	test_sort();
	test_search();
}

void test_str(void) {
	char	v1[128];

	if (strlen("\0") != 0) fail("strlen-1");
	if (strlen("0123456789abcdef") != 16) fail("strlen-2");

	if (strcmp("\0", "\0")) fail("strcmp-1");
	if (strcmp("abcdef", "abcdef")) fail("strcmp-2");
	if (strcmp("abcdef", "abcdefg") != -'g') fail("strcmp-3");
	if (strcmp("abcdefg", "abcdef") != 'g') fail("strcmp-4");
	if (strcmp("abcdef0", "abcdef3") != -3) fail("strcmp-5");

	strcpy(v1, "0123456789ABCDEF");
	if (strcmp(v1, "0123456789ABCDEF")) fail("strcpy-1");
	strcpy(v1, "ABCDEF");
	if (strcmp(v1, "ABCDEF")) fail("strcpy-2");
	if (v1[7] != '7') fail("strcpy-3");

	strcpy(v1, "abcXXXXX"); v1[3] = 0;
	strcat(v1, "DEF");
	if (strcmp(v1, "abcDEF")) fail("strcat-1");
	if (v1[7] != 'X') fail("strcat-2");

	if (strncmp("abcdef", "abcdef", 6)) fail("strncmp-1");
	if (strncmp("abcdxx", "abcdyy", 4)) fail("strncmp-2");
	if (strncmp("abcdx0", "abcdy9", 5) != -1) fail("strncmp-3");
	if (strncmp("abcdy0", "abcdx9", 5) != 1) fail("strncmp-4");
	if (strncmp("abcdef", "abcdef", 10)) fail("strncmp-5");
	if (strncmp("abcdefg", "abcdef", 7) != 'g') fail("strncmp-6");
	if (strncmp("abcdef", "abcdefg", 7) != -'g') fail("strncmp-7");
	if (strncmp("abcdefg", "abcdef", 10) != 'g') fail("strncmp-8");
	if (strncmp("abcdef", "abcdefg", 10) != -'g') fail("strncmp-9");

	strcpy(v1, "0123456789");
	strncpy(v1, "abcdef", 6);
	if (strcmp(v1, "abcdef6789")) fail("strncpy-1");
	strcpy(v1, "0123456789");
	strncpy(v1, "0123", 5);
	if (memcmp(v1, "0123\00056789", 10)) fail("strncpy-2");
	strncpy(v1, "0123", 10);
	if (memcmp(v1, "0123\00056789", 10)) fail("strncpy-3");

	strcpy(v1, "012345");
	strncat(v1, "6789", 5);
	if (strcmp(v1, "0123456789")) fail("strncat-1");
	strncat(v1, "abcdef0000", 6);
	if (strcmp(v1, "0123456789abcdef")) fail("strncat-2");

	/* sprintf() , strspn(), strtok(), strcspn() */
}

void test_math(void) {
	int	i;

	i = 12345;
	if (i != atoi("   12345")) fail("atoi-1");
	if (i != atoi("\t+12345")) fail("atoi-2");
	i = -i;
	if (i != atoi("-12345")) fail("atoi-3");
	if (i != atoi("  -12345")) fail("atoi-4");
	if (i != atoi("\t-12345")) fail("atoi-5");

	if (abs(0) != 0) fail("abs-1");
	if (abs(123) != 123) fail("abs-2");
	if (abs(-456) != 456) fail("abs-3");
	if (abs(INT_MIN) != INT_MIN) fail("abs-4"); /* man page says so */
}

void test_misc(void) {
	test_math();
}

jmp_buf	here;
int	count;

void jump(void) {
	longjmp(&here, 1);
	fail("longjmp-1");
}

void test_ljmp(void) {
	count = 0;
	if (setjmp(&here)) {
		if (count != 1) fail("setjmp-1");
		return;
	}
	count++;
	jump();
}

void test_proc(void) {
	test_ljmp();
}

void test_sio1(void) {
	FILE	*f;
	char	buf[80];
	int	i, j;
	int	fd;

	if ((f = fopen(TMPFILE, "w+")) == NULL) {
		fail("fopen-1");
	}
	else {
		fgetc(f);
		if (!feof(f)) fail("feof-1");
		fclose(f);
	}
	if ((f = fopen(TMPFILE, "w")) == NULL) {
		fail("fopen-2");
	}
	else {
		fputs("1111111111\n", f);
		fputs("2222222222\n", f);
		fputs("3333333333\n", f);
		fputs("4444444444\n", f);
		fputs("5555555555\n", f);
		fd = fileno(f);
		if (fclose(f)) fail("fclose-1");
		if (_close(fd) == 0) fail("fclose-2");

		if ((f = fopen(TMPFILE, "r")) == NULL) {
			fail("fopen-3");
		}
		else {
			fgets(buf, 80, f);
			i = '1';
			while (!feof(f)) {
				for (j=0; j<10; j++) {
					if (buf[j] != i) {
						fail("fgets-1");
						break;
					}
				}
				i++;
				fgets(buf, 80, f);
			}
			fd = fileno(f);
			if (fclose(f)) fail("fclose-1");
			if (_close(fd) == 0) fail("fclose-2");
		}
	}
	_unlink(TMPFILE);
}

void test_sio2(void) {
	FILE	*f;
	int	i;

	if (fileno(stdin) != 0) fail("fileno-1");
	if (fileno(stdout) != 1) fail("fileno-2");
	if (fileno(stderr) != 2) fail("fileno-3");

	if ((f = fopen(TMPFILE, "w+")) == NULL) {
		fail("fopen-4");
		return;
	}

	for (i='A'; i<='Z'; i++)
		if (fputc(i, f) != i)
			fail("fputc-1");

	if (fflush(f) < 0) fail("fflush-1");

	rewind(f);
	if (_lseek(fileno(f), 0, SEEK_CUR) != 0)
		fail("rewind-1");

	for (i='A'; i<='Z'; i++) {
		if (fgetc(f) != i) {
			fail("fgetc-1");
			break;
		}
	}
	if (fgetc(f) != EOF) fail("fgetc-2");
	
	clearerr(f);
	if (ungetc('X', f) != 'X') fail("ungetc-1");
	if (fgetc(f) != 'X') fail("ungetc-2");
	if (ungetc('X', f) != 'X') fail("ungetc-3");
	if (ungetc('X', f) != EOF) fail("ungetc-4");
	if (fgetc(f) != 'X') fail("ungetc-5");

	if (fclose(f)) fail("fclose-5");

	_unlink(TMPFILE);
}

void test_sio3(void) {
	FILE	*f;
	static char	buf[1024];
	int	i;

	if ((f = fopen(TMPFILE, "w+")) == NULL) {
		fail("fopen-5");
		return;
	}

	for (i=0; i<1024; i++)
		buf[i] = '5';

	for (i=0; i<16; i++) {
		if (fwrite(buf, 1, 1024, f) != 1024) {
			fail("fwrite-1");
			break;
		}
	}

	rewind(f);
	for (i=0; i<16; i++) {
		if (fread(buf, 1, 1024, f) != 1024) {
			fail("fread-1");
			break;
		}
	}
	if (fgetc(f) != EOF) fail("fread-2");

	clearerr(f);
	rewind(f);
	if (fseek(f, 0, SEEK_END) != 16384) fail("fseek-1");
	if (ftell(f) != 16384) fail("ftell-1");
	if (fseek(f, 8100, SEEK_SET) != 8100) fail("fseek-2");
	if (ftell(f) != 8100) fail("fseek-3");
	if (fseek(f, 1900, SEEK_CUR) != 10000) fail("fseek-4");
	if (ftell(f) != 10000) fail("fseek-5");

	fclose(f);
	_unlink(TMPFILE);
}

void test_stdout(void) {
	int	i;

	puts("0---|----1----|----2----|----3----|----4----|----5");
	for (i=0; i<50; i++) putc('A', stdout);
	puts("");
	for (i=0; i<50; i++) putchar('B');
	puts("");
	printf("CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC\n");
	printf("|%48s|\n", "DDDDDDDDDDDDDDDDDDDDDDDD");
	printf("|%-48s|\n", "EEEEEEEEEEEEEEEEEEEEEEEE");
	for (i=0; i<5; i++)
		printf("%c%c%c%c%c%c%c%c%c%c",
		'1', '2', '3', '4', '5', '6', '7', '8', '9', '0' );
	puts("");
	printf("%50d\n", -12345);
	printf("%-49d|\n", -12345);
	printf("%050d\n", 12345);
	printf("0x%x %15s%d%15s 0%o\n", 0xABCD, "", 0xABCD, "", 0xABCD);
	for (i=0; i<5; i++) printf("%%%%%%%%%%%%%%%%%%%%");
	puts("");
	puts("0---|----1----|----2----|----3----|----4----|----5");
}

void test_sio4(void) {
	FILE	*t, *f;
	char	s1[80], s2[80];
	int	err, lno;

	err = lno = 0;
	system("./libtest test-stdout >stdio.tmp");
	if ((f = fopen("stdio.ok", "r")) == NULL) {
		fputs("missing file: stdio.ok\n", stdout);
		Errors++;
	}
	else {
		if ((t = fopen(TMPFILE, "r")) == NULL) {
			fputs("could not open test file: stdio.tmp\n",
				stdout);
			fputs("system() failed?\n", stdout);
		}
		else {
			fgets(s1, 80, t);
			for (;;) {
				++lno;
				fgets(s2, 80, f);
				if (feof(t)) break;
				if (feof(f)) break;
				if (memcmp(s1, s2, 52)) ++err;
				fgets(s1, 80, t);
			}
			if (!feof(f) || !feof(t)) {
				puts("test files have different sizes");
				++err;
			}
			fclose(t);
		}
		fclose(f);
	}
	if (!err) _unlink(TMPFILE);
	if (err) fail("printf");
}

void test_stdio(void) {
	test_sio1();
	test_sio2();
	test_sio3();
	test_sio4();
}

void test_exit(void) {
	exit(Errors? 1: 0);
	fail("exit-1");
}

int main(int argc, char **argv) {
	if (argc > 1) {
		if (!strcmp(argv[1], "test-stdout")) test_stdout();
		return EXIT_SUCCESS;
	}
	test_mem();
	test_str();
	test_misc();
	test_proc();
	test_stdio();
	test_exit();
	return EXIT_FAILURE;
}
