CC=gcc
FLAGS = -Wall -g -fno-stack-protector -m32
EXE := vulnerable shell

vulnerable: vulnerable.c
	$(CC) $^ -o $@ $(FLAGS)

shell: shell.c
	$(CC) $^ -o $@ $(FLAGS)

.PHONY: clean
clean:
	rm -rf *.o $(EXE)
