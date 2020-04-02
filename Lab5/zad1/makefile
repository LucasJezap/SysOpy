CC = gcc -Wall -std=gnu11 -ggdb

all:
	$(CC) -o pipe pipe.c
	$(CC) -o popen popen.c
	make tests

compile:
	$(CC) -o pipe pipe.c
	$(CC) -o popen popen.c

clean:
	rm pipe
	rm popen

tests:
	echo 'NOW RESULTS FOR EXEC/FORK/PIPE VERSION'
	./pipe commands
	echo 'NOW RESULTS FOR POPEN VERSION'
	./popen commands