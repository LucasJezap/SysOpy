CC = gcc -Wall -std=c11 -ggdb

execute:
	$(CC) -o generator generator.c
	$(CC) -o macierz macierz.c
	$(CC) -o checker checker.c
	make execute_tests

clean:
	find . -maxdepth 1 ! -name 'checker' ! -name 'checker.c' ! -name 'wyniki2' ! -name 'macierz' ! -name 'generator' ! -name 'macierz.c' ! -name 'makefile' ! -name 'generator.c' ! -name 'list' -type f -exec rm -v {} +

execute_tests:
	rm wyniki2
	./generator 30 4 5
	./macierz list 2 2 0 >> wyniki2
	./checker list 30 0 >> wyniki2
	make clean
	

	
