# still learning about make
CC=gcc
SRC := src
LIB := lib
OBJ := obj
DIRS=$(OBJ)
EXE=ex2 catch_sig

SOURCES := $(wildcard $(SRC)/*.c, $(SRC)/week*/*.c)
OBJECTS := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SOURCES))

#all: $(EXE)
#	$(CC) -I$(SRC) $< -o $(OBJ)/$@

all: $(DIRS) $(OBJECTS) $(EXE)

ex2: ex2.c
	$(CC) $< -o $@ -pthread -Wall

catch_sig: catch_sig.c
	$(CC) $< -o $@ -pthread -Wall

# EXE

# dirs
$(OBJ)/%/:
	mkdir $@

# objects
$(OBJ)/%.o: $(SRC)/%.c
	$(CC) -I$(SRC) $< -c -o $@

.PHONY: clean

clean:
	rm -f $(OBJ)/*.o $(EXE) $(OBJ)/week*/*.o
	rmdir $(DIRS)
