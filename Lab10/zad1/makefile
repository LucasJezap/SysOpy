CC = gcc -Wall -std=gnu11 -ggdb

compile:
	$(CC) -o utils.o -c utils.c
	$(CC) -o server server.c utils.o -lpthread
	$(CC) -o client client.c utils.o -lpthread

clean:
	rm server
	rm client
	rm utils.o