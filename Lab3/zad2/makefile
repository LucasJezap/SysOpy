CC = gcc -Wall -std=c11 -ggdb

execute:
	$(CC) -o generator generator.c
	$(CC) -o macierz macierz.c
	$(CC) -o checker checker.c
	make execute_tests

clean:
	find . -maxdepth 1 ! -name 'checker.c' ! -name 'wyniki2' ! -name 'macierz.c' ! -name 'makefile' ! -name 'generator.c'  -type f -exec rm -v {} +

execute_tests:
	rm wyniki2
	./generator 5 5 6
	./macierz list 2 1 0 >> wyniki2
	./checker list 5 0 >> wyniki2
	make clean
	

	
