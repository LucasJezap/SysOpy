CC = gcc -Wall -std=gnu11 -ggdb

all:
	$(CC) -o producer producer.c
	$(CC) -o consumer consumer.c
	$(CC) -o tester tester.c

compile:
	$(CC) -o producer producer.c
	$(CC) -o consumer consumer.c
	$(CC) -o tester tester.c

clean:
	rm consumer
	rm producer
	rm tester
