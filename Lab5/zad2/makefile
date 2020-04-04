CC = gcc -Wall -std=gnu11 -ggdb

all:
	$(CC) -o popen popen.c
	make tests

compile:
	$(CC) -o popen popen.c

clean:
	rm popen

tests:
	./popen example