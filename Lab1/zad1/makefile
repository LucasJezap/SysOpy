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