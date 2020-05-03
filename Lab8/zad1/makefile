CC = gcc -Wall -std=gnu11 -ggdb
LIB = semaphore.o sharedArray.o sharedCounter.o -lm

compile:
	$(CC) -o semaphore.o -c semaphore.c
	$(CC) -o main main.c semaphore.o -lm -lpthread

clean:
	rm main
	rm semaphore.o
