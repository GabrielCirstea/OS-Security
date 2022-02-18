/* Ex2 (ex1 from threads) from first lab: allocate mem with mmap, read only and
 * try to access it use a sig heandler to catch the "segfault" signal and
 * change the permision for the memory
 * ----thread----
 *  the thread part is to see where the handler is executed, looks like it is
 *  executed by each thread when the sigfault arise */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <malloc.h>
#include <sys/mman.h>
#include <signal.h>
#include <pthread.h>

// "ignore" SEGSEGV
// try to change the mem protection on the page
void ign_segv(int sig, siginfo_t *si, void *unused)
{
	fprintf(stderr, "ign_segv: in pid: %d and tid: %ld\n", getpid(), pthread_self());
	fprintf(stderr, "ign_segv: Got a signal %d for address %p\n", sig, si->si_addr);
	char *boundry = si->si_addr - ((long)si->si_addr % getpagesize());
	if(mprotect(boundry, 1024, PROT_WRITE)) {
		perror("ign_segv: mprotect");
		exit(1);
	}
	fprintf(stderr, "ign_segv: Changed mem prot\n");
}

int make_sigaction()
{
	// sigaction struct
	struct sigaction sa;

	// flag to use the sigaction function and give more info
	sa.sa_flags = SA_SIGINFO;
	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = ign_segv;
	if (sigaction(SIGSEGV, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	return 0;
}

void *thread1(void *arg)
{
	char *addr = (char*)arg;
	printf("thread1: try access memory\n");
	// access memory after the first page
	addr[5000] = 'a';

	return NULL;
}

int main()
{
	make_sigaction();

	// map some memory
	const int mem_size = 4 * getpagesize();
	char *addr = mmap(NULL, mem_size, PROT_READ, MAP_ANON | MAP_PRIVATE, 0, 0);
	if(addr == (void*)-1) {
		perror("mmap");
		exit(1);
	}

	pthread_t tid;
	if(pthread_create(&tid, NULL, thread1, addr) == -1) {
		perror("pthread_create");
		exit(1);
	}
	printf("tid: %ld\n", tid);

	// test the memory write permision
	int *v = (int*)addr;
	v[0] = 3;
	printf("v[0] = %d\n", v[0]);


	if(pthread_join(tid, NULL)) {
		perror("pthread_join");
		exit(1);
	}

	// clean up
	if(munmap(addr, mem_size)) {
		perror("munmap");
		exit(1);
	}

	return 0;
}
