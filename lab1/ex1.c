/* Ex1: a simple "hello world" to inspect with `objdump`
 * - gcc ex1.c -o ex1
 * - objdump -h ex1 */
#include <stdio.h>

int g_a;	// .data if 0 else .bss
int main()
{
	const int a = 1;	// .rodata
	static int s = 8;	// .data if no 0 else .bss
	static const int b = 3;
	printf("Hello World!\n");
	getchar();

	return 0;
}
