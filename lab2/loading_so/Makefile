default: 
	gcc library.c -shared -fPIC -o library.so
	gcc main.c -o main -ldl

.PHONY: clean

clean:
	rm library.so main
