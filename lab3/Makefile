# still learning about make
CC=gcc
SRC := .
OBJ := obj
DIRS=$(OBJ)
FLAGS = -Wall
EXE := p1_main p2_main p3_main curl_main guardian

SOURCES := $(wildcard $(SRC)/*.c, $(SRC)/week*/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

#all: $(EXE)
#	$(CC) -I$(SRC) $< -o $(OBJ)/$@

all: $(DIRS) $(OBJECTS)

p1: inotify/in.c
	$(CC) $^ -o p1_main $(FLAGS)

p2: sha256/test.c sha256/sha-256.c sha256/sha-256.h
	$(CC) $^ -o p2_main $(FLAGS)

curl_main: curl/curl.c
	$(CC) $^ -o curl_main $(FLAGS) -lcurl

p3: curl.o sha.o main.o

curl.o: curl/curl.c
	$(CC) $^ -c -o $@ $(FLAGS) -lcurl

guardian: watcher/main.c watcher/sha-256.c
	$(CC) $^ -o $@ $(FLAGS)

# dirs
$(OBJ):
	mkdir $@

# objects
$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -I$(SRC) $< -c -o $@

.PHONY: clean

clean:
	rm -f $(OBJ)/*.o $(EXE) $(OBJ)/week*/*.o
	rmdir $(DIRS)
