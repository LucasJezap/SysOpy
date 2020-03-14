CC = gcc -Wall -std=c11 -ggdb

execute: 
	$(CC) -o zad1 main.c
	make execute_tests

clean:
	find . -maxdepth 1 ! -name 'main.c' ! -name 'makefile' ! -name 'wyniki2' -type f -exec rm -v {} +

execute_tests:
	find . -type f -name wyniki2 -exec rm {} \;
	./zad1 "/home/yyy/Desktop" -atime -5 -mtime -5 -maxdepth 3 >> wyniki2
	make clean