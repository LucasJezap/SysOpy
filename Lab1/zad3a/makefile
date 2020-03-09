CC = gcc -Wall -std=c11 -ggdb

static:
		$(CC) -c lib.c -o lib.o -I.
		ar rcs lib.a lib.o

shared:
		$(CC) -fPIC -c lib.c -I.
		$(CC) -shared -Wl,-soname,lib.so -o libfoo.so *.o

clean:
		rm -f *.o
		rm -f *.a
		rm -f *.so

execute3_static:
		make static
		$(CC) -o zad3 main.c lib.a -I.
		make execute_tests

execute3_shared:
		make shared
		$(CC) -o zad3 main.c -L. libfoo.so -I.
		make execute_tests

execute3_dynamic:
		make shared
		$(CC) -o zad3 main.c -L. -lfoo -ldl
		make execute_tests

execute_tests:
		echo 'small sequence of similar files' >> results3a.txt | ./zad3 create_table 1000 generate_and_test "10 similar medium" >> results3a.txt
		echo 'small sequence of different files' >> results3a.txt | ./zad3 create_table 1000 generate_and_test "10 different medium" >> results3a.txt
		echo 'medium sequence of similar files' >> results3a.txt | ./zad3 create_table 1000 generate_and_test "100 similar medium" >> results3a.txt
		echo 'medium sequence of different files' >> results3a.txt | ./zad3 create_table 1000 generate_and_test "100 different medium" >> results3a.txt
		echo 'big sequence of similar files' >> results3a.txt | ./zad3 create_table 1000 generate_and_test "250 similar medium" >> results3a.txt
		echo 'big sequence of different files' >> results3a.txt | ./zad3 create_table 1000 generate_and_test "250 different medium" >> results3a.txt
		rm zad3
		make clean


execute_all:
		find . -type f -name results3a.txt -exec rm {} \;
		echo 'RESULTS FOR STATIC LIBRARY' >> results3a.txt
		make execute3_static
		echo 'RESULTS FOR SHARED LIBRARY' >> results3a.txt
		make execute3_shared
		echo 'RESULTS FOR DYNAMIC LIBRARY' >> results3a.txt
		make execute3_dynamic