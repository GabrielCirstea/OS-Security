/* Purpose: What happend if I send a signal to a procces with more threads?
 * Who wil catch the signal?
 * --------------------
 *  Looks like if the signal comes from another process, the main thread will catch
 *  the signal, but if the signal is send from a thread to another using `pthread_kill`
 *  the targeted thread will catch the signal */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>

#define errExit(x) do { perror(x); exit(1);\
				} while(0)

void int_handl(int sig, siginfo_t *si, void *unused)
{
	fprintf(stderr, "int_handl: in pid: %d and tid: %ld\n", getpid(), pthread_self());
}

int make_sigaction()
{
	// sigaction struct
	struct sigaction sa;

	// flag to use the sigaction function and give more info
	sa.sa_flags = SA_SIGINFO;
	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = int_handl;
	if (sigaction(SIGINT, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	return 0;
}

void *thread_func(void *arg)
{
	int nr = *(int*)arg;
	printf("Thread %d ready to go\n", nr);
	sleep(10);
	printf("Thread %d out\n", nr);

	return NULL;
}

int main()
{
	printf("Starting pid: %d and tid: %ld\n", getpid(), pthread_self());
	make_sigaction();
	const int t_nr = 3;
	pthread_t *tids = malloc(t_nr * sizeof(*tids));
	int nums[] = {1, 2, 3, 4};
	if(!tids)
		errExit("malloc");
	for(int i=0; i<t_nr; ++i) {
		if(pthread_create(&tids[i], NULL, thread_func, &nums[i]))
			errExit("pthread_create");
	}
	sleep(1);
	pthread_kill(tids[1], SIGINT);

	for(int i=0; i<t_nr; ++i) {
		if(pthread_join(tids[i], NULL))
			errExit("pthread_join");
	}
	return 0;
}
