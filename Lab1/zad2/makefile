CC = gcc -Wall -std=c11 -ggdb

static: 
		make clean
		$(CC) -c my_library.c -o my_library.o -I.
		ar rcs my_library.a my_library

shared: 
		make clean
		$(CC) -c -fPIC my_library.c -o my_library.o -I.
		$(CC) -shared -fPIC -o my_library.so my_library

clean:
		rm -f *.o
		rm -f *.a
		rm -f *.so

execute2_static:
		make static
		$(CC) -o zad2 main.c my_library.a -I.
		make execute_tests

execute2_shared:
		make shared
		$(CC) main.c -Wl,-rpath /usr/local/lib 	
		make execute_tests

execute_tests:
		find . -type f -name raport2.txt -exec rm {} \;
		echo 'small sequence of similar files' >> raport2.txt | ./zad2 create_table 1000 generate_and_test "5 similar medium" >> raport2.txt
		echo 'small sequence of different files' >> raport2.txt | ./zad2 create_table 1000 generate_and_test "5 different medium" >> raport2.txt
		echo 'medium sequence of similar files' >> raport2.txt | ./zad2 create_table 1000 generate_and_test "20 similar medium" >> raport2.txt
		echo 'medium sequence of different files' >> raport2.txt | ./zad2 create_table 1000 generate_and_test "20 different medium" >> raport2.txt
		echo 'big sequence of similar files' >> raport2.txt | ./zad2 create_table 1000 generate_and_test "50 similar medium" >> raport2.txt
		echo 'big sequence of different files' >> raport2.txt | ./zad2 create_table 1000 generate_and_test "50 different medium" >> raport2.txt
		rm zad2
		make clean
