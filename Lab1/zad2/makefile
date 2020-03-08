CC = gcc -Wall -std=c11 -ggdb

static:
		$(CC) -c my_library.c -o my_library.o -I.
		ar rcs my_library.a my_library.o

shared:
		$(CC) -c -fPIC my_library.c -o my_library.o -I.
		$(CC) -shared -fPIC -o my_library.so my_library.o

clean:
		rm -f *.o
		rm -f *.a
		rm -f *.so
		rm zad2

execute2_static:
		make static
		$(CC) -o zad2 main.c my_library.a -I.
		make execute_tests

execute2_shared:
		make shared
		$(CC) -o zad2 main.c -L. my_library.so -I.
		make execute_tests

execute_tests:
		./zad2 create_table 10000 generate_and_test "50 different medium"