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
		$(CC) -o zad3 -O main.c lib.a -I.
		make execute_tests

execute3_shared:
		make shared
		$(CC) -o zad3 -O main.c -L. libfoo.so -I.
		make execute_tests

execute3_dynamic:
		make shared
		$(CC) -o zad3 -O main.c -L. -lfoo -ldl
		make execute_tests

execute3_staticO3:
		make static
		$(CC) -o zad3 -O3 main.c lib.a -I.
		make execute_tests

execute3_sharedO3:
		make shared
		$(CC) -o zad3 -O3 main.c -L. libfoo.so -I.
		make execute_tests

execute3_dynamicO3:
		make shared
		$(CC) -o zad3 -O3 main.c -L. -lfoo -ldl
		make execute_tests

execute3_staticOs:
		make static
		$(CC) -o zad3 -Os main.c lib.a -I.
		make execute_tests

execute3_sharedOs:
		make shared
		$(CC) -o zad3 -Os main.c -L. libfoo.so -I.
		make execute_tests

execute3_dynamicOs:
		make shared
		$(CC) -o zad3 -Os main.c -L. -lfoo -ldl
		make execute_tests

execute_tests:
		echo 'small sequence of similar files' >> results3b.txt | ./zad3 create_table 1000 generate_and_test "10 similar medium" >> results3b.txt
		echo 'small sequence of different files' >> results3b.txt | ./zad3 create_table 1000 generate_and_test "10 different medium" >> results3b.txt
		echo 'medium sequence of similar files' >> results3b.txt | ./zad3 create_table 1000 generate_and_test "100 similar medium" >> results3b.txt
		echo 'medium sequence of different files' >> results3b.txt | ./zad3 create_table 1000 generate_and_test "100 different medium" >> results3b.txt
		echo 'big sequence of similar files' >> results3b.txt | ./zad3 create_table 1000 generate_and_test "250 similar medium" >> results3b.txt
		echo 'big sequence of different files' >> results3b.txt | ./zad3 create_table 1000 generate_and_test "250 different medium" >> results3b.txt
		rm zad3
		make clean


execute_all:
		find . -type f -name results3b.txt -exec rm {} \;
		echo 'RESULTS FOR OPTIMALIZATION O0' >> results3b.txt
		echo 'RESULTS FOR STATIC LIBRARY' >> results3b.txt
		make execute3_static
		echo 'RESULTS FOR SHARED LIBRARY' >> results3b.txt
		make execute3_shared
		echo 'RESULTS FOR DYNAMIC LIBRARY' >> results3b.txt
		make execute3_dynamic
		echo 'RESULTS FOR OPTIMALIZATION O3' >> results3b.txt
		echo 'RESULTS FOR STATIC LIBRARY' >> results3b.txt
		make execute3_staticO3
		echo 'RESULTS FOR SHARED LIBRARY' >> results3b.txt
		make execute3_sharedO3
		echo 'RESULTS FOR DYNAMIC LIBRARY' >> results3b.txt
		make execute3_dynamicO3
		echo 'RESULTS FOR OPTIMALIZATION Os' >> results3b.txt
		echo 'RESULTS FOR STATIC LIBRARY' >> results3b.txt
		make execute3_staticOs
		echo 'RESULTS FOR SHARED LIBRARY' >> results3b.txt
		make execute3_sharedOs
		echo 'RESULTS FOR DYNAMIC LIBRARY' >> results3b.txt
		make execute3_dynamicOs