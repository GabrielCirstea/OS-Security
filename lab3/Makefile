# still learning about make
CC=gcc
FLAGS = -Wall
EXE := p1_main p2_main p3_main curl_main guardian

all: p1 p2 curl_main guardian

p1: inotify/in.c
	$(CC) $^ -o p1_main $(FLAGS)

p2: sha256/test.c sha256/sha-256.c sha256/sha-256.h
	$(CC) $^ -o p2_main $(FLAGS)

curl_main: curl/curl.c
	$(CC) $^ -o curl_main $(FLAGS) -lcurl

curl.o: curl/curl.c
	$(CC) $^ -c -o $@ $(FLAGS) -lcurl

guardian: watcher/main.c watcher/sha-256.c watcher/curl.h
	$(CC) $^ -o $@ $(FLAGS) -lcurl

.PHONY: clean

clean:
	rm -f $(EXE)
