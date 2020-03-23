CC = gcc -Wall -std=c11 -ggdb

execute:
	$(CC) -o generator generator.c
	$(CC) -o macierz macierz.c
	$(CC) -o checker checker.c
	make execute_tests

clean:
	find . -maxdepth 1 ! -name 'checker.c' ! -name 'wyniki3' ! -name 'macierz.c' ! -name 'makefile' ! -name 'generator.c'  -type f -exec rm -v {} +

execute_tests:
	rm wyniki3
	./generator 30 4 5
	./macierz list 2 2 0 2 10 >> wyniki3
	./checker list 30 0 >> wyniki3
	make clean


	
