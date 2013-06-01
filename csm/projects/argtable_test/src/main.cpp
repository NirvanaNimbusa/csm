#include <stdio.h>

extern int test_01(int argc, char **argv);

int main(int argc, char **argv)
{
	char *xargv[] = {"program name", "-a", NULL};
	int xargc = sizeof(xargv) / sizeof(char*) - 1;

	test_01(xargc, xargv);
	return 0;
}