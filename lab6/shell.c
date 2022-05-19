/* Run a shell code */
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>

char shell[] = "\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69"
	"\x6e\x89\xe3\x50\x89\xe2\x53\x89\xe1\xb0\x0b\xcd\x80";

int main()
{
	void *p = mmap(NULL, 1000, PROT_WRITE | PROT_EXEC, MAP_ANON | MAP_PRIVATE,
			0, 0);
	if(p == (void*) -1) {
		perror("mmap");
		exit(1);
	}
	strncpy(p, shell, 26);
	int (*fct)(void);
	fct = p;
	fct();
	return 0;
}
