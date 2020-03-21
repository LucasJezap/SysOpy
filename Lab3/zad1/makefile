CC = gcc -Wall -std=c11 -ggdb

execute: 
	$(CC) -o zad1 main.c
	make execute_tests

clean:
	find . -maxdepth 1 ! -name 'main.c' ! -name 'makefile' ! -name 'wyniki1' -type f -exec rm -v {} +

execute_tests:
	./zad1 "/home/yyy/Desktop" -atime -5 -mtime +5 -maxdepth 3
	make clean