CC = gcc -Wall -std=gnu11 -ggdb
LIB = semaphore.o sharedArray.o sharedCounter.o -lm -lrt -lpthread

libraries:
	$(CC) -o semaphore.o -c semaphore.c
	$(CC) -o sharedArray.o -c sharedArray.c
	$(CC) -o sharedCounter.o -c sharedCounter.c

compile:
	make libraries
	$(CC) -o main main.c $(LIB)
	$(CC) -o packer packer.c $(LIB)
	$(CC) -o receiver receiver.c $(LIB)
	$(CC) -o sender sender.c $(LIB)

run:
	make compile
	./main 4 3 2

clean:
	rm main
	rm packer
	rm receiver
	rm sender
	rm *.o
