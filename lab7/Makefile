# still learning about make
CC=gcc
FLAGS = -Wall
EXE := main

all: main

main: main.o
	$(CC) $^ -o main $(FLAGS)


.PHONY: clean

clean:
	rm -f $(EXE) *.o
