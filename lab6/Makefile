CC=gcc
FLAGS = -Wall -g -fno-stack-protector -m32 -z execstack
EXE := vulnerable shell mys

vulnerable: vulnerable.c
	$(CC) $^ -o $@ $(FLAGS)

shell: shell.c
	$(CC) $^ -o $@ $(FLAGS)

mys: mystack.c
	$(CC) $^ -o $@ $(FLAGS)

.PHONY: clean
clean:
	rm -rf *.o $(EXE)
