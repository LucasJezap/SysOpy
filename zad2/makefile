CC = gcc -Wall -std=c11 -ggdb

all:
	$(CC) -o fork fork.c
	$(CC) -o exec exec.c
	$(CC) -o exec2 exec2.c
	make tests

tests:
	make test2

clean:
	rm fork
	rm exec
	rm exec2

test1:
	./fork pending

test2:
	./exec pending
	